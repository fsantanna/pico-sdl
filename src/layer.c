#include <assert.h>
#include <string.h>
#include <stdio.h>

#include <alloca.h>
#include <SDL2/SDL.h>

#include "_pico.h"


// makes L the current layer and render target: point the renderer at
// L->tex and set the clip from L->scene.clip (the canonical target+clip
// dance); returns the previous layer for save/restore
Pico_Layer* _pico_set_layer (Pico_Layer* L) {
    Pico_Layer* old = G.layer;
    G.layer = L;
    SDL_SetRenderTarget(G.window.ren, L->tex);
    Pico_Abs_Rect r = _pico_abs_rect(L->scene.clip, NULL, NULL);
    SDL_RenderSetClipRect(G.window.ren, &r);
    return old;
}

// appends DN as the last (front-most) child of UP
void _pico_layer_attach (Pico_Layer* UP, Pico_Layer* DN) {
    assert(DN->hier.up==NULL && "layer is already attached");
    int access;
    SDL_QueryTexture(UP->tex, NULL, &access, NULL, NULL);
    assert(access==SDL_TEXTUREACCESS_TARGET && "layer cannot host children");
    DN->hier.up = UP;
    DN->hier.nxt = NULL;
    if (UP->hier.dn.fst == NULL) {
        UP->hier.dn.fst = DN;
        UP->hier.dn.lst = DN;
    } else {
        UP->hier.dn.lst->hier.nxt = DN;
        UP->hier.dn.lst = DN;
    }
}

// splices L out of its parent's child chain (no-op if detached)
void _pico_layer_detach (Pico_Layer* L) {
    Pico_Layer* UP = L->hier.up;
    if (UP == NULL) {
        return;
    }
    Pico_Layer* prv = NULL;
    Pico_Layer* cur = UP->hier.dn.fst;
    while (cur!=NULL && cur!=L) {
        prv = cur;
        cur = cur->hier.nxt;
    }
    assert(cur==L && "bug found");
    if (prv == NULL) {
        UP->hier.dn.fst = L->hier.nxt;
    } else {
        prv->hier.nxt = L->hier.nxt;
    }
    if (UP->hier.dn.lst == L) {
        UP->hier.dn.lst = prv;
    }
    L->hier.up  = NULL;
    L->hier.nxt = NULL;
}

void _pico_layer_traverse (Pico_Layer* UP, _pico_layer_traverse_cb_t pre, _pico_layer_traverse_cb_t pos) {
    Pico_Layer* CUR = UP->hier.dn.fst;
    while (CUR != NULL) {
        if (pre != NULL) {
            pre(UP, CUR);
        }
        _pico_layer_traverse(CUR, pre, pos);
        if (pos != NULL) {
            pos(UP, CUR);
        }
        CUR = CUR->hier.nxt;
    }
}

// target CUR so the recursion draws CUR's children onto it
static void _draw_all_pre (Pico_Layer* UP, Pico_Layer* CUR) {
    SDL_SetRenderTarget(G.window.ren, CUR->tex);
}

// composite CUR onto UP (G.layer=UP so _pico_layer_output reads UP's pencil)
static void _draw_all_pos (Pico_Layer* UP, Pico_Layer* CUR) {
    _pico_set_layer(UP);
    _pico_layer_output(CUR, NULL);
}

void _pico_layer_draw_all (Pico_Layer* UP) {
    Pico_Layer* old = G.layer;
    _pico_layer_traverse(UP, _draw_all_pre, _draw_all_pos);
    G.layer = old;
}

///////////////////////////////////////////////////////////////////////////////
// cache-owned layers (easy paths): loaded once, kept until pico_close

Pico_Layer* _pico_layer_image_path (const char* path) {
    assert(path!=NULL && "image path required");
    int n = strlen("/image/") + strlen(path) + 1;
    char* key = alloca(n);
    snprintf(key, n, "/image/%s", path);
    Pico_Layer* ret = (Pico_Layer*) _pico_cache_get(key);
    if (ret == NULL) {
        ret = _pico_mem_layer_image(1, path);
        _pico_cache_put(key, PICO_CACHE_LAYER, ret);
    }
    return ret;
}

Pico_Layer* _pico_layer_text_fp (int height, const char* text) {
    assert(text!=NULL && text[0]!='\0' && "text required");

    // content fingerprint: everything that forces a re-raster
    Pico_Font* font = G.layer->pencil.font;
    Pico_Color clr = G.layer->pencil.color;
    int id = (font == NULL) ? 0 : font->id;
    int n = strlen("/text/") + 10 + 1 + 10 + 1 + 3+1+3+1+3 + 1
        + strlen(text) + 1;
    char* key = alloca(n);
    snprintf(key, n, "/text/%d/%d/%d.%d.%d/%s",
             id, height, clr.r, clr.g, clr.b, text);

    Pico_Layer* ret = (Pico_Layer*) _pico_cache_get(key);
    if (ret == NULL) {
        ret = _pico_mem_layer_text(1, height, text);
        _pico_cache_put(key, PICO_CACHE_LAYER, ret);
    }
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
            pico_output_draw_text (
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
            pico_output_draw_text (
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

Pico_Layer* pico_layer_world (void) {
    _pico_guard();
    return &G.world;
}

Pico_Layer* pico_layer_window (void) {
    _pico_guard();
    return &G.window.layer;
}

Pico_Layer* pico_layer_pixmap (
    Pico_Layer* up, Pico_Abs_Dim dim, const Pico_Color* pixels
) {
    _pico_guard();
    assert(pixels!=NULL && "pixels required");
    Pico_Layer* ret = _pico_mem_layer_pixmap(0, dim, pixels);
    if (up != NULL) {
        _pico_layer_attach(up, ret);
    }
    return ret;
}

Pico_Layer* pico_layer_empty (
    Pico_Layer* up, int clear, Pico_Rel_Dim dim, Pico_Rel_Dim* tile
) {
    _pico_guard();
    Pico_Layer* ret = _pico_mem_layer_empty(0, up, clear, dim, tile);
    if (up != NULL) {
        _pico_layer_attach(up, ret);
    }
    return ret;
}

Pico_Layer* pico_layer_image (Pico_Layer* up, const char* path) {
    _pico_guard();
    assert(path!=NULL && "image path required");
    Pico_Layer* ret = _pico_mem_layer_image(0, path);
    if (up != NULL) {
        _pico_layer_attach(up, ret);
    }
    return ret;
}

Pico_Layer* pico_layer_screenshot (
    Pico_Layer* up, Pico_Layer* src, const Pico_Rel_Rect* rect
) {
    _pico_guard();
    Pico_Layer* ret = _pico_mem_layer_shot(0, src, rect);
    if (up != NULL) {
        _pico_layer_attach(up, ret);
    }
    return ret;
}

Pico_Layer* pico_layer_sub (
    Pico_Layer* up, Pico_Layer* src, const Pico_Rel_Rect* crop
) {
    _pico_guard();
    assert(src!=NULL  && "source layer required");
    assert(crop!=NULL && "crop rect required");
    assert(src->type!=PICO_LAYER_SUB
        && "cannot create sub-layer of sub-layer");
    Pico_Layer* ret = _pico_mem_layer_sub(0, src, *crop);
    if (up != NULL) {
        _pico_layer_attach(up, ret);
    }
    return ret;
}

Pico_Layer* pico_layer_text (
    Pico_Layer* up, Pico_Rel_Dim dim, const char* text
) {
    _pico_guard();
    assert(text!=NULL && text[0]!='\0' && "text required");
    assert(!(dim.w==0 && dim.h==0) && "invalid dim");
    assert(dim.w==0 && "invalid dim.w : not implemented");

    // resolve dim.h against up's scene.dim (or current if detached)
    Pico_Abs_Rect base, *xbase=NULL;
    if (up != NULL) {
        base = (Pico_Abs_Rect) {0, 0, up->scene.dim.w, up->scene.dim.h};
        xbase = &base;
    }
    Pico_Abs_Dim abs = _pico_abs_dim(&dim, xbase, NULL);

    Pico_Layer* ret = _pico_mem_layer_text(0, abs.h, text);
    if (up != NULL) {
        _pico_layer_attach(up, ret);
    }
    return ret;
}

void pico_layer_free (Pico_Layer* layer) {
    _pico_guard();
    assert(layer!=NULL && "layer required");
    assert(layer!=&G.world && "cannot free the world layer");
    assert(layer!=&G.window.layer && "cannot free the window layer");
    assert(!layer->cached && "bug found");
    assert(layer!=G.layer && "cannot free the current layer");
    assert(layer->hier.dn.fst==NULL && "cannot free a layer with children");
    assert(layer->subs==0 && "cannot free a layer with live sub-layers");
    _pico_mem_layer_free(layer);
}

///////////////////////////////////////////////////////////////////////////////
// FONT / SOUND
///////////////////////////////////////////////////////////////////////////////

Pico_Font* pico_font_load (const char* path) {
    _pico_guard();
    return _pico_mem_font_new(0, path);
}

void pico_font_free (Pico_Font* font) {
    _pico_guard();
    assert(font!=NULL && "font required");
    assert(!font->cached && font!=&G.font && "bug found");
    _pico_mem_font_free(font);
}

Pico_Sound* pico_sound_load (const char* path) {
    _pico_guard();
    return _pico_mem_sound_new(0, path);
}

void pico_sound_free (Pico_Sound* sound) {
    _pico_guard();
    assert(sound!=NULL && "sound required");
    assert(!sound->cached && "bug found");
    _pico_mem_sound_free(sound);
}
