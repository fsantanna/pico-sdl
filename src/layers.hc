#ifndef PICO_LAYERS_HC
#define PICO_LAYERS_HC

typedef enum {
    PICO_LAYER_ROOT,
    PICO_LAYER_PLAIN,
    PICO_LAYER_VIDEO,
    PICO_LAYER_SUB,
} PICO_LAYER;

typedef struct Pico_Layer {
    PICO_LAYER            type;
    char*                 name;     // NULL for main layer
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
static Pico_Layer* _pico_layer_null (const char* name);

static Pico_Layer* _pico_layer_pixmap (
    int mode, const char* key, Pico_Abs_Dim dim,
    const Pico_Color* pixels
);
static Pico_Layer* _pico_layer_image (
    int mode, const char* key, const char* path
);
static Pico_Layer* _pico_layer_text (
    Pico_Layer* L, int mode, const char* key, int height, const char* text
);
static void _pico_output_draw_layer (
    Pico_Layer* dst, Pico_Layer* src, Pico_Rel_Rect* rect
);

#endif // PICO_LAYERS_HC

#ifdef PICO_LAYERS_C

static Pico_Layer* _pico_layer_name (const char* name) {
    assert(name != NULL);
    Pico_Layer* L = (Pico_Layer*) realm_get(G.realm, strlen(name)+1, name);
    pico_assert(L!=NULL && "layer does not exist");
    return L;
}

static Pico_Layer* _pico_layer_null (const char* name) {
    if (name == NULL) {
        return S.layer;
    } else {
        return _pico_layer_name(name);
    }
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

static void _pico_output_draw_layer (Pico_Layer*, Pico_Layer*, Pico_Rel_Rect*);

static void _layer_traverse (Pico_Layer* UP) {
    const char* cur = UP->hier.dn.fst;
    while (cur != NULL) {
        Pico_Layer* CUR = (Pico_Layer*) realm_get(G.realm, strlen(cur)+1, cur);
        assert(CUR != NULL);

        SDL_SetRenderTarget(G.ren, CUR->tex);
        _layer_traverse(CUR);

        SDL_SetRenderTarget(G.ren, UP->tex);
        _pico_output_draw_layer(UP, CUR, NULL);

        // post-composite clear: allows drawing bw presents
        if (!CUR->scene.keep) {
            SDL_SetRenderTarget(G.ren, CUR->tex);
            Pico_Color c = CUR->effect.color;
            SDL_SetRenderDrawColor(G.ren, c.r, c.g, c.b, c.a);
            SDL_RenderClear(G.ren);
        }
        cur = CUR->hier.nxt;
    }
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
    Pico_Layer* L, int mode, const char* key, int height, const char* text
) {
    assert(L != NULL);
    assert(text!=NULL && text[0]!='\0' && "text required");

    const char* str;
    char* str_buf = NULL;
    if (key == NULL) {
        const char* font = L->pencil.font;
        Pico_Color clr = L->pencil.color;
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

    _alloc_text_t ctx = { L, height, text };
    Pico_Layer* ret = (Pico_Layer*) realm_put (
        G.realm, mode, strlen(str)+1, str,
        _free_layer, _alloc_layer_text, &ctx
    );
    assert(ret != NULL);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////

static void _pico_output_draw_layer (
    Pico_Layer* dst, Pico_Layer* src, Pico_Rel_Rect* rect
) {
    assert(dst != NULL);
    assert(src != NULL);
    // blit src onto current render target (dst is the destination context)
    if (rect == NULL) {
        rect = &src->scene.dst;
    }
    Pico_Abs_Dim* dp = NULL;
    if (rect->w == 0 || rect->h == 0) {
        dp = &src->scene.dim;
    }
    SDL_FRect rf = _sdl_rect(dst, rect, NULL, dp);
    SDL_Rect xdst = _abs_rect(&rf);

    Pico_Abs_Dim* sup = (src->type == PICO_LAYER_SUB) ? &((Pico_Layer_Sub*)src)->sup : &src->scene.dim;
    Pico_Abs_Rect xsrc = pico_cv_rect_rel_abs (
        src->name, &src->scene.src,
        &(Pico_Abs_Rect){0, 0, sup->w, sup->h}
    );

    SDL_SetTextureAlphaMod(src->tex, dst->pencil.color.a*src->effect.alpha/255);
    SDL_Point center = {
        xdst.w * src->effect.rotate.anchor.x,
        xdst.h * src->effect.rotate.anchor.y
    };
    SDL_RenderCopyEx(G.ren, src->tex, &xsrc, &xdst,
                     src->effect.rotate.angle, &center,
                     src->effect.flip);

    if (src->effect.grid) {
        _show_tile(src, &src->scene, xdst);
    }

    _pico_output_present(0);
}

#undef PICO_LAYERS_C
#endif
