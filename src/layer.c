#include <assert.h>
#include <string.h>
#include <stdio.h>

#include <SDL2/SDL.h>

#include "_pico.h"


Pico_Layer* _pico_layer_name (const char* name) {
    assert(name != NULL);
    Pico_Layer* L = (Pico_Layer*) realm_get(G.realm, strlen(name)+1, name);
    assert(L!=NULL && "layer does not exist");
    return L;
}

// makes L the current render target: point the renderer at L->tex and
// set the clip from L->scene.clip (the canonical target+clip dance)
void _pico_layer_target (Pico_Layer* L) {
    SDL_SetRenderTarget(G.window.ren, L->tex);
    Pico_Abs_Rect r = _pico_abs_rect(L->scene.clip, NULL, NULL);
    SDL_RenderSetClipRect(G.window.ren, &r);
}

void _pico_layer_attach (const char* up, const char* dn) {
    Pico_Layer* UP = _pico_layer_name(up);
    Pico_Layer* DN = _pico_layer_name(dn);
    int access;
    SDL_QueryTexture(UP->tex, NULL, &access, NULL, NULL);
    assert(access==SDL_TEXTUREACCESS_TARGET && "layer cannot host children");
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

void _pico_layer_traverse (Pico_Layer* UP, _pico_layer_traverse_cb_t pre, _pico_layer_traverse_cb_t pos) {
    const char* cur = UP->hier.dn.fst;
    while (cur != NULL) {
        Pico_Layer* CUR = (Pico_Layer*) realm_get(G.realm, strlen(cur)+1, cur);
        assert(CUR != NULL);
        if (pre != NULL) {
            pre(UP, CUR);
        }
        _pico_layer_traverse(CUR, pre, pos);
        if (pos != NULL) {
            pos(UP, CUR);
        }
        cur = CUR->hier.nxt;
    }
}

// target CUR so the recursion draws CUR's children onto it
static void _draw_all_pre (Pico_Layer* UP, Pico_Layer* CUR) {
    SDL_SetRenderTarget(G.window.ren, CUR->tex);
}

// composite CUR onto UP (G.layer=UP so _pico_layer_output reads UP's pencil)
static void _draw_all_pos (Pico_Layer* UP, Pico_Layer* CUR) {
    G.layer = UP;
    _pico_layer_target(UP);
    _pico_layer_output(CUR, NULL);
}

void _pico_layer_draw_all (Pico_Layer* UP) {
    Pico_Layer* old = G.layer;
    _pico_layer_traverse(UP, _draw_all_pre, _draw_all_pos);
    G.layer = old;
}

///////////////////////////////////////////////////////////////////////////////

Pico_Layer* _pico_layer_pixmap (
    int mode,
    const char* key,
    Pico_Abs_Dim dim,
    const Pico_Color* pixels
) {
    assert(key!=NULL && "layer key required");
    assert(pixels!=NULL && "pixels required");
    _pico_mem_alloc_pixmap_t ctx = { dim, pixels };
    Pico_Layer* ret = (Pico_Layer*) realm_put (
        G.realm, mode, strlen(key)+1, (const void**)&key, 0, NULL,
        _pico_mem_free_layer, _pico_mem_alloc_layer_pixmap, &ctx
    );
    pico_assert_key(ret, key);
    return ret;
}

Pico_Layer* _pico_layer_image (
    int mode, const char* key, const char* path
) {
    assert(path!=NULL && "image path required");
    const char* str = (key != NULL) ? key : path;
    Pico_Layer* ret = (Pico_Layer*) realm_put (
        G.realm, mode, strlen(str)+1, (const void**)&str, 0, NULL,
        _pico_mem_free_layer, _pico_mem_alloc_layer_image, (void*)path
    );
    pico_assert_key(ret, str);
    return ret;
}

Pico_Layer* _pico_layer_text (
    int mode, const char* key, int height, const char* text
) {
    assert(text!=NULL && text[0]!='\0' && "text required");

    // content fingerprint: everything that forces a re-raster;
    // doubles as the auto-key when key is NULL
    const char* font = G.layer->pencil.font;
    Pico_Color clr = G.layer->pencil.color;
    const char* font_str = font ? font : "null";
    int buflen = strlen("/text/") + strlen(font_str) + 1
        + 10 + 1 + 3+1+3+1+3 + 1 + strlen(text) + 1;
    char* fp = alloca(buflen);
    snprintf(fp, buflen, "/text/%s/%d/%d.%d.%d/%s",
             font_str, height, clr.r, clr.g, clr.b, text);
    const char* str = (key != NULL) ? key : fp;

    _pico_mem_alloc_text_t ctx = { height, text };
    Pico_Layer* ret = (Pico_Layer*) realm_put (
        G.realm, mode, strlen(str)+1, (const void**)&str,
        strlen(fp)+1, fp,
        _pico_mem_free_layer, _pico_mem_alloc_layer_text, &ctx
    );
    pico_assert_key(ret, str);
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
                (Pico_Rel_Pos){ '!', {dst.x+i, dst.y},       PICO_ANCHOR_NW },
                (Pico_Rel_Pos){ '!', {dst.x+i, dst.y+dst.h}, PICO_ANCHOR_NW }
            );
        }
    }
    if (dy > 0) {
        for (int j=dy; j<dst.h; j+=dy) {
            pico_output_draw_line (
                (Pico_Rel_Pos){ '!', {dst.x, dst.y+j},       PICO_ANCHOR_NW },
                (Pico_Rel_Pos){ '!', {dst.x+dst.w, dst.y+j}, PICO_ANCHOR_NW }
            );
        }
    }

    // surrounding rect
    pico_output_draw_rect (
        (Pico_Rel_Rect){ '!', {dst.x, dst.y, dst.w, dst.h}, PICO_ANCHOR_NW }
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
                    (Pico_Rel_Pos) {
                        '!', {dst.x+i, dst.y}, PICO_ANCHOR_NW
                    },
                    (Pico_Rel_Pos) {
                        '!', {dst.x+i, dst.y+dst.h}, PICO_ANCHOR_NW
                    }
                );
            }
        }
        if ((dst.h%layer->scene.dim.h == 0) && (layer->scene.dim.h < dst.h)) {
            for (int j=0; j<dst.h; j+=(dst.h/layer->scene.dim.h)) {
                if (j == 0) continue;
                pico_output_draw_line (
                    (Pico_Rel_Pos) {
                        '!', {dst.x, dst.y+j}, PICO_ANCHOR_NW
                    },
                    (Pico_Rel_Pos) {
                        '!', {dst.x+dst.w, dst.y+j}, PICO_ANCHOR_NW
                    }
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
            pico_output_draw_text_fix (
                lbl,
                (Pico_Rel_Rect) {
                    '!',
                    {dst.x+x, dst.y+10, 0, H},
                    PICO_ANCHOR_C
                }
            );
        }

        for (int y=0; y<dst.h; y+=50) {
            if (y == 0) continue;
            int v = src.y + (y * src.h / dst.h);
            char lbl[8];
            snprintf(lbl, sizeof(lbl), "%d", v);
            pico_output_draw_text_fix (
                lbl,
                (Pico_Rel_Rect) {
                    '!',
                    {dst.x+10, dst.y+y, 0, H},
                    PICO_ANCHOR_C
                }
            );
        }
    }

    pico_set_pencil_color(x_clr);
}

void _pico_layer_output (
    Pico_Layer* layer, const Pico_Rel_Rect* rect
) {
    // blit layer onto current render target
    if (rect == NULL) {
        rect = &layer->scene.dst;
    }
    Pico_Abs_Dim* dp = NULL;
    if (rect->w == 0 || rect->h == 0) {
        dp = &layer->scene.dim;
    }
    SDL_Rect dst = _pico_abs_rect(*rect, NULL, dp);

    Pico_Abs_Dim* sup = (layer->type == PICO_LAYER_SUB) ?
                            &((Pico_Layer_Sub*)layer)->sup : &layer->scene.dim;
    Pico_Abs_Rect src = _pico_abs_rect (
        layer->scene.src, &(Pico_Abs_Rect){0, 0, sup->w, sup->h}, NULL
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

        // fully off-screen: nothing to do
        if (
            dst.x+dst.w <= 0 || dst.y+dst.h <= 0 ||
            dst.x >= max_w || dst.y >= max_h
        ) {
            return;
        }

        // degenerate (zero/negative) dst or src: nothing to draw
        if (dst.w<=0 || dst.h<=0 || src.w<=0 || src.h<=0) {
            return;
        }

        aux(&dst, &src, max_w, max_h);
        if (src.w<=0 || src.h<=0) {
            return; // dst-clip may have shrunk src to nothing
        }
        aux(&src, &dst, sup->w, sup->h);
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
///////////////////////////////////////////////////////////////////////////////
// LAYER
///////////////////////////////////////////////////////////////////////////////

int pico_unique (void) {
    return realm_unique(G.realm);
}

// Auto-generates a unique key "/unique/N" into buf when key is NULL.
// The '/' prefix is reserved (users cannot create such keys), so
// generated keys never collide with user keys.

static const char* _key_unique (const char* key, char* buf, int n) {
    if (key == NULL) {
        snprintf(buf, n, "/unique/%d", pico_unique());
        key = buf;
    }
    return key;
}

const char* pico_layer_pixmap (
    const char* up,
    const char* key,
    Pico_Abs_Dim dim,
    const Pico_Color* pixels
) {
    _pico_guard();
    return pico_layer_pixmap_mode('!', up, key, dim, pixels);
}

const char* pico_layer_pixmap_mode (
    int mode,
    const char* up,
    const char* key,
    Pico_Abs_Dim dim,
    const Pico_Color* pixels
) {
    _pico_guard();
    char buf[24];
    key = _key_unique(key, buf, sizeof(buf));
    Pico_Layer* ret = _pico_layer_pixmap(mode, key, dim, pixels);
    if (up != NULL) {
        _pico_layer_attach(up, ret->name);
    }
    return ret->name;
}

const char* pico_layer_empty (
    const char* up, const char* key, int clear,
    Pico_Rel_Dim dim, Pico_Rel_Dim* tile
) {
    _pico_guard();
    return pico_layer_empty_mode('!', up, key, clear, dim, tile);
}

const char* pico_layer_empty_mode (
    int mode, const char* up, const char* key, int clear,
    Pico_Rel_Dim dim, Pico_Rel_Dim* tile
) {
    _pico_guard();
    char buf[24];
    key = _key_unique(key, buf, sizeof(buf));
    _pico_mem_alloc_empty_t ctx = { up, clear, dim, tile };
    void* ret = realm_put (
        G.realm, mode, strlen(key)+1, (const void**)&key, 0, NULL,
        _pico_mem_free_layer, _pico_mem_alloc_layer_empty, &ctx
    );
    pico_assert_key(ret, key);
    if (up != NULL) {
        _pico_layer_attach(up, key);
    }
    return key;
}

const char* pico_layer_image (const char* up, const char* key, const char* path) {
    _pico_guard();
    return pico_layer_image_mode('!', up, key, path);
}

const char* pico_layer_image_mode (
    int mode, const char* up, const char* key, const char* path
) {
    _pico_guard();
    Pico_Layer* ret = _pico_layer_image(mode, key, path);
    if (up != NULL) {
        _pico_layer_attach(up, ret->name);
    }
    return ret->name;
}

const char* pico_layer_screenshot (const char* up, const char* key,
    const char* src, const Pico_Rel_Rect* rect)
{
    _pico_guard();
    return pico_layer_screenshot_mode('!', up, key, src, rect);
}

const char* pico_layer_screenshot_mode (int mode, const char* up, const char* key,
    const char* src, const Pico_Rel_Rect* rect)
{
    _pico_guard();
    char buf[24];
    key = _key_unique(key, buf, sizeof(buf));
    _pico_mem_alloc_shot_t ctx = { src, rect };
    void* ret = realm_put (
        G.realm, mode, strlen(key)+1, (const void**)&key, 0, NULL,
        _pico_mem_free_layer, _pico_mem_alloc_layer_shot, &ctx
    );
    pico_assert_key(ret, key);
    if (up != NULL) {
        _pico_layer_attach(up, key);
    }
    return key;
}

const char* pico_layer_sub (const char* up, const char* key,
    const char* parent, const Pico_Rel_Rect* crop)
{
    _pico_guard();
    return pico_layer_sub_mode('!', up, key, parent, crop);
}

const char* pico_layer_sub_mode (int mode, const char* up, const char* key,
    const char* parent, const Pico_Rel_Rect* crop)
{
    _pico_guard();
    assert(parent!=NULL   && "parent key required");
    assert(crop!=NULL     && "crop rect required");
    char buf[24];
    key = _key_unique(key, buf, sizeof(buf));

    Pico_Layer* par = (Pico_Layer*)realm_get(
        G.realm, strlen(parent)+1, parent);
    assert(par!=NULL && "parent layer does not exist");
    assert(par->type!=PICO_LAYER_SUB
        && "cannot create sub-layer of sub-layer");

    _pico_mem_alloc_sub_t ctx = { par, *crop };
    void* ret = realm_put (
        G.realm, mode, strlen(key)+1, (const void**)&key, 0, NULL,
        _pico_mem_free_layer, _pico_mem_alloc_layer_sub, &ctx
    );
    pico_assert_key(ret, key);
    if (up != NULL) {
        _pico_layer_attach(up, key);
    }
    return key;
}

const char* pico_layer_text (
    const char* up, const char* key, Pico_Rel_Dim dim, const char* text
) {
    _pico_guard();
    return pico_layer_text_mode('!', up, key, dim, text);
}

const char* pico_layer_text_mode (
    int mode, const char* up, const char* key, Pico_Rel_Dim dim, const char* text
) {
    _pico_guard();
    assert(!(dim.w==0 && dim.h==0) && "invalid dim");
    assert(dim.w==0 && "invalid dim.w : not implemented");
    char buf[24];
    key = _key_unique(key, buf, sizeof(buf));

    // resolve dim.h against up's scene.dim (or current if detached)
    Pico_Abs_Rect base, *xbase=NULL;
    if (up != NULL) {
        Pico_Layer* par = _pico_layer_name(up);
        base = (Pico_Abs_Rect) {0, 0, par->scene.dim.w, par->scene.dim.h};
        xbase = &base;
    }
    Pico_Abs_Dim abs = _pico_abs_dim(&dim, xbase, NULL);

    Pico_Layer* ret = _pico_layer_text(mode, key, abs.h, text);
    if (up != NULL) {
        _pico_layer_attach(up, ret->name);
    }
    return ret->name;
}
