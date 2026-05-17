#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include "pico.h"
#include "state.h"
#include "aux.h"
#include "layers.h"
#include "mem.h"

///////////////////////////////////////////////////////////////////////////////
// OUTPUT
///////////////////////////////////////////////////////////////////////////////

void pico_output_clear (void) {
    _pico_guard();
    SDL_SetRenderDrawColor(G.window.ren,
        G.layer->effect.color.r, G.layer->effect.color.g, G.layer->effect.color.b, G.layer->effect.color.a
    );
    Pico_Abs_Rect r = _pico_rnd_rect(_pico_raw_rect(G.layer->scene.clip, NULL, NULL));
    SDL_RenderFillRect(G.window.ren, &r);
    _pico_output_present(0);
}

void pico_output_draw_image (const char* path, Pico_Rel_Rect rect) {
    _pico_guard();
    Pico_Layer* layer = _pico_layer_image('=', NULL, path);
    Pico_Abs_Dim* orig = (rect.w==0 || rect.h==0) ? &layer->scene.dim : NULL;
    Pico_Rel_Dim rel = { rect.mode, {rect.w, rect.h} };
    _pico_raw_dim(&rel, NULL, orig);
    rect.w = rel.w;
    rect.h = rel.h;
    _pico_output_draw_layer(layer, &rect);
}

void pico_output_draw_layer (const char* key, const Pico_Rel_Rect* rect) {
    _pico_guard();
    assert(key!=NULL && "layer key required");

    Pico_Layer* layer = (Pico_Layer*)realm_get(
        G.realm, strlen(key)+1, key);
    pico_assert(layer!=NULL && "layer does not exist");

    _pico_output_draw_layer(layer, rect);
}

void pico_output_draw_line (Pico_Rel_Pos p1, Pico_Rel_Pos p2) {
    _pico_guard();
    SDL_SetRenderDrawColor(G.window.ren,
        G.layer->pencil.color.r, G.layer->pencil.color.g, G.layer->pencil.color.b, G.layer->pencil.color.a
    );
    Pico_Abs_Pos i1 = _pico_rnd_pos(_pico_raw_pos(p1, NULL));
    Pico_Abs_Pos i2 = _pico_rnd_pos(_pico_raw_pos(p2, NULL));
    SDL_RenderDrawLine(G.window.ren, i1.x,i1.y, i2.x,i2.y);
    _pico_output_present(0);
}

void pico_output_draw_oval (Pico_Rel_Rect rect) {
    _pico_guard();
    Pico_Abs_Rect i = _pico_rnd_rect(_pico_raw_rect(rect, NULL, NULL));
    SDL_SetRenderDrawColor(G.window.ren,
        G.layer->pencil.color.r, G.layer->pencil.color.g, G.layer->pencil.color.b, G.layer->pencil.color.a
    );
    switch (G.layer->pencil.style) {
        case PICO_STYLE_FILL:
            filledEllipseRGBA (G.window.ren,
                i.x+i.w/2, i.y+i.h/2, i.w/2, i.h/2,
                G.layer->pencil.color.r, G.layer->pencil.color.g, G.layer->pencil.color.b, G.layer->pencil.color.a
            );
            break;
        case PICO_STYLE_STROKE:
            ellipseRGBA (G.window.ren,
                i.x+i.w/2, i.y+i.h/2, i.w/2, i.h/2,
                G.layer->pencil.color.r, G.layer->pencil.color.g, G.layer->pencil.color.b, G.layer->pencil.color.a
            );
            break;
    }
    _pico_output_present(0);
}

void pico_output_draw_pixel (Pico_Rel_Pos pos) {
    _pico_guard();
    SDL_SetRenderDrawColor(G.window.ren,
        G.layer->pencil.color.r, G.layer->pencil.color.g, G.layer->pencil.color.b, G.layer->pencil.color.a
    );
    Pico_Abs_Pos i = _pico_rnd_pos(_pico_raw_pos(pos, NULL));
    SDL_RenderDrawPoint(G.window.ren, i.x, i.y);
        // TODO: could use PointF, but 4.5->4 (not 5 desired)
    _pico_output_present(0);
}

void pico_output_draw_pixels (int n, const Pico_Rel_Pos* ps) {
    _pico_guard();
    Pico_Abs_Pos vs[n];
    for (int i=0; i<n; i++) {
        vs[i] = _pico_rnd_pos(_pico_raw_pos(ps[i], NULL));
    }
    SDL_SetRenderDrawColor(G.window.ren,
        G.layer->pencil.color.r, G.layer->pencil.color.g, G.layer->pencil.color.b, G.layer->pencil.color.a
    );
    SDL_RenderDrawPoints(G.window.ren, vs, n);
    _pico_output_present(0);
}

void pico_output_draw_pixmap (
    const char* key,
    Pico_Abs_Dim dim,
    const Pico_Color pixmap[],
    Pico_Rel_Rect rect
) {
    _pico_guard();
    assert(key!=NULL && "layer key required");
    pico_layer_pixmap_mode('=', NULL, key, dim, pixmap);
    pico_output_draw_layer(key, &rect);
}

void pico_output_draw_poly (int n, const Pico_Rel_Pos* ps) {
    _pico_guard();
    Sint16 xs[n], ys[n];
    for (int i=0; i<n; i++) {
        Pico_Abs_Pos v = _pico_rnd_pos(_pico_raw_pos(ps[i], NULL));
        xs[i] = v.x;
        ys[i] = v.y;
    }
    SDL_SetRenderDrawColor(G.window.ren,
        G.layer->pencil.color.r, G.layer->pencil.color.g, G.layer->pencil.color.b, G.layer->pencil.color.a
    );
    switch (G.layer->pencil.style) {
        case PICO_STYLE_FILL:
            filledPolygonRGBA(G.window.ren,
                xs, ys, n,
                G.layer->pencil.color.r, G.layer->pencil.color.g, G.layer->pencil.color.b, G.layer->pencil.color.a
            );
            break;
        case PICO_STYLE_STROKE:
            polygonRGBA(G.window.ren,
                xs, ys, n,
                G.layer->pencil.color.r, G.layer->pencil.color.g, G.layer->pencil.color.b, G.layer->pencil.color.a
            );
            break;
    }
    _pico_output_present(0);
}

void pico_output_draw_rect (Pico_Rel_Rect rect) {
    _pico_guard();
    SDL_SetRenderDrawColor(G.window.ren,
        G.layer->pencil.color.r, G.layer->pencil.color.g, G.layer->pencil.color.b, G.layer->pencil.color.a
    );

    Pico_Abs_Rect i = _pico_rnd_rect(_pico_raw_rect(rect, NULL, NULL));
    switch (G.layer->pencil.style) {
        case PICO_STYLE_FILL:
            SDL_RenderFillRect(G.window.ren, &i);
            break;
        case PICO_STYLE_STROKE:
            SDL_RenderDrawRect(G.window.ren, &i);
            break;
    }
    _pico_output_present(0);
}

void pico_output_draw_text (const char* text, Pico_Rel_Rect rect) {
    _pico_guard();
    pico_output_draw_text_mode('=', NULL, text, rect);
}

void pico_output_draw_text_mode (
    int mode, const char* key,
    const char* text, Pico_Rel_Rect rect
) {
    _pico_guard();
    assert(rect.h != 0);
    if (text[0] == '\0') return;

    SDL_FDim dim = _pico_raw_dim (
        &(Pico_Rel_Dim){ rect.mode, {0, rect.h} }, NULL, NULL
    );

    Pico_Layer* layer = _pico_layer_text(mode, key, dim.h, text);
    Pico_Abs_Dim* orig = (rect.w == 0) ? &layer->scene.dim : NULL;

    Pico_Rel_Dim rel = { rect.mode, {rect.w, rect.h} };
    _pico_raw_dim(&rel, NULL, orig);
    rect.w = rel.w;

    _pico_output_draw_layer(layer, &rect);
}

void pico_output_draw_tri (
    Pico_Rel_Pos p1, Pico_Rel_Pos p2, Pico_Rel_Pos p3
) {
    _pico_guard();
    Pico_Abs_Pos i1 = _pico_rnd_pos(_pico_raw_pos(p1, NULL));
    Pico_Abs_Pos i2 = _pico_rnd_pos(_pico_raw_pos(p2, NULL));
    Pico_Abs_Pos i3 = _pico_rnd_pos(_pico_raw_pos(p3, NULL));

    SDL_SetRenderDrawColor(G.window.ren,
        G.layer->pencil.color.r, G.layer->pencil.color.g, G.layer->pencil.color.b, G.layer->pencil.color.a
    );
    switch (G.layer->pencil.style) {
        case PICO_STYLE_FILL:
            filledTrigonRGBA(G.window.ren,
                i1.x, i1.y,
                i2.x, i2.y,
                i3.x, i3.y,
                G.layer->pencil.color.r, G.layer->pencil.color.g, G.layer->pencil.color.b, G.layer->pencil.color.a
            );
            break;
        case PICO_STYLE_STROKE:
            trigonRGBA(G.window.ren,
                i1.x, i1.y,
                i2.x, i2.y,
                i3.x, i3.y,
                G.layer->pencil.color.r, G.layer->pencil.color.g, G.layer->pencil.color.b, G.layer->pencil.color.a
            );
            break;
    }
    _pico_output_present(0);
}

void _pico_output_present (int force) {
    _pico_guard();
    if (G.window.ing.out) {
        return;
    } else if (force) {
        // ok
    } else if (G.expert.on) {
        return;
    } else if (!(G.layer==&G.world || G.layer==&G.window.layer)) {
        return;  // auto-present only on root layers (world or window)
    }

    G.window.ing.out = 1;

    if (!G.expert.on) {
        SDL_SetRenderTarget(G.window.ren, G.window.layer.tex);
        Pico_Color c = G.window.layer.effect.color;
        SDL_SetRenderDrawColor(G.window.ren, c.r, c.g, c.b, c.a);
        SDL_RenderClear(G.window.ren);
        _pico_output_draw_layers(&G.window.layer);
    }

    // mirror window.tex -> framebuffer
    SDL_SetRenderTarget(G.window.ren, NULL);
    SDL_RenderCopy(G.window.ren, G.window.layer.tex, NULL, NULL);
    SDL_RenderPresent(G.window.ren);

    G.window.ing.out = 0;

    // restore current layer's render target + clip
    SDL_SetRenderTarget(G.window.ren, G.layer->tex);
    Pico_Abs_Rect r = _pico_rnd_rect(_pico_raw_rect(G.layer->scene.clip, NULL, NULL));
    SDL_RenderSetClipRect(G.window.ren, &r);
}

void pico_output_present (void) {
    _pico_guard();
    _pico_output_present(1);
}

void pico_output_draw_layers (void) {
    _pico_guard();
    _pico_output_draw_layers(&G.window.layer);
    SDL_SetRenderTarget(G.window.ren, G.window.layer.tex);
}

static void _output_sound_cache (const char* path, int cache) {
    Mix_Chunk* mix = NULL;

    if (cache) {
        int n = strlen(path) + 1;
        mix = (Mix_Chunk*)realm_put(
            G.realm, '=', n, path,
            _pico_free_sound, _pico_alloc_sound, (void*)path
        );
    } else {
        mix = Mix_LoadWAV(path);
    }
    pico_assert(mix != NULL);

    Mix_PlayChannel(-1, mix, 0);

    if (!cache) {
        Mix_FreeChunk(mix);
    }
}

const char* pico_output_screenshot (const char* layer, const char* path, const Pico_Rel_Rect* rect) {
    _pico_guard();
    const char* old = NULL;
    if (layer != NULL) {
        old = pico_set_layer(layer);
    }
    Pico_Layer* L = G.layer;
    Pico_Abs_Rect ri = (rect == NULL)
        ? (Pico_Abs_Rect){0, 0, L->scene.dim.w, L->scene.dim.h}
        : _pico_rnd_rect(_pico_raw_rect(*rect, NULL, NULL));

    const char* ret = path;
    if (path == NULL) {
        static char _path_[32] = "";
        time_t ts = time(NULL);
        struct tm* ti = localtime(&ts);
        assert(strftime(_path_, 32, "pico-sdl-%Y%m%d-%H%M%S.png", ti) == 28);
        ret = _path_;
    }

    // window screenshots read the framebuffer directly (SDL forces alpha=255
    // for fb pixels, matching what the user sees on screen)
    SDL_Texture* tex = (L == &G.window.layer) ? NULL : L->tex;
    SDL_Texture* tmp = NULL;
    if (tex != NULL) {
        int access;
        SDL_QueryTexture(tex, NULL, &access, NULL, NULL);
        if (access != SDL_TEXTUREACCESS_TARGET) {
            tmp = SDL_CreateTexture (
                G.window.ren, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
                L->scene.dim.w, L->scene.dim.h
            );
            pico_assert(tmp != NULL);
            SDL_SetRenderTarget(G.window.ren, tmp);
            SDL_RenderCopy(G.window.ren, tex, NULL, NULL);
            tex = tmp;
        }
    }
    SDL_SetRenderTarget(G.window.ren, tex);
    pico_input_delay(5);            // TODO: bug if removed

    void* buf = malloc(4 * ri.w * ri.h);
    assert(buf != NULL);
    SDL_RenderReadPixels(G.window.ren, &ri, SDL_PIXELFORMAT_RGBA32, buf, 4*ri.w);
    SDL_Surface* sfc = SDL_CreateRGBSurfaceWithFormatFrom (
        buf, ri.w, ri.h, 32, 4*ri.w, SDL_PIXELFORMAT_RGBA32
    );
    pico_assert(IMG_SavePNG(sfc, ret) == 0);
    free(buf);
    SDL_FreeSurface(sfc);
    if (tmp != NULL) {
        SDL_DestroyTexture(tmp);
    }

    if (layer != NULL) {
        pico_set_layer(old);
    }
    return ret;
}

void pico_output_sound (const char* path) {
    _pico_guard();
    _output_sound_cache(path, 1);
}
