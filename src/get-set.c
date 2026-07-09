#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "_pico.h"

///////////////////////////////////////////////////////////////////////////////
// GET
///////////////////////////////////////////////////////////////////////////////

Pico_Layer_Pencil pico_get_pencil (void) {
    _pico_guard();
    return G.layer->pencil;
}

Pico_Color pico_get_pencil_color (void) {
    _pico_guard();
    return G.layer->pencil.color;
}

const char* pico_get_pencil_font (void) {
    _pico_guard();
    return G.layer->pencil.font;
}

PICO_STYLE pico_get_pencil_style (void) {
    _pico_guard();
    return G.layer->pencil.style;
}

int pico_get_expert (int* fps) {
    _pico_guard();
    if (fps != NULL) {
        *fps = G.expert.fps;
    }
    return G.expert.on;
}

Pico_Abs_Dim pico_get_image (Pico_Rel_Dim* rel, const char* path) {
    _pico_guard();
    if (rel == NULL) {
        Pico_Layer* layer = _pico_layer_image('=', NULL, path);
        return layer->scene.dim;
    } else if (rel->w==0 || rel->h==0) {
        Pico_Layer* layer = _pico_layer_image('=', NULL, path);
        return _pico_abs_dim(rel, NULL, &layer->scene.dim);
    } else {
        return _pico_abs_dim(rel, NULL, NULL);
    }
}


const char* pico_get_layer (void) {
    _pico_guard();
    return G.layer->name;
}


Uint32 pico_get_now (void) {
    _pico_guard();
    return SDL_GetTicks();
}

Pico_Layer_Effect pico_get_effect (void) {
    _pico_guard();
    return G.layer->effect;
}

unsigned char pico_get_effect_alpha (void) {
    _pico_guard();
    return G.layer->effect.alpha;
}

Pico_Color pico_get_effect_color (void) {
    _pico_guard();
    return G.layer->effect.color;
}

PICO_FLIP pico_get_effect_flip (void) {
    _pico_guard();
    return G.layer->effect.flip;
}

int pico_get_effect_grid (void) {
    _pico_guard();
    return G.layer->effect.grid;
}

Pico_Rot pico_get_effect_rotate (void) {
    _pico_guard();
    return G.layer->effect.rotate;
}

Pico_Abs_Dim pico_get_text (Pico_Rel_Dim* rel, const char* text) {
    _pico_guard();
    return pico_get_text_mode('=', NULL, rel, text);
}

Pico_Abs_Dim pico_get_text_mode (
    int mode, const char* key,
    Pico_Rel_Dim* rel, const char* text
) {
    _pico_guard();
    if (text[0] == '\0') return (Pico_Abs_Dim){0, 0};

    assert(rel!=NULL && rel->h!=0);
    if (rel->w == 0) {
        // auto-width: report the NATIVE drawn size (same as
        // pico_output_draw_text_mode blits) so measured == drawn. the
        // requested height only picks the ptsize; the box height is the
        // font's content-independent cell (>= requested).
        Pico_Rel_Dim rel_h = { rel->mode, {0, rel->h} };
        int h = _pico_abs_dim(&rel_h, NULL, NULL).h;
        Pico_Layer* layer = _pico_layer_text(mode, key, h, text);
        Pico_Abs_Dim nat = layer->scene.dim;
        *rel = _pico_rel_dim(nat, rel->mode);
        return nat;
    } else {
        return _pico_abs_dim(rel, NULL, NULL);
    }
}

void pico_get_scene (Pico_Layer_Scene* view) {
    _pico_guard();
    *view = G.layer->scene;
}

Pico_Rel_Rect pico_get_scene_clip (void) {
    _pico_guard();
    return G.layer->scene.clip;
}

Pico_Abs_Dim pico_get_scene_dim (void) {
    _pico_guard();
    return G.layer->scene.dim;
}

Pico_Rel_Rect pico_get_scene_dst (void) {
    _pico_guard();
    return G.layer->scene.dst;
}

int pico_get_scene_clear (void) {
    _pico_guard();
    return G.layer->scene.clear;
}

Pico_Rel_Rect pico_get_scene_src (void) {
    _pico_guard();
    return G.layer->scene.src;
}

Pico_Abs_Dim pico_get_scene_tile (void) {
    _pico_guard();
    return G.layer->scene.tile;
}

Pico_Window pico_get_window (void) {
    _pico_guard();
    return (Pico_Window) {
        .fs    = G.window.pub.fs,
        .show  = SDL_GetWindowFlags(G.window.win) & SDL_WINDOW_SHOWN,
        .title = SDL_GetWindowTitle(G.window.win),
    };
}

int pico_get_window_fs (void) {
    _pico_guard();
    return G.window.pub.fs;
}

int pico_get_window_show (void) {
    _pico_guard();
    return SDL_GetWindowFlags(G.window.win) & SDL_WINDOW_SHOWN;
}

const char* pico_get_window_title (void) {
    _pico_guard();
    return SDL_GetWindowTitle(G.window.win);
}

///////////////////////////////////////////////////////////////////////////////
// SET
///////////////////////////////////////////////////////////////////////////////

void pico_set_dim (Pico_Rel_Dim dim) {
    _pico_guard();
    const char* old = pico_set_layer("window");
    pico_set_scene_dim(dim);
    pico_set_layer("world");
    pico_set_scene_dim(dim);
    pico_set_layer(old);
}

void pico_set_pencil (Pico_Layer_Pencil pencil) {
    _pico_guard();
    G.layer->pencil = pencil;
}

void pico_set_pencil_color (Pico_Color color) {
    _pico_guard();
    G.layer->pencil.color = color;
}

void pico_set_pencil_font (const char* path) {
    _pico_guard();
    G.layer->pencil.font = path;
}

void pico_set_pencil_style (PICO_STYLE style) {
    _pico_guard();
    G.layer->pencil.style = style;
}

int pico_set_expert (int on, int fps) {
    _pico_guard();
    assert(fps >= -1);
    G.expert.on  = on;
    G.expert.fps = fps;
    {
        if (fps == 0) {
            G.expert.ms = -1;
        } else if (fps == -1) {
            G.expert.ms = 0;
        } else {
            G.expert.ms = 1000 / fps;
        }
        G.expert.t0 = SDL_GetTicks();
    }
    G.world.effect.grid = 0;
    return G.expert.ms;
}

const char* pico_set_layer (const char* key) {
    _pico_guard();
    const char* old = (G.layer == NULL) ? NULL : G.layer->name;
    Pico_Layer* data = (Pico_Layer*)realm_get (
        G.realm, strlen(key)+1, key
    );
    assert(data!=NULL && "layer does not exist");
    //assert(data->type!=PICO_LAYER_SUB &&
    //    "cannot set render target to sub-layer");
    G.layer = data;

    SDL_SetRenderTarget(G.window.ren, G.layer->tex);
    Pico_Abs_Rect r = _pico_abs_rect(G.layer->scene.clip, NULL, NULL);
    SDL_RenderSetClipRect(G.window.ren, &r);
    return old;
}


void pico_set_effect (Pico_Layer_Effect effect) {
    _pico_guard();
    pico_set_effect_alpha (effect.alpha);
    pico_set_effect_color (effect.color);
    pico_set_effect_flip  (effect.flip);
    pico_set_effect_grid  (effect.grid);
    pico_set_effect_rotate(effect.rotate);
}

void pico_set_effect_alpha (unsigned char alpha) {
    _pico_guard();
    G.layer->effect.alpha = alpha;
    _pico_output_present(0);
}

void pico_set_effect_color (Pico_Color color) {
    _pico_guard();
    Pico_Layer* L = G.layer;
    L->effect.color = color;
    // repaint the background of a cascade-clear layer immediately, so its
    // effect color takes effect without an explicit pico_output_clear
    if (L->scene.clear && L->hier.up!=NULL) {
        assert(L->type != PICO_LAYER_SUB); // clear would corrupt parent
        SDL_SetRenderTarget(G.window.ren, L->tex);
        SDL_SetRenderDrawColor(G.window.ren, color.r, color.g, color.b, color.a);
        SDL_RenderClear(G.window.ren);
        SDL_SetRenderTarget(G.window.ren, G.layer->tex);
    }
    _pico_output_present(0);
}

void pico_set_effect_flip (PICO_FLIP flip) {
    _pico_guard();
    G.layer->effect.flip = flip;
    _pico_output_present(0);
}

void pico_set_effect_grid (int on) {
    _pico_guard();
    G.layer->effect.grid = on;
    _pico_output_present(0);
}

void pico_set_effect_rotate (Pico_Rot rotate) {
    _pico_guard();
    G.layer->effect.rotate = rotate;
    _pico_output_present(0);
}

void pico_set_scene (Pico_Layer_Scene view) {
    _pico_guard();
    Pico_Layer* L = G.layer;
    if (L->tex != NULL) {
        SDL_DestroyTexture(L->tex);
    }
    L->scene = view;
    L->tex = _pico_tex_create(view.dim);
    SDL_BlendMode mode = (L == &G.world) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND;
    SDL_SetTextureBlendMode(L->tex, mode);
    SDL_SetRenderTarget(G.window.ren, L->tex);
    Pico_Abs_Rect r = _pico_abs_rect(L->scene.clip, NULL, NULL);
    SDL_RenderSetClipRect(G.window.ren, &r);
    pico_output_clear();
    _pico_output_present(0);
}

void pico_set_scene_clip (Pico_Rel_Rect clip) {
    _pico_guard();
    Pico_Layer* L = G.layer;
    L->scene.clip = clip;
    Pico_Abs_Rect r = _pico_abs_rect(clip, NULL, NULL);
    SDL_SetRenderTarget(G.window.ren, L->tex);
    SDL_RenderSetClipRect(G.window.ren, &r);
    _pico_output_present(0);
}

void pico_set_scene_dim (Pico_Rel_Dim dim) {
    _pico_guard();
    assert(dim.mode != '%');
    Pico_Layer* L = G.layer;
    Pico_Abs_Dim di = _pico_abs_dim(&dim, NULL, NULL);
    L->scene.dim = di;
    assert(L->tex != NULL);
    SDL_DestroyTexture(L->tex);
    L->tex = _pico_tex_create(di);
    pico_assert_0 (
        SDL_SetTextureBlendMode (
            L->tex,
            (L==&G.window.layer || L==&G.world) ?
                SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND
        )
    );

    Pico_Abs_Rect r = _pico_abs_rect(L->scene.clip, NULL, NULL);
    SDL_SetRenderTarget(G.window.ren, L->tex);
    SDL_RenderSetClipRect(G.window.ren, &r);

    if (L == &G.window.layer) {
        assert(!G.window.pub.fs);
        SDL_SetWindowSize(G.window.win, di.w, di.h);
        _pico_output_present(0);
    } else {
        pico_output_clear();
    }
}

void pico_set_scene_dst (Pico_Rel_Rect dst) {
    _pico_guard();
    // stored raw; resolved lazily at each use (draw + cv/vs walks)
    // with the layer's dim as aspect ratio for w/h==0
    G.layer->scene.dst = dst;
    _pico_output_present(0);
}

void pico_set_scene_clear (int on) {
    _pico_guard();
    Pico_Layer* L = G.layer;
    if (L->scene.clear != on) {
        assert(L != &G.world           && "cannot set clear on world");
        assert(L != &G.window.layer    && "cannot set clear on window");
        assert(L->type!=PICO_LAYER_SUB && "cannot set clear on sub-layer");
    }
    L->scene.clear = on;
}

void pico_set_scene_src (Pico_Rel_Rect src) {
    _pico_guard();
    G.layer->scene.src = src;
    _pico_output_present(0);
}

void pico_set_scene_tile (Pico_Abs_Dim tile) {
    _pico_guard();
    G.layer->scene.tile = tile;
    _pico_output_present(0);
}

void pico_set_window (Pico_Window win) {
    _pico_guard();
    pico_set_window_fs(win.fs);
    pico_set_window_show(win.show);
    pico_set_window_title(win.title);
}

void pico_set_window_fs (int fs) {
    _pico_guard();
    if (fs == G.window.pub.fs) {
        return;
    }
    G.window.ing.fs = 1;
    if (fs) {
        int ret = SDL_SetWindowFullscreen(G.window.win, SDL_WINDOW_FULLSCREEN_DESKTOP);
        pico_assert(ret == 0);
        pico_input_delay(50);    // TODO: required for some reason
    } else {
        pico_assert(0 == SDL_SetWindowFullscreen(G.window.win, 0));
    }
    G.window.pub.fs = fs;
    _pico_output_present(0);
}

void pico_set_window_show (int on) {
    _pico_guard();
    if (on) {
        SDL_ShowWindow(G.window.win);
        _pico_output_present(0);
    } else {
        SDL_HideWindow(G.window.win);
    }
}

void pico_set_window_title (const char* title) {
    _pico_guard();
    SDL_SetWindowTitle(G.window.win, title);
    _pico_output_present(0);
}


