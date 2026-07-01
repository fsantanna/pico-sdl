#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include "_pico.h"


// tiny_ttf.h *defines* the data; declare as extern to avoid double-def
extern unsigned char pico_tiny_ttf[];
extern unsigned int  pico_tiny_ttf_len;

///////////////////////////////////////////////////////////////////////////////
// Free callbacks
///////////////////////////////////////////////////////////////////////////////

void _pico_mem_free_font (int n, const void* key, void* value) {
    TTF_CloseFont((TTF_Font*)value);
}

// Detach this layer from the parent-child graph before freeing it.
// Three borrowed-pointer caches reference data->name (which
// free(data->name) at the bottom of _pico_mem_free_layer will
// invalidate):
//   - parent's  hier.dn.fst / hier.dn.lst
//   - sibling's hier.nxt
//   - each child's hier.up
// Runs for realm '~' replace, realm_leave pop, and realm_close.
// Each block asserts the chain invariants it touches.
static void _detach_layer (Pico_Layer* data) {
    // splice self out of parent's child chain
    if (data->hier.up != NULL) {
        Pico_Layer* UP = (Pico_Layer*) realm_get(
            G.realm, strlen(data->hier.up)+1, data->hier.up
        );
        assert(UP!=NULL && "bug found");
        const char* prv = NULL;
        const char* cur = UP->hier.dn.fst;
        while (cur!=NULL && cur!=data->name) {
            Pico_Layer* CUR = (Pico_Layer*) realm_get(
                G.realm, strlen(cur)+1, cur
            );
            assert(CUR!=NULL && "bug found");
            prv = cur;
            cur = CUR->hier.nxt;
        }
        assert(cur==data->name && "bug found");
        if (prv == NULL) {
            UP->hier.dn.fst = data->hier.nxt;
        } else {
            Pico_Layer* PRV = (Pico_Layer*) realm_get(
                G.realm, strlen(prv)+1, prv
            );
            assert(PRV!=NULL && "bug found");
            PRV->hier.nxt = data->hier.nxt;
        }
        if (UP->hier.dn.lst == data->name) {
            UP->hier.dn.lst = prv;
        }
    }

    // detach own children — null each child's hier.up so a later
    // free does not splice against our (soon-to-be freed) name
    {
        const char* cur = data->hier.dn.fst;
        const char* lst = NULL;
        while (cur != NULL) {
            Pico_Layer* CUR = (Pico_Layer*) realm_get(
                G.realm, strlen(cur)+1, cur
            );
            assert(CUR!=NULL && "bug found");
            assert(CUR->hier.up==data->name && "bug found");
            lst = cur;
            cur = CUR->hier.nxt;
            CUR->hier.up = NULL;
        }
        assert(lst==data->hier.dn.lst && "bug found");
    }
}

void _pico_mem_free_layer (int n, const void* key, void* value) {
    Pico_Layer* data = (Pico_Layer*)value;
    // '~' replace: realm still has us (entry unlinked only on leave /
    // close). Refuse to replace a parent — the new layer would silently
    // orphan the children's subtree.
    if (realm_get(G.realm, n, key) == data) {
        assert(data->hier.dn.fst == NULL
            && "cannot '~' replace a layer with children");
    }
    _detach_layer(data);
    if (data->type == PICO_LAYER_VIDEO) {
        _pico_video_free_layer((Pico_Layer_Video*)data);
    }
    if (data->type != PICO_LAYER_SUB) {
        SDL_DestroyTexture(data->tex);
    }
    free(data);
}

// Detach-only callback for static layers (G.world, G.window.layer).
// Their struct, texture, and string-literal name are owned by pico
// itself, not by the realm — so we just disown children / splice
// from parent, and let pico_init(false) free the rest.
void _pico_mem_detach_layer (int n, const void* key, void* value) {
    _detach_layer((Pico_Layer*)value);
}

void _pico_mem_free_sound (int n, const void* key, void* value) {
    Mix_FreeChunk((Mix_Chunk*)value);
}

///////////////////////////////////////////////////////////////////////////////
// Alloc helpers
///////////////////////////////////////////////////////////////////////////////

Pico_Layer* _pico_mem_layer_new (
    int clear, int type, size_t size,
    const char* key, SDL_Texture* tex, Pico_Abs_Dim dim
) {
    Pico_Layer* data = calloc(1, size);
    assert(data != NULL);
    *data = (Pico_Layer) {
        .type = type,
        .name = (char*)key,
        .tex  = tex,
        .pencil = {
            .color={0xFF, 0xFF, 0xFF, 0xFF}, .font=NULL, .style=PICO_STYLE_FILL
        },
        .effect = {
            .alpha=0xFF, .color={0, 0, 0, 0xFF}, .flip=PICO_FLIP_NONE, .grid=0, .rotate={0, PICO_ANCHOR_C}
        },
        .scene = {
            .clear = clear,
            .dim   = dim,
            .tile  = {0, 0},
            .dst   = {'%', {.5,.5,1,1}, PICO_ANCHOR_C},
            .src   = {'%', {.5,.5,1,1}, PICO_ANCHOR_C},
            .clip  = {'%', {.5,.5,1,1}, PICO_ANCHOR_C},
        },
    };
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    return data;
}

///////////////////////////////////////////////////////////////////////////////
// Alloc callbacks
///////////////////////////////////////////////////////////////////////////////

void* _pico_mem_alloc_layer_pixmap (int n, const void* key, void* ctx) {
    _pico_mem_alloc_pixmap_t* c = (_pico_mem_alloc_pixmap_t*)ctx;
    SDL_Surface* sfc = SDL_CreateRGBSurfaceWithFormatFrom(
        (void*)c->pixels, c->dim.w, c->dim.h,
        32, 4 * c->dim.w, SDL_PIXELFORMAT_RGBA32
    );
    SDL_Texture* tex = SDL_CreateTextureFromSurface(G.window.ren, sfc);
    pico_assert(tex != NULL);
    SDL_FreeSurface(sfc);
    return _pico_mem_layer_new (
        0, PICO_LAYER_PLAIN, sizeof(Pico_Layer),
        (const char*)key, tex, c->dim
    );
}

void* _pico_mem_alloc_layer_shot (int n, const void* key, void* ctx) {
    _pico_mem_alloc_shot_t* c = (_pico_mem_alloc_shot_t*)ctx;

    // same capture as pico_output_screenshot, then upload to a texture
    // instead of writing a PNG
    SDL_Surface* sfc = _pico_shot(c->src, c->rect);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(G.window.ren, sfc);
    pico_assert(tex != NULL);
    Pico_Abs_Dim dim = { sfc->w, sfc->h };
    SDL_FreeSurface(sfc);

    return _pico_mem_layer_new (
        0, PICO_LAYER_PLAIN, sizeof(Pico_Layer),
        (const char*)key, tex, dim
    );
}

void* _pico_mem_alloc_layer_empty (int n, const void* key, void* ctx) {
    _pico_mem_alloc_empty_t* arg = (_pico_mem_alloc_empty_t*)ctx;

    // resolve rel dim against parent's scene.dim
    Pico_Abs_Dim dim;
    if (arg->up == NULL) {
        // falls back to G.layer->scene.dim
        dim = _pico_abs_dim(&arg->dim, NULL, NULL);
    } else {
        Pico_Layer* par = _pico_layer_name(arg->up);
        Pico_Abs_Rect base = (Pico_Abs_Rect) {
            0, 0, par->scene.dim.w, par->scene.dim.h
        };
        dim = _pico_abs_dim(&arg->dim, &base, NULL);
    }

    if (arg->tile != NULL) {
        dim.w *= arg->tile->w;
        dim.h *= arg->tile->h;
    }
    Pico_Layer* lay = _pico_mem_layer_new (
        arg->clear, PICO_LAYER_PLAIN, sizeof(Pico_Layer),
        (const char*)key, _pico_tex_create(dim), dim
    );
    if (arg->tile != NULL) {
        lay->scene.tile = *arg->tile;
    }
    return lay;
}

void* _pico_mem_alloc_layer_image (int n, const void* key, void* ctx) {
    const char* path = (const char*)ctx;
    SDL_Texture* tex = IMG_LoadTexture(G.window.ren, path);
    pico_assert(tex != NULL);
    Pico_Abs_Dim dim;
    SDL_QueryTexture(tex, NULL, NULL, &dim.w, &dim.h);
    return _pico_mem_layer_new (
        0, PICO_LAYER_PLAIN, sizeof(Pico_Layer),
        (const char*)key, tex, dim
    );
}

void* _pico_mem_alloc_layer_sub (int n, const void* key, void* ctx) {
    _pico_mem_alloc_sub_t* c = (_pico_mem_alloc_sub_t*)ctx;
    Pico_Abs_Rect base = {0, 0, c->par->scene.dim.w, c->par->scene.dim.h};
    Pico_Abs_Rect abs = _pico_abs_rect(c->crop, &base, NULL);
    Pico_Layer* data = _pico_mem_layer_new (
        0, PICO_LAYER_SUB, sizeof(Pico_Layer_Sub),
        (const char*)key, c->par->tex,
        (Pico_Abs_Dim){abs.w, abs.h}
    );
    data->scene.src = c->crop;
    ((Pico_Layer_Sub*)data)->sup = c->par->scene.dim;
    return data;
}

static SDL_Texture* _tex_text (int height, const char* text, Pico_Abs_Dim* dim) {
    SDL_Color c = { G.layer->pencil.color.r, G.layer->pencil.color.g, G.layer->pencil.color.b, 0xFF };

#if 1
    // resolve target pixel height to a point size: largest p with
    // TTF_FontHeight(p) <= height. probe once to estimate the
    // ptsize/pixel ratio, then correct +/-1 (TTF rounds internally).
    const char* font = G.layer->pencil.font;
    int ptsize;
    {
        int p0 = height > 0 ? height : 1;
        int fh0 = TTF_FontHeight(_pico_font_get(font, p0));
        if (fh0 <= 0) {
            ptsize = p0;
        } else {
            ptsize = (height * p0 + fh0 / 2) / fh0;
            if (ptsize < 1) ptsize = 1;
            while (ptsize > 1 && TTF_FontHeight(_pico_font_get(font, ptsize)) > height) ptsize--;
            while (TTF_FontHeight(_pico_font_get(font, ptsize + 1)) <= height) ptsize++;
        }
    }
    TTF_Font* ttf = _pico_font_get(font, ptsize);
#else
    TTF_Font* ttf = _pico_font_get(G.layer->pencil.font, height);
#endif

    int fh = TTF_FontHeight(ttf);
    SDL_Surface* sfc = TTF_RenderText_Blended(ttf, text, c);
    pico_assert(sfc != NULL);

    // pin the raster to the font's max height (TTF_FontHeight) so the
    // layer dim never changes with the string's ascenders/descenders.
    // a content-independent box height keeps a non-top anchor from
    // turning a per-string height change into a vertical snap on reveal.
    SDL_Surface* box = SDL_CreateRGBSurfaceWithFormat(0, sfc->w, fh, 32, SDL_PIXELFORMAT_RGBA32);
    pico_assert(box != NULL);
    SDL_SetSurfaceBlendMode(sfc, SDL_BLENDMODE_NONE);
    SDL_BlitSurface(sfc, NULL, box, &(SDL_Rect){ 0, 0, sfc->w, sfc->h });

    SDL_Texture* tex = SDL_CreateTextureFromSurface(G.window.ren, box);
    pico_assert(tex != NULL);
    *dim = (Pico_Abs_Dim){ sfc->w, fh };
    SDL_FreeSurface(sfc);
    SDL_FreeSurface(box);
    return tex;
}

void* _pico_mem_alloc_layer_text (int n, const void* key, void* ctx) {
    _pico_mem_alloc_text_t* c = (_pico_mem_alloc_text_t*)ctx;
    Pico_Abs_Dim dim;
    SDL_Texture* tex = _tex_text(c->height, c->text, &dim);
    return _pico_mem_layer_new (
        0, PICO_LAYER_PLAIN, sizeof(Pico_Layer),
        (const char*)key, tex, dim
    );
}

void* _pico_mem_alloc_font (int n, const void* key, void* ctx) {
    _pico_mem_alloc_font_t* c = (_pico_mem_alloc_font_t*)ctx;
    TTF_Font* ttf;
    if (c->path == NULL) {
        SDL_RWops* rw = SDL_RWFromConstMem(pico_tiny_ttf, pico_tiny_ttf_len);
        ttf = TTF_OpenFontRW(rw, 1, c->h);
    } else {
        ttf = TTF_OpenFont(c->path, c->h);
    }
    pico_assert(ttf != NULL);
    return ttf;
}

void* _pico_mem_alloc_sound (int n, const void* key, void* ctx) {
    const char* path = (const char*)ctx;
    return Mix_LoadWAV(path);
}
