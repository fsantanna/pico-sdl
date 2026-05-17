#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include "mem.h"
#include "state.h"
#include "aux.h"
#include "layers.h"
#include "video.h"

// tiny_ttf.h *defines* the data; declare as extern to avoid double-def
extern unsigned char pico_tiny_ttf[];
extern unsigned int  pico_tiny_ttf_len;

///////////////////////////////////////////////////////////////////////////////
// Free callbacks
///////////////////////////////////////////////////////////////////////////////

void _free_font (int n, const void* key, void* value) {
    TTF_CloseFont((TTF_Font*)value);
}

void _free_layer (int n, const void* key, void* value) {
    Pico_Layer* data = (Pico_Layer*)value;
    if (data->type == PICO_LAYER_VIDEO) {
        _free_layer_video((Pico_Layer_Video*)data);
    }
    if (data->type != PICO_LAYER_SUB) {
        SDL_DestroyTexture(data->tex);
    }
    free(data->name);
    free(data);
}

void _free_sound (int n, const void* key, void* value) {
    Mix_FreeChunk((Mix_Chunk*)value);
}

///////////////////////////////////////////////////////////////////////////////
// Alloc helpers
///////////////////////////////////////////////////////////////////////////////

Pico_Layer* _layer_new (
    int clear, int type, size_t size,
    const char* key, SDL_Texture* tex, Pico_Abs_Dim dim
) {
    Pico_Layer* data = calloc(1, size);
    assert(data != NULL);
    *data = (Pico_Layer) {
        .type = type,
        .name = strdup(key),
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
    assert(data->name != NULL);
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    return data;
}

///////////////////////////////////////////////////////////////////////////////
// Alloc callbacks
///////////////////////////////////////////////////////////////////////////////

void* _alloc_layer_pixmap (int n, const void* key, void* ctx) {
    _alloc_pixmap_t* c = (_alloc_pixmap_t*)ctx;
    SDL_Surface* sfc = SDL_CreateRGBSurfaceWithFormatFrom(
        (void*)c->pixels, c->dim.w, c->dim.h,
        32, 4 * c->dim.w, SDL_PIXELFORMAT_RGBA32
    );
    SDL_Texture* tex = SDL_CreateTextureFromSurface(G.window.ren, sfc);
    pico_assert(tex != NULL);
    SDL_FreeSurface(sfc);
    return _layer_new (
        0, PICO_LAYER_PLAIN, sizeof(Pico_Layer),
        (const char*)key, tex, c->dim
    );
}

void* _alloc_layer_empty (int n, const void* key, void* ctx) {
    _alloc_empty_t* arg = (_alloc_empty_t*)ctx;

    // resolve rel dim against parent's scene.dim
    Pico_Abs_Dim dim;
    if (arg->up == NULL) {
        // falls back to G.layer->scene.dim
        dim = _rnd_dim(_raw_dim(&arg->dim, NULL, NULL));
    } else {
        Pico_Layer* par = _pico_layer_name(arg->up);
        Pico_Abs_Rect base = (Pico_Abs_Rect) {
            0, 0, par->scene.dim.w, par->scene.dim.h
        };
        dim = _rnd_dim(_raw_dim(&arg->dim, &base, NULL));
    }

    if (arg->tile != NULL) {
        dim.w *= arg->tile->w;
        dim.h *= arg->tile->h;
    }
    Pico_Layer* lay = _layer_new (
        arg->clear, PICO_LAYER_PLAIN, sizeof(Pico_Layer),
        (const char*)key, _tex_create(dim), dim
    );
    if (arg->tile != NULL) {
        lay->scene.tile = *arg->tile;
    }
    return lay;
}

void* _alloc_layer_image (int n, const void* key, void* ctx) {
    const char* path = (const char*)ctx;
    SDL_Texture* tex = IMG_LoadTexture(G.window.ren, path);
    pico_assert(tex != NULL);
    Pico_Abs_Dim dim;
    SDL_QueryTexture(tex, NULL, NULL, &dim.w, &dim.h);
    return _layer_new (
        0, PICO_LAYER_PLAIN, sizeof(Pico_Layer),
        (const char*)key, tex, dim
    );
}

void* _alloc_layer_sub (int n, const void* key, void* ctx) {
    _alloc_sub_t* c = (_alloc_sub_t*)ctx;
    Pico_Abs_Rect base = {0, 0, c->par->scene.dim.w, c->par->scene.dim.h};
    Pico_Abs_Rect abs = _rnd_rect(_raw_rect(c->crop, &base, NULL));
    Pico_Layer* data = _layer_new (
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
    TTF_Font* ttf = _font_get(G.layer->pencil.font, height);
    SDL_Surface* sfc = TTF_RenderText_Solid(ttf, text, c);
    pico_assert(sfc != NULL);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(G.window.ren, sfc);
    pico_assert(tex != NULL);
    *dim = (Pico_Abs_Dim){ sfc->w, sfc->h };
    SDL_FreeSurface(sfc);
    return tex;
}

void* _alloc_layer_text (int n, const void* key, void* ctx) {
    _alloc_text_t* c = (_alloc_text_t*)ctx;
    Pico_Abs_Dim dim;
    SDL_Texture* tex = _tex_text(c->height, c->text, &dim);
    return _layer_new (
        0, PICO_LAYER_PLAIN, sizeof(Pico_Layer),
        (const char*)key, tex, dim
    );
}

void* _alloc_font (int n, const void* key, void* ctx) {
    _alloc_font_t* c = (_alloc_font_t*)ctx;
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

void* _alloc_sound (int n, const void* key, void* ctx) {
    const char* path = (const char*)ctx;
    return Mix_LoadWAV(path);
}
