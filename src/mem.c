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
// CACHE
///////////////////////////////////////////////////////////////////////////////

// Private memo table behind the easy paths (draw_image, draw_text,
// draw_video, sound_path, pencil_font_path): string key -> resource.
// Entries are never exposed to the user, never evicted, and released
// together at pico_close.

static unsigned long _cache_hash (const char* key) {
    unsigned long hash = 5381;
    for (const char* p=key; *p!='\0'; p++) {
        hash = ((hash << 5) + hash) + (unsigned char)*p;
    }
    return hash % PICO_HASH_BUK;
}

void* _pico_cache_get (const char* key) {
    Pico_Cache_Entry* e = G.cache[_cache_hash(key)];
    while (e != NULL) {
        if (strcmp(e->key, key) == 0) {
            return e->value;
        }
        e = e->next;
    }
    return NULL;
}

void _pico_cache_put (const char* key, PICO_CACHE kind, void* value) {
    assert(value != NULL);
    Pico_Cache_Entry* e = malloc(sizeof(Pico_Cache_Entry));
    assert(e != NULL);
    e->key = malloc(strlen(key) + 1);
    assert(e->key != NULL);
    strcpy(e->key, key);
    e->kind  = kind;
    e->value = value;
    unsigned long idx = _cache_hash(key);
    e->next = G.cache[idx];
    G.cache[idx] = e;
}

void _pico_cache_close (void) {
    for (int i=0; i<PICO_HASH_BUK; i++) {
        Pico_Cache_Entry* e = G.cache[i];
        while (e != NULL) {
            Pico_Cache_Entry* nxt = e->next;
            switch (e->kind) {
                case PICO_CACHE_LAYER:
                    _pico_mem_layer_free((Pico_Layer*)e->value);
                    break;
                case PICO_CACHE_FONT:
                    _pico_mem_font_free((Pico_Font*)e->value);
                    break;
                case PICO_CACHE_SOUND:
                    _pico_mem_sound_free((Pico_Sound*)e->value);
                    break;
            }
            free(e->key);
            free(e);
            e = nxt;
        }
        G.cache[i] = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
// OWNERSHIP
///////////////////////////////////////////////////////////////////////////////

// Tracks user-owned resources (pico_layer_*, pico_font_load,
// pico_sound_load), so that pico_close can release whatever the
// program still holds.

void _pico_mem_own (Pico_Own* own, int cached, int kind, void* self) {
    if (cached) {
        return;
    }
    own->kind = kind;
    own->self = self;
    own->prv  = NULL;
    own->nxt  = G.owned;
    if (G.owned != NULL) {
        G.owned->prv = own;
    }
    G.owned = own;
    G.live++;
}

void _pico_mem_disown (Pico_Own* own, int cached) {
    if (cached) {
        return;
    }
    assert(G.live > 0);
    if (own->prv == NULL) {
        assert(G.owned == own);
        G.owned = own->nxt;
    } else {
        own->prv->nxt = own->nxt;
    }
    if (own->nxt != NULL) {
        own->nxt->prv = own->prv;
    }
    G.live--;
}

static void _layer_release (Pico_Layer* data);

// Releases every user-owned resource, in no particular order and
// with no hierarchy checks: only for teardown, when nothing is used
// afterwards.
void _pico_mem_free_all (void) {
    Pico_Own* own = G.owned;
    while (own != NULL) {
        Pico_Own* nxt = own->nxt;
        switch (own->kind) {
            case PICO_OWN_LAYER:
                _layer_release((Pico_Layer*)own->self);
                break;
            case PICO_OWN_FONT: {
                Pico_Font* font = (Pico_Font*)own->self;
                _pico_mem_font_done(font);
                free(font->path);
                free(font);
                break;
            }
            case PICO_OWN_SOUND: {
                Pico_Sound* sound = (Pico_Sound*)own->self;
                Mix_FreeChunk(sound->mix);
                free(sound);
                break;
            }
        }
        own = nxt;
    }
    G.owned = NULL;
    G.live  = 0;
}

///////////////////////////////////////////////////////////////////////////////
// LAYERS
///////////////////////////////////////////////////////////////////////////////

Pico_Layer* _pico_mem_layer_new (
    int cached, int clear, int type, size_t size,
    SDL_Texture* tex, Pico_Abs_Dim dim
) {
    Pico_Layer* data = calloc(1, size);
    assert(data != NULL);
    *data = (Pico_Layer) {
        .type   = type,
        .cached = cached,
        .subs   = 0,
        .tex    = tex,
        .pencil = {
            .color={0xFF, 0xFF, 0xFF, 0xFF}, .font=NULL, .style=PICO_STYLE_FILL
        },
        .effect = {
            .alpha=0xFF, .color=PICO_COLOR_TRANSPARENT, .flip=PICO_FLIP_NONE, .grid=0, .rotate={0, PICO_ANCHOR_C}
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
    _pico_mem_own(&data->own, cached, PICO_OWN_LAYER, data);
    return data;
}

// Releases a layer: splices it out of its parent, drops its claim on a
// borrowed texture (sub-layer), and frees its own resources. The layer
// must have no children and no live sub-layers.
void _pico_mem_layer_free (Pico_Layer* data) {
    assert(data->hier.dn.fst==NULL && "cannot free a layer with children");
    assert(data->subs==0 && "cannot free a layer with live sub-layers");
    _pico_layer_detach(data);
    if (data->type == PICO_LAYER_SUB) {
        Pico_Layer* src = ((Pico_Layer_Sub*)data)->src;
        assert(src->subs > 0);
        src->subs--;
    }
    _pico_mem_disown(&data->own, data->cached);
    _layer_release(data);
}

// frees the layer's own resources only (texture, video state, struct)
static void _layer_release (Pico_Layer* data) {
    if (data->type == PICO_LAYER_VIDEO) {
        _pico_video_free_layer((Pico_Layer_Video*)data);
    }
    if (data->type != PICO_LAYER_SUB) {
        SDL_DestroyTexture(data->tex);
    }
    free(data);
}

Pico_Layer* _pico_mem_layer_pixmap (
    int cached, Pico_Abs_Dim dim, const Pico_Color* pixels
) {
    SDL_Surface* sfc = SDL_CreateRGBSurfaceWithFormatFrom(
        (void*)pixels, dim.w, dim.h,
        32, 4 * dim.w, SDL_PIXELFORMAT_RGBA32
    );
    SDL_Texture* tex = SDL_CreateTextureFromSurface(G.window.ren, sfc);
    pico_assert(tex != NULL);
    SDL_FreeSurface(sfc);
    return _pico_mem_layer_new (
        cached, 0, PICO_LAYER_PLAIN, sizeof(Pico_Layer), tex, dim
    );
}

Pico_Layer* _pico_mem_layer_shot (
    int cached, Pico_Layer* src, const Pico_Rel_Rect* rect
) {
    // same capture as pico_output_screenshot, then upload to a texture
    // instead of writing a PNG
    SDL_Surface* sfc = _pico_shot(src, rect);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(G.window.ren, sfc);
    pico_assert(tex != NULL);
    Pico_Abs_Dim dim = { sfc->w, sfc->h };
    SDL_FreeSurface(sfc);

    return _pico_mem_layer_new (
        cached, 0, PICO_LAYER_PLAIN, sizeof(Pico_Layer), tex, dim
    );
}

Pico_Layer* _pico_mem_layer_empty (
    int cached, Pico_Layer* up, int clear,
    Pico_Rel_Dim rdim, Pico_Rel_Dim* rtile
) {
    // resolve (dim, tile) jointly; '%' dim against parent's scene.dim
    Pico_Abs_Dim dim, tile;
    if (up == NULL) {
        // falls back to G.layer->scene.dim
        _pico_resolve_dim_tile(rdim, rtile, NULL, &dim, &tile);
    } else {
        Pico_Abs_Rect base = (Pico_Abs_Rect) {
            0, 0, up->scene.dim.w, up->scene.dim.h
        };
        _pico_resolve_dim_tile(rdim, rtile, &base, &dim, &tile);
    }

    SDL_Texture* tex = _pico_tex_create(dim);
    Pico_Layer* lay = _pico_mem_layer_new (
        cached, clear, PICO_LAYER_PLAIN, sizeof(Pico_Layer), tex, dim
    );

    // clear new layer
    Pico_Layer* old = _pico_set_layer(lay);
    pico_output_clear();
    _pico_set_layer(old);

    if (rtile != NULL) {
        lay->scene.tile = tile;
    }
    return lay;
}

Pico_Layer* _pico_mem_layer_image (int cached, const char* path) {
    SDL_Texture* tex = IMG_LoadTexture(G.window.ren, path);
    pico_assert(tex != NULL);
    Pico_Abs_Dim dim;
    SDL_QueryTexture(tex, NULL, NULL, &dim.w, &dim.h);
    return _pico_mem_layer_new (
        cached, 0, PICO_LAYER_PLAIN, sizeof(Pico_Layer), tex, dim
    );
}

Pico_Layer* _pico_mem_layer_sub (
    int cached, Pico_Layer* src, Pico_Rel_Rect crop
) {
    Pico_Abs_Rect base = {0, 0, src->scene.dim.w, src->scene.dim.h};
    Pico_Abs_Rect abs = _pico_abs_rect(crop, &base, NULL);
    Pico_Layer* data = _pico_mem_layer_new (
        cached, 0, PICO_LAYER_SUB, sizeof(Pico_Layer_Sub),
        src->tex, (Pico_Abs_Dim){abs.w, abs.h}
    );
    data->scene.src = crop;
    ((Pico_Layer_Sub*)data)->src = src;
    ((Pico_Layer_Sub*)data)->sup = src->scene.dim;
    src->subs++;
    return data;
}

static SDL_Texture* _tex_text (int height, const char* text, Pico_Abs_Dim* dim) {
    SDL_Color c = {
        G.layer->pencil.color.r,
        G.layer->pencil.color.g,
        G.layer->pencil.color.b,
        0xFF,
    };
    // pad the raster to a content-independent box height so the layer
    // dim depends only on the font size, never on the string's
    // ascenders/descenders -- a fixed box keeps a non-top anchor from
    // turning a per-string height change into a vertical snap on reveal.
    // the box is the max of the line-skip and the reference cell
    // "|gjpqyA" (tallest glyph + deepest descenders; 'A' because some
    // brush/decorative fonts render caps taller than '|'), which bounds
    // any string's raster (metrics under-report the raster, so we
    // measure it).
    TTF_Font* ttf = _pico_font_ttf(G.layer->pencil.font, height);
    int ref;
    TTF_SizeText(ttf, "|gjpqyA", &(int){0}, &ref);
    int H = ref > TTF_FontLineSkip(ttf) ? ref : TTF_FontLineSkip(ttf);

    SDL_Surface* sfc = TTF_RenderText_Solid(ttf, text, c);
    pico_assert(sfc != NULL);
    assert(sfc->h <= H && "text raster exceeds reference cell");

    SDL_Surface* box = SDL_CreateRGBSurfaceWithFormat (
        0, sfc->w, H, 32, SDL_PIXELFORMAT_RGBA32
    );
    pico_assert(box != NULL);
    SDL_SetSurfaceBlendMode(sfc, SDL_BLENDMODE_NONE);
    SDL_BlitSurface(sfc, NULL, box, NULL);

    SDL_Texture* tex = SDL_CreateTextureFromSurface(G.window.ren, box);
    pico_assert(tex != NULL);
    *dim = (Pico_Abs_Dim){ sfc->w, H };
    SDL_FreeSurface(sfc);
    SDL_FreeSurface(box);
    return tex;
}

Pico_Layer* _pico_mem_layer_text (int cached, int height, const char* text) {
    Pico_Abs_Dim dim;
    SDL_Texture* tex = _tex_text(height, text, &dim);
    return _pico_mem_layer_new (
        cached, 0, PICO_LAYER_PLAIN, sizeof(Pico_Layer), tex, dim
    );
}

///////////////////////////////////////////////////////////////////////////////
// FONTS
///////////////////////////////////////////////////////////////////////////////

Pico_Font* _pico_mem_font_new (int cached, const char* path) {
    assert(path!=NULL && "font path required");
    Pico_Font* font = calloc(1, sizeof(Pico_Font));
    assert(font != NULL);
    font->id     = ++G.fonts;
    font->cached = cached;
    font->path   = malloc(strlen(path) + 1);
    assert(font->path != NULL);
    strcpy(font->path, path);

    // fail early on a bad path (sizes are otherwise opened lazily)
    TTF_Font* ttf = TTF_OpenFont(path, 10);
    pico_assert(ttf != NULL);
    TTF_CloseFont(ttf);

    _pico_mem_own(&font->own, cached, PICO_OWN_FONT, font);
    return font;
}

// closes every opened size (the struct itself stays valid)
void _pico_mem_font_done (Pico_Font* font) {
    for (int i=0; i<font->n; i++) {
        TTF_CloseFont(font->sizes[i].ttf);
    }
    free(font->sizes);
    font->sizes = NULL;
    font->n = 0;
}

void _pico_mem_font_free (Pico_Font* font) {
    _pico_mem_font_done(font);
    free(font->path);
    _pico_mem_disown(&font->own, font->cached);
    free(font);
}

// the TTF_Font of `font` at pixel height `h` (NULL = built-in font),
// opened on first use and kept for the lifetime of the font
TTF_Font* _pico_font_ttf (Pico_Font* font, int h) {
    if (font == NULL) {
        font = &G.font;
    }
    for (int i=0; i<font->n; i++) {
        if (font->sizes[i].h == h) {
            return font->sizes[i].ttf;
        }
    }

    TTF_Font* ttf;
    if (font->path == NULL) {
        SDL_RWops* rw = SDL_RWFromConstMem(pico_tiny_ttf, pico_tiny_ttf_len);
        ttf = TTF_OpenFontRW(rw, 1, h);
    } else {
        ttf = TTF_OpenFont(font->path, h);
    }
    pico_assert(ttf != NULL);

    font->sizes = realloc(font->sizes, (font->n+1) * sizeof(*font->sizes));
    assert(font->sizes != NULL);
    font->sizes[font->n].h   = h;
    font->sizes[font->n].ttf = ttf;
    font->n++;
    return ttf;
}

///////////////////////////////////////////////////////////////////////////////
// SOUNDS
///////////////////////////////////////////////////////////////////////////////

Pico_Sound* _pico_mem_sound_new (int cached, const char* path) {
    assert(path!=NULL && "sound path required");
    Mix_Chunk* mix = Mix_LoadWAV(path);
    pico_assert(mix != NULL);
    Pico_Sound* sound = calloc(1, sizeof(Pico_Sound));
    assert(sound != NULL);
    sound->cached = cached;
    sound->mix    = mix;
    _pico_mem_own(&sound->own, cached, PICO_OWN_SOUND, sound);
    return sound;
}

void _pico_mem_sound_free (Pico_Sound* sound) {
    Mix_FreeChunk(sound->mix);
    _pico_mem_disown(&sound->own, sound->cached);
    free(sound);
}
