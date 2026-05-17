#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <stdarg.h>
#include <math.h>

#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#define MAX(x,y) ((x) > (y) ? (x) : (y))

#define REALM_C
#include "realm.hc"
#undef REALM_C
#include "tiny_ttf.h"
#include "pico.h"

#define PICO_ANCHORS_C
#include "anchors.h"

///////////////////////////////////////////////////////////////////////////////
// DATA
///////////////////////////////////////////////////////////////////////////////

#include "aux.h"
#include "layers.h"
#include "mem.h"
#include "video.h"
#include "state.h"

PicoState G = { 0 };

///////////////////////////////////////////////////////////////////////////////
// AUX
///////////////////////////////////////////////////////////////////////////////

void _pico_guard (void) {
    if (!G.init) {
        fprintf(stderr, "ERROR : pico-sdl is not initialized\n");
        abort();
    }
}

SDL_Texture* _pico_tex_create (Pico_Abs_Dim dim) {
    SDL_Texture* tex = SDL_CreateTexture (
        G.window.ren, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
        dim.w, dim.h
    );
    pico_assert(tex != NULL);
    return tex;
}

TTF_Font* _pico_font_get (const char* path, int h) {
    const char* path_str = path ? path : "null";
    char key[256];
    snprintf(key, sizeof(key), "/font/%s/%d", path_str, h);
    int n = strlen(key) + 1;
    _pico_alloc_font_t ctx = { path, h };
    TTF_Font* ret = realm_put(
        G.realm, '=', n, key,
        _pico_free_font, _pico_alloc_font, &ctx
    );
    assert(ret != NULL);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////
// INIT
///////////////////////////////////////////////////////////////////////////////

void pico_init (int on) {
    if (on) {
        assert(G.init == 0);

        realm_t* realm = realm_open(PICO_HASH_BUK);
        {
            assert(realm != NULL);
            realm_enter(realm);
            realm_put (
                realm, '!', strlen("window")+1, "window", NULL, NULL, &G.window.layer
            );
            realm_put (
                realm, '!', strlen("world")+1,  "world",  NULL, NULL, &G.world
            );
        }

        pico_assert_0(SDL_Init(SDL_INIT_EVERYTHING));
        TTF_Init();
        Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 1024);

        SDL_Window*   win;
        SDL_Renderer* ren;
        SDL_Texture*  phy;
        SDL_Texture*  log;
        {
            // window
            win = SDL_CreateWindow (
                PICO_TITLE,
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                PICO_DIM_PHY.w, PICO_DIM_PHY.h,
                SDL_WINDOW_SHOWN
            );
            pico_assert_X(win);

            // renderer
            ren = SDL_CreateRenderer (
                win, -1,
                getenv("PICO_TESTS")
                    ? SDL_RENDERER_SOFTWARE
                    : SDL_RENDERER_ACCELERATED // |SDL_RENDERER_PRESENTVSYNC
            );
            pico_assert_X(ren);
            pico_assert_0 (
                SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND)
            );

            // textures
            G.window.ren = ren;
            phy = _pico_tex_create(PICO_DIM_PHY);
            pico_assert_0 (
                SDL_SetTextureBlendMode(phy, SDL_BLENDMODE_NONE)
            );

            log = _pico_tex_create(PICO_DIM_LOG);
            pico_assert_0 (
                SDL_SetTextureBlendMode(log, SDL_BLENDMODE_NONE)
            );
        }

        G = (typeof(G)) {
            .init  = 1,
            .realm = realm,
            .world = {
                .type = PICO_LAYER_PLAIN,
                .name = "world",
                .tex  = log,
                .pencil = {
                    .color = {0xFF, 0xFF, 0xFF, 0xFF},
                    .font  = NULL,
                    .style = PICO_STYLE_FILL,
                },
                .effect = {
                    .alpha  = 0xFF,
                    .color  = {0, 0, 0, 0xFF},
                    .flip   = PICO_FLIP_NONE,
                    .grid   = 1,
                    .rotate = {0, PICO_ANCHOR_C},
                },
                .hier = { NULL, NULL, {NULL,NULL} },
                .scene = {
                    .clear = 0,
                    .dim   = PICO_DIM_LOG,
                    .tile  = {0, 0},
                    .dst   = {'%', {.5,.5,1,1}, PICO_ANCHOR_C},
                    .src   = {'%', {.5,.5,1,1}, PICO_ANCHOR_C},
                    .clip  = {'%', {.5,.5,1,1}, PICO_ANCHOR_C},
                },
            },
            .layer  = NULL,
            .expert = {0, 0, -1, 0},
            .window = {
                .win = win,
                .ren = ren,
                .layer = {
                    .type = PICO_LAYER_PLAIN,
                    .name = "window",
                    .tex  = phy,
                    .pencil = {
                        .color = {0xFF, 0xFF, 0xFF, 0xFF},
                        .font  = NULL,
                        .style = PICO_STYLE_FILL,
                    },
                    .effect = {
                        .alpha  = 0xFF,
                        .color  = {0x77, 0x77, 0x77, 0xFF},
                        .flip   = PICO_FLIP_NONE,
                        .grid   = 0,
                        .rotate = {0, PICO_ANCHOR_C},
                    },
                    .hier = { NULL, NULL, {NULL,NULL} },
                    .scene = {
                        .clear = 0,
                        .dim  = PICO_DIM_PHY,
                        .tile = {0, 0},
                        .dst  = {'%', {.5,.5,1,1}, PICO_ANCHOR_C},
                        .src  = {'%', {.5,.5,1,1}, PICO_ANCHOR_C},
                        .clip = {'%', {.5,.5,1,1}, PICO_ANCHOR_C},
                    },
                },
                .ing = { 0, 0 },
                .pub = { .fs = 0 },
            },
        };

        _pico_layer_attach("window", "world");
        pico_set_layer("world");
        pico_output_clear();

        // prevents WMs to resize window at start
        SDL_PumpEvents();
        SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
        SDL_SetWindowResizable(G.window.win, 1);
    }
    else {
        assert(G.init == 1);
        G.init = 0;

        assert(G.realm != NULL);
        realm_close(G.realm);

        Mix_CloseAudio();
        TTF_Quit();

        assert(G.world.tex != NULL);
        SDL_DestroyTexture(G.world.tex);

        assert(G.window.layer.tex != NULL);
        SDL_DestroyTexture(G.window.layer.tex);

        assert(G.window.ren != NULL);
        SDL_DestroyRenderer(G.window.ren);

        assert(G.window.win != NULL);
        SDL_DestroyWindow(G.window.win);

        SDL_Quit();
    }
}

void pico_quit (void) {
    _pico_guard();
    SDL_Event e = { .type = SDL_QUIT };
    SDL_PushEvent(&e);
}

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

Pico_Abs_Dim pico_get_image (const char* path, Pico_Rel_Dim* rel) {
    _pico_guard();
    if (rel == NULL) {
        Pico_Layer* layer = _pico_layer_image('=', NULL, path);
        return layer->scene.dim;
    } else if (rel->w==0 || rel->h==0) {
        Pico_Layer* layer = _pico_layer_image('=', NULL, path);
        SDL_FDim fd = _pico_raw_dim(rel, NULL, &layer->scene.dim);
        return (Pico_Abs_Dim){fd.w, fd.h};
    } else {
        SDL_FDim fd = _pico_raw_dim(rel, NULL, NULL);
        return (Pico_Abs_Dim){fd.w, fd.h};
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

Pico_Abs_Dim pico_get_text (const char* text, Pico_Rel_Dim* rel) {
    _pico_guard();
    return pico_get_text_mode('=', NULL, text, rel);
}

Pico_Abs_Dim pico_get_text_mode (
    int mode, const char* key,
    const char* text, Pico_Rel_Dim* rel
) {
    _pico_guard();
    if (text[0] == '\0') return (Pico_Abs_Dim){0, 0};

    assert(rel!=NULL && rel->h!=0);
    if (rel->w == 0) {
        Pico_Rel_Dim rel_h = { rel->mode, {0, rel->h} };
        SDL_FDim fd_h = _pico_raw_dim(&rel_h, NULL, NULL);
        int height = (int)fd_h.h;
        Pico_Layer* layer = _pico_layer_text(mode, key, height, text);
        SDL_FDim fd = _pico_raw_dim(rel, NULL, &layer->scene.dim);
        return (Pico_Abs_Dim){fd.w, fd.h};
    } else {
        SDL_FDim fd = _pico_raw_dim(rel, NULL, NULL);
        return (Pico_Abs_Dim){fd.w, fd.h};
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
    G.world.scene.clear = on ? 1 : 0;
    return G.expert.ms;
}

const char* pico_set_layer (const char* key) {
    _pico_guard();
    const char* old = (G.layer == NULL) ? NULL : G.layer->name;
    Pico_Layer* data = (Pico_Layer*)realm_get (
        G.realm, strlen(key)+1, key
    );
    pico_assert(data!=NULL && "layer does not exist");
    //pico_assert(data->type!=PICO_LAYER_SUB &&
    //    "cannot set render target to sub-layer");
    G.layer = data;

    SDL_SetRenderTarget(G.window.ren, G.layer->tex);
    Pico_Abs_Rect r = _pico_rnd_rect(_pico_raw_rect(G.layer->scene.clip, NULL, NULL));
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
    Pico_Abs_Rect r = _pico_rnd_rect(_pico_raw_rect(L->scene.clip, NULL, NULL));
    SDL_RenderSetClipRect(G.window.ren, &r);
    pico_output_clear();
    _pico_output_present(0);
}

void pico_set_scene_clip (Pico_Rel_Rect clip) {
    _pico_guard();
    G.layer->scene.clip = clip;
    _pico_output_present(0);
}

void pico_set_scene_dim (Pico_Rel_Dim dim) {
    _pico_guard();
    assert(dim.mode != '%');
    Pico_Layer* L = G.layer;
    Pico_Abs_Dim di = _pico_rnd_dim(_pico_raw_dim(&dim, NULL, NULL));
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

    Pico_Abs_Rect r = _pico_rnd_rect(_pico_raw_rect(L->scene.clip, NULL, NULL));
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
    Pico_Layer* L = G.layer;
    assert(L->hier.up!=NULL && "scene.dst requires layer to be attached");
    if (dst.w==0 || dst.h==0) {
        // aspect-fill: resolve eagerly so cv walks and draw-time
        // see a concrete '!' rect. Pb (parent box) is offset for
        // '!' and '#' modes (size unused) and base for '%' mode.
        Pico_Layer* P = _pico_layer_name(L->hier.up);
        Pico_Abs_Rect Pb = {0, 0, P->scene.dim.w, P->scene.dim.h};
        SDL_FRect f = _pico_raw_rect(dst, &Pb, &L->scene.dim);
        dst = (Pico_Rel_Rect) {
            '!', {f.x, f.y, f.w, f.h}, PICO_ANCHOR_NW
        };
    }
    L->scene.dst = dst;
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


