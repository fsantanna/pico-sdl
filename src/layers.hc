#ifndef PICO_LAYERS_HC
#define PICO_LAYERS_HC

typedef enum {
    PICO_LAYER_WINDOW,
    PICO_LAYER_WORLD,
    PICO_LAYER_PLAIN,
    PICO_LAYER_VIDEO,
    PICO_LAYER_SUB,
} PICO_LAYER;

typedef struct Pico_Layer {
    PICO_LAYER            type;
    char*                 name;     // "world" for the world/root layer
    SDL_Texture*          tex;
    Pico_Layer_Pencil     pencil;
    Pico_Layer_Effect     effect;
    Pico_Layer_Scene      scene;
    struct {
        const char* up;             // parent id; NULL = root or detached
        const char* nxt;            // next sibling under same up
        struct {
            const char* fst;        // first child (back; drawn first)
            const char* lst;        // last child  (front; drawn last)
        } dn;
    } hier;
} Pico_Layer;

typedef struct {
    Pico_Layer   base;
    Pico_Abs_Dim sup;     // snapshot of source scene.dim
} Pico_Layer_Sub;

static Pico_Layer* _pico_layer_name (const char* name);
static void _layer_attach (const char* up, const char* dn);

static Pico_Layer* _pico_layer_pixmap (
    int mode, const char* key, Pico_Abs_Dim dim,
    const Pico_Color* pixels
);
static Pico_Layer* _pico_layer_image (
    int mode, const char* key, const char* path
);
static Pico_Layer* _pico_layer_text (
    int mode, const char* key, int height, const char* text
);
static void _pico_output_draw_layer (
    Pico_Layer* layer, Pico_Rel_Rect* rect
);

#endif // PICO_LAYERS_HC

#ifdef PICO_LAYERS_C

static Pico_Layer* _pico_layer_name (const char* name) {
    assert(name != NULL);
    Pico_Layer* L = (Pico_Layer*) realm_get(G.realm, strlen(name)+1, name);
    pico_assert(L!=NULL && "layer does not exist");
    return L;
}

static void _layer_attach (const char* up, const char* dn) {
    Pico_Layer* UP = _pico_layer_name(up);
    Pico_Layer* DN = _pico_layer_name(dn);
    DN->hier.up = UP->name;
    DN->hier.nxt = NULL;
    if (UP->hier.dn.fst == NULL) {
        UP->hier.dn.fst = DN->name;
        UP->hier.dn.lst = DN->name;
    } else {
        Pico_Layer* lst = (Pico_Layer*)
            realm_get(G.realm, strlen(UP->hier.dn.lst)+1, UP->hier.dn.lst);
        lst->hier.nxt = DN->name;
        UP->hier.dn.lst = DN->name;
    }
}

static void _pico_output_draw_layer (Pico_Layer*, Pico_Rel_Rect*);

static void _pico_output_draw_layers (Pico_Layer* UP) {
    Pico_Layer* old = G.layer;
    G.layer = UP;
    const char* cur = UP->hier.dn.fst;
    while (cur != NULL) {
        Pico_Layer* CUR = (Pico_Layer*) realm_get(G.realm, strlen(cur)+1, cur);
        assert(CUR != NULL);

        SDL_SetRenderTarget(G.window.ren, CUR->tex);
        _pico_output_draw_layers(CUR);

        SDL_SetRenderTarget(G.window.ren, UP->tex);
        _pico_output_draw_layer(CUR, NULL);

        // post-composite clear: allows drawing bw presents
        if (CUR->scene.clear) {
            SDL_SetRenderTarget(G.window.ren, CUR->tex);
            Pico_Color c = CUR->effect.color;
            SDL_SetRenderDrawColor(G.window.ren, c.r, c.g, c.b, c.a);
            SDL_RenderClear(G.window.ren);
        }
        cur = CUR->hier.nxt;
    }
    G.layer = old;
}

///////////////////////////////////////////////////////////////////////////////

static Pico_Layer* _pico_layer_pixmap (
    int mode,
    const char* key,
    Pico_Abs_Dim dim,
    const Pico_Color* pixels
) {
    assert(key!=NULL && "layer key required");
    assert(pixels!=NULL && "pixels required");
    _alloc_pixmap_t ctx = { dim, pixels };
    Pico_Layer* ret = (Pico_Layer*) realm_put (
        G.realm, mode, strlen(key)+1, key,
        _free_layer, _alloc_layer_pixmap, &ctx
    );
    assert(ret != NULL);
    return ret;
}

static Pico_Layer* _pico_layer_image (
    int mode, const char* key, const char* path
) {
    assert(path!=NULL && "image path required");
    const char* str = (key != NULL) ? key : path;
    Pico_Layer* ret = (Pico_Layer*) realm_put (
        G.realm, mode, strlen(str)+1, str,
        _free_layer, _alloc_layer_image, (void*)path
    );
    assert(ret != NULL);
    return ret;
}

static Pico_Layer* _pico_layer_text (
    int mode, const char* key, int height, const char* text
) {
    assert(text!=NULL && text[0]!='\0' && "text required");

    const char* str;
    char* str_buf = NULL;
    if (key == NULL) {
        const char* font = G.layer->pencil.font;
        Pico_Color clr = G.layer->pencil.color;
        const char* font_str = font ? font : "null";
        int buflen = strlen("/text/") + strlen(font_str) + 1
            + 10 + 1 + 3+1+3+1+3 + 1 + strlen(text) + 1;
        str_buf = alloca(buflen);
        snprintf(str_buf, buflen, "/text/%s/%d/%d.%d.%d/%s",
                 font_str, height, clr.r, clr.g, clr.b, text);
        str = str_buf;
    } else {
        str = key;
    }

    _alloc_text_t ctx = { height, text };
    Pico_Layer* ret = (Pico_Layer*) realm_put (
        G.realm, mode, strlen(str)+1, str,
        _free_layer, _alloc_layer_text, &ctx
    );
    assert(ret != NULL);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////

static void _show_tile (Pico_Layer_Scene* view, SDL_Rect dst) {
    if (view->tile.w<=0 || view->tile.h<=0) return;

    Pico_Color x_clr   = pico_get_pencil_color();
    PICO_STYLE x_style = pico_get_pencil_style();

    pico_set_pencil_color((Pico_Color){0xFF, 0xFF, 0xFF, 0xAA});
    pico_set_pencil_style(PICO_STYLE_STROKE);

    // grid
    int dx = dst.w * view->tile.w / view->dim.w;
    int dy = dst.h * view->tile.h / view->dim.h;
    if (dx > 0) {
        for (int i=dx; i<dst.w; i+=dx) {
            pico_output_draw_line (
                &(Pico_Rel_Pos){ '!', {dst.x+i, dst.y}, PICO_ANCHOR_NW },
                &(Pico_Rel_Pos){ '!', {dst.x+i, dst.y+dst.h}, PICO_ANCHOR_NW }
            );
        }
    }
    if (dy > 0) {
        for (int j=dy; j<dst.h; j+=dy) {
            pico_output_draw_line (
                &(Pico_Rel_Pos){ '!', {dst.x, dst.y+j}, PICO_ANCHOR_NW },
                &(Pico_Rel_Pos){ '!', {dst.x+dst.w, dst.y+j}, PICO_ANCHOR_NW }
            );
        }
    }

    // surrounding rect
    pico_output_draw_rect (
        &(Pico_Rel_Rect){ '!', {dst.x, dst.y, dst.w, dst.h}, PICO_ANCHOR_NW }
    );

    pico_set_pencil_color(x_clr);
    pico_set_pencil_style(x_style);
}

static void _show_grid (Pico_Layer* layer, Pico_Abs_Rect src, SDL_Rect dst) {
    if (!layer->effect.grid) return;

    Pico_Color x_clr = pico_get_pencil_color();
    pico_set_pencil_color((Pico_Color){0x77, 0x77, 0x77, 0x77});

    // grid lines
    {
        if ((dst.w%layer->scene.dim.w == 0) && (layer->scene.dim.w < dst.w)) {
            for (int i=0; i<dst.w; i+=(dst.w/layer->scene.dim.w)) {
                if (i == 0) continue;
                pico_output_draw_line (
                    &(Pico_Rel_Pos){ '!', {dst.x+i, dst.y}, PICO_ANCHOR_NW },
                    &(Pico_Rel_Pos){ '!', {dst.x+i, dst.y+dst.h}, PICO_ANCHOR_NW }
                );
            }
        }
        if ((dst.h%layer->scene.dim.h == 0) && (layer->scene.dim.h < dst.h)) {
            for (int j=0; j<dst.h; j+=(dst.h/layer->scene.dim.h)) {
                if (j == 0) continue;
                pico_output_draw_line (
                    &(Pico_Rel_Pos){ '!', {dst.x, dst.y+j}, PICO_ANCHOR_NW },
                    &(Pico_Rel_Pos){ '!', {dst.x+dst.w, dst.y+j}, PICO_ANCHOR_NW }
                );
            }
        }
    }

    // metric labels
    {
        pico_set_pencil_color((Pico_Color){0x77, 0x77, 0x77, 0xFF});
        int H = 10;

        for (int x=0; x<dst.w; x+=50) {
            if (x == 0) continue;
            int v = src.x + (x * src.w / dst.w);
            char lbl[8];
            snprintf(lbl, sizeof(lbl), "%d", v);
            Pico_Abs_Dim dim = pico_get_text (
                lbl,
                &(Pico_Rel_Dim){ '!', {0, H} }
            );
            pico_output_draw_text (
                lbl,
                &(Pico_Rel_Rect){ '!', {dst.x+x-dim.w/2, dst.y+10-dim.h/2, 0, dim.h}, PICO_ANCHOR_NW }
            );
        }

        for (int y=0; y<dst.h; y+=50) {
            if (y == 0) continue;
            int v = src.y + (y * src.h / dst.h);
            char lbl[8];
            snprintf(lbl, sizeof(lbl), "%d", v);
            Pico_Abs_Dim dim = pico_get_text(
                lbl,
                &(Pico_Rel_Dim){ '!', {0, H} });
            pico_output_draw_text (
                lbl,
                &(Pico_Rel_Rect){ '!', {dst.x+10-dim.w/2, dst.y+y-dim.h/2, 0, dim.h}, PICO_ANCHOR_NW }
            );
        }
    }

    pico_set_pencil_color(x_clr);
}

static void _pico_output_draw_layer (
    Pico_Layer* layer, Pico_Rel_Rect* rect
) {
    // blit layer onto current render target
    if (rect == NULL) {
        rect = &layer->scene.dst;
    }
    Pico_Abs_Dim* dp = NULL;
    if (rect->w == 0 || rect->h == 0) {
        dp = &layer->scene.dim;
    }
    SDL_FRect rf = _sdl_rect(rect, NULL, dp);
    SDL_Rect dst = _rnd_rect(&rf);

    Pico_Abs_Dim* sup = (layer->type == PICO_LAYER_SUB) ? &((Pico_Layer_Sub*)layer)->sup : &layer->scene.dim;
    Pico_Abs_Rect src = pico_cv_rect_rel_abs (
        &layer->scene.src,
        &(Pico_Abs_Rect){0, 0, sup->w, sup->h}
    );

    // clip dst to current layer (parent) bounds, propagate to src
    {
        void aux (SDL_Rect* a, SDL_Rect* b, int max_w, int max_h) {
            assert(a->w>0 && a->h>0);
            float sw = b->w / (float)a->w;
            float sh = b->h / (float)a->h;
            if (a->x < 0) {
                int d = -a->x;
                b->x += (d * sw);
                b->w -= (d * sw);
                a->w -= d;
                a->x = 0;
            }
            if (a->y < 0) {
                int d = -a->y;
                b->y += (d * sh);
                b->h -= (d * sh);
                a->h -= d;
                a->y = 0;
            }
            if (a->x+a->w > max_w) {
                int d = (a->x + a->w) - max_w;
                b->w -= (d * sw);
                a->w -= d;
            }
            if (a->y+a->h > max_h) {
                int d = (a->y + a->h) - max_h;
                b->h -= (d * sh);
                a->h -= d;
            }
        }
        int max_w, max_h;
        SDL_QueryTexture(SDL_GetRenderTarget(G.window.ren), NULL, NULL, &max_w, &max_h);
        //if (dst.x<0 || dst.y<0 || dst.x+dst.w>max_w || dst.y+dst.h>max_h)
        {
            aux(&dst, &src, max_w, max_h);
            aux(&src, &dst, sup->w, sup->h);
        }
    }

    SDL_SetTextureAlphaMod(layer->tex, G.layer->pencil.color.a*layer->effect.alpha/255);
    SDL_Point center = {
        dst.w * layer->effect.rotate.anchor.x,
        dst.h * layer->effect.rotate.anchor.y
    };
    SDL_RenderCopyEx(G.window.ren, layer->tex, &src, &dst,
                     layer->effect.rotate.angle, &center,
                     layer->effect.flip);

    if (layer->effect.grid) {
        _show_tile(&layer->scene, dst);
        if (G.layer == &G.window.layer) {
            _show_grid(layer, src, dst);
        }
    }

    _pico_output_present(0);
}

#undef PICO_LAYERS_C
#endif
