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
#include "tiny_ttf.h"
#include "pico.h"

#define PICO_COLORS_C
#include "colors.hc"

#define PICO_ANCHORS_C
#include "anchors.h"

///////////////////////////////////////////////////////////////////////////////
// DATA
///////////////////////////////////////////////////////////////////////////////

#include "layers.hc"
#include "mem.hc"
#include "video.hc"

static struct {
    int           init;
    realm_t*      realm;
    Pico_Layer    world;
    Pico_Layer*   layer;            // current render target
    struct {
        int on;
        int fps;
        int ms;
        int t0;
    } expert;
    struct {
        SDL_Window*   win;
        SDL_Renderer* ren;
        Pico_Layer    layer;
        struct {
            int fs;
            int out;
        } ing;
        struct { int fs; } pub;
    } window;
} G = { 0 };

///////////////////////////////////////////////////////////////////////////////
// AUX
///////////////////////////////////////////////////////////////////////////////

static void _pico_output_present (int force);

static void _pico_guard (void) {
    if (!G.init) {
        fprintf(stderr, "ERROR : pico-sdl is not initialized\n");
        abort();
    }
}

static SDL_Texture* _tex_create (Pico_Abs_Dim dim) {
    SDL_Texture* tex = SDL_CreateTexture (
        G.window.ren, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
        dim.w, dim.h
    );
    pico_assert(tex != NULL);
    return tex;
}

static TTF_Font* _font_get (const char* path, int h) {
    const char* path_str = path ? path : "null";
    char key[256];
    snprintf(key, sizeof(key), "/font/%s/%d", path_str, h);
    int n = strlen(key) + 1;
    _alloc_font_t ctx = { path, h };
    TTF_Font* ret = realm_put(
        G.realm, '=', n, key,
        _free_font, _alloc_font, &ctx
    );
    assert(ret != NULL);
    return ret;
}

#include "aux.hc"
#include "geom.hc"

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
                #ifdef PICO_TESTS
                    SDL_RENDERER_SOFTWARE
                #else
                    SDL_RENDERER_ACCELERATED // |SDL_RENDERER_PRESENTVSYNC
                #endif
            );
            pico_assert_X(ren);
            pico_assert_0 (
                SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND)
            );

            // textures
            G.window.ren = ren;
            phy = _tex_create(PICO_DIM_PHY);
            pico_assert_0 (
                SDL_SetTextureBlendMode(phy, SDL_BLENDMODE_NONE)
            );

            log = _tex_create(PICO_DIM_LOG);
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

        _layer_attach("window", "world");
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
        SDL_FDim fd = _sdl_dim(rel, NULL, &layer->scene.dim);
        return (Pico_Abs_Dim){fd.w, fd.h};
    } else {
        SDL_FDim fd = _sdl_dim(rel, NULL, NULL);
        return (Pico_Abs_Dim){fd.w, fd.h};
    }
}

static Pico_Keyboard _pico_keyboard (int key, SDL_Keymod mod) {
    return (Pico_Keyboard) {
        .key   = key,
        .ctrl  = !!(mod & KMOD_CTRL),
        .shift = !!(mod & KMOD_SHIFT),
        .alt   = !!(mod & KMOD_ALT),
    };
}

Pico_Keyboard pico_get_keyboard (void) {
    _pico_guard();
    return _pico_keyboard(0, SDL_GetModState());
}

const char* pico_get_layer (void) {
    _pico_guard();
    return G.layer->name;
}

Pico_Mouse pico_get_mouse (const char* layer, Pico_Rel_Pos* pos) {
    _pico_guard();
    assert((pos->mode=='!' || pos->mode=='%' || pos->mode=='#'));

    SDL_Point phy;
    Uint32 masks = SDL_GetMouseState(&phy.x, &phy.y);

    Pico_Mouse m = {
        .mode   = pos->mode,
        .anchor = pos->anchor,
        .left   = !!(masks & SDL_BUTTON(SDL_BUTTON_LEFT)),
        .right  = !!(masks & SDL_BUTTON(SDL_BUTTON_RIGHT)),
        .middle = !!(masks & SDL_BUTTON(SDL_BUTTON_MIDDLE)),
    };

    pico_cv_pos (
        layer, pos, "window",
        &(Pico_Rel_Pos){'!', {phy.x, phy.y}, PICO_ANCHOR_NW}
    );
    m.x = pos->x;
    m.y = pos->y;

    return m;
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
        SDL_FDim fd_h = _sdl_dim(&rel_h, NULL, NULL);
        int height = (int)fd_h.h;
        Pico_Layer* layer = _pico_layer_text(mode, key, height, text);
        SDL_FDim fd = _sdl_dim(rel, NULL, &layer->scene.dim);
        return (Pico_Abs_Dim){fd.w, fd.h};
    } else {
        SDL_FDim fd = _sdl_dim(rel, NULL, NULL);
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
    Pico_Abs_Rect r = _rnd_rect(_sdl_rect(G.layer->scene.clip, NULL, NULL));
    SDL_RenderSetClipRect(G.window.ren, &r);
    return old;
}

void pico_set_mouse (const char* layer, Pico_Rel_Pos pos) {
    _pico_guard();
    Pico_Rel_Pos w = {'!', {}, PICO_ANCHOR_NW};
    pico_cv_pos("window", &w, layer, &pos);
    Pico_Abs_Pos wi = _rnd_pos((SDL_FPoint){w.x, w.y});
    SDL_WarpMouseInWindow(G.window.win, wi.x, wi.y);
    SDL_PumpEvents();
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
    L->tex = _tex_create(view.dim);
    SDL_BlendMode mode = (L == &G.world) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND;
    SDL_SetTextureBlendMode(L->tex, mode);
    SDL_SetRenderTarget(G.window.ren, L->tex);
    Pico_Abs_Rect r = _rnd_rect(_sdl_rect(L->scene.clip, NULL, NULL));
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
    Pico_Abs_Dim di = _rnd_dim(_sdl_dim(&dim, NULL, NULL));
    L->scene.dim = di;
    assert(L->tex != NULL);
    SDL_DestroyTexture(L->tex);
    L->tex = _tex_create(di);
    pico_assert_0 (
        SDL_SetTextureBlendMode (
            L->tex,
            (L==&G.window.layer || L==&G.world) ?
                SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND
        )
    );

    Pico_Abs_Rect r = _rnd_rect(_sdl_rect(L->scene.clip, NULL, NULL));
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
        SDL_FRect f = _sdl_rect(dst, &Pb, &L->scene.dim);
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

#define PICO_MEM_C
#include "mem.hc"

#define PICO_LAYERS_C
#include "layers.hc"

///////////////////////////////////////////////////////////////////////////////
// LAYER
///////////////////////////////////////////////////////////////////////////////

void pico_layer_pixmap (
    const char* up,
    const char* key,
    Pico_Abs_Dim dim,
    const Pico_Color* pixels
) {
    _pico_guard();
    pico_layer_pixmap_mode('!', up, key, dim, pixels);
}

void pico_layer_pixmap_mode (
    int mode,
    const char* up,
    const char* key,
    Pico_Abs_Dim dim,
    const Pico_Color* pixels
) {
    _pico_guard();
    _pico_layer_pixmap(mode, key, dim, pixels);
    if (up != NULL) {
        _layer_attach(up, key);
    }
}

void pico_layer_empty (
    const char* up, const char* key, int clear,
    Pico_Rel_Dim dim, Pico_Abs_Dim* tile
) {
    _pico_guard();
    pico_layer_empty_mode('!', up, key, clear, dim, tile);
}

void pico_layer_empty_mode (
    int mode, const char* up, const char* key, int clear,
    Pico_Rel_Dim dim, Pico_Abs_Dim* tile
) {
    _pico_guard();
    assert(key!=NULL && "layer key required");
    _alloc_empty_t ctx = { up, clear, dim, tile };
    void* ret = realm_put (
        G.realm, mode, strlen(key)+1, key,
        _free_layer, _alloc_layer_empty, &ctx
    );
    assert(ret != NULL);
    if (up != NULL) {
        _layer_attach(up, key);
    }
}

void pico_layer_image (const char* up, const char* key, const char* path) {
    _pico_guard();
    pico_layer_image_mode('!', up, key, path);
}

void pico_layer_image_mode (
    int mode, const char* up, const char* key, const char* path
) {
    _pico_guard();
    const char* str = (key != NULL) ? key : path;
    _pico_layer_image(mode, key, path);
    if (up != NULL) {
        _layer_attach(up, str);
    }
}

void pico_layer_sub (const char* up, const char* key,
    const char* parent, const Pico_Rel_Rect* crop)
{
    _pico_guard();
    pico_layer_sub_mode('!', up, key, parent, crop);
}

void pico_layer_sub_mode (int mode, const char* up, const char* key,
    const char* parent, const Pico_Rel_Rect* crop)
{
    _pico_guard();
    assert(key!=NULL      && "sub-layer key required");
    assert(parent!=NULL   && "parent key required");
    assert(crop!=NULL     && "crop rect required");

    Pico_Layer* par = (Pico_Layer*)realm_get(
        G.realm, strlen(parent)+1, parent);
    assert(par!=NULL && "parent layer does not exist");
    assert(par->type!=PICO_LAYER_SUB
        && "cannot create sub-layer of sub-layer");

    _alloc_sub_t ctx = { par, *crop };
    void* ret = realm_put (
        G.realm, mode, strlen(key)+1, key,
        _free_layer, _alloc_layer_sub, &ctx
    );
    assert(ret != NULL);
    if (up != NULL) {
        _layer_attach(up, key);
    }
}

void pico_layer_text (
    const char* up, const char* key, int height, const char* text
) {
    _pico_guard();
    pico_layer_text_mode('!', up, key, height, text);
}

void pico_layer_text_mode (
    int mode, const char* up, const char* key, int height, const char* text
) {
    _pico_guard();
    assert(key!=NULL && "layer key required");
    _pico_layer_text(mode, key, height, text);
    if (up != NULL) {
        _layer_attach(up, key);
    }
}

///////////////////////////////////////////////////////////////////////////////
// INPUT
///////////////////////////////////////////////////////////////////////////////

// Handles auto aids: quit/exit, window resize, ctrl+zoom/scroll/grid.
//

static int pico_event_handler (Pico_Event* pico, int do_exit) {
    switch (pico->type) {
        case PICO_EVENT_QUIT: {
            if (!G.expert.on && do_exit) {
                exit(0);
            }
            break;
        }

        case PICO_EVENT_WINDOW_RESIZE: {
            if (G.window.ing.fs) {
                G.window.ing.fs = 0;
            } else {
                Pico_Rel_Dim phy = { '!', {pico->window.w, pico->window.h} };
                const char* old = pico_set_layer("window");
                pico_set_scene_dim(phy);
                pico_set_layer(old);
            }
            break;
        }

        case PICO_EVENT_KEY_DN: {
            if (!pico->keyboard.ctrl) {
                break;
            }
            switch (pico->keyboard.key) {
                case SDLK_0: {
                    return 1;
                }
                case SDLK_MINUS: {
                    assert(G.layer == &G.world);
                    Pico_Rel_Rect pct = {'%', {0}, PICO_ANCHOR_C};
                    _rel_rect (
                        _sdl_rect(G.layer->scene.src, NULL, NULL), &pct, NULL
                    );
                    pct.w += 0.1;
                    pct.h += 0.1;
                    Pico_Rel_Rect r = G.layer->scene.src;
                    _rel_rect (
                        _sdl_rect(pct, NULL, NULL), &r, NULL
                    );
                    pico_set_scene_src(r);
                    return 1;
                }
                case SDLK_EQUALS: {
                    assert(G.layer == &G.world);
                    Pico_Rel_Rect pct = {'%', {0}, PICO_ANCHOR_C};
                    _rel_rect (
                        _sdl_rect(G.layer->scene.src, NULL, NULL), &pct, NULL
                    );
                    pct.w -= 0.1;
                    pct.h -= 0.1;
                    Pico_Rel_Rect r = G.layer->scene.src;
                    _rel_rect (
                        _sdl_rect(pct, NULL, NULL), &r, NULL
                    );
                    pico_set_scene_src(r);
                    return 1;
                }
                case SDLK_LEFT: {
                    assert(G.layer == &G.world);
                    Pico_Rel_Rect pct = {'%', {0}, PICO_ANCHOR_C};
                    _rel_rect (
                        _sdl_rect(G.layer->scene.src, NULL, NULL), &pct, NULL
                    );
                    pct.x -= 0.1;
                    Pico_Rel_Rect r = G.layer->scene.src;
                    _rel_rect (
                        _sdl_rect(pct, NULL, NULL), &r, NULL
                    );
                    pico_set_scene_src(r);
                    return 1;
                }
                case SDLK_RIGHT: {
                    assert(G.layer == &G.world);
                    Pico_Rel_Rect pct = {'%', {0}, PICO_ANCHOR_C};
                    _rel_rect (
                        _sdl_rect(G.layer->scene.src, NULL, NULL), &pct, NULL
                    );
                    pct.x += 0.1;
                    Pico_Rel_Rect r = G.layer->scene.src;
                    _rel_rect (
                        _sdl_rect(pct, NULL, NULL), &r, NULL
                    );
                    pico_set_scene_src(r);
                    return 1;
                }
                case SDLK_UP: {
                    assert(G.layer == &G.world);
                    Pico_Rel_Rect pct = {'%', {0}, PICO_ANCHOR_C};
                    _rel_rect (
                        _sdl_rect(G.layer->scene.src, NULL, NULL), &pct, NULL
                    );
                    pct.y -= 0.1;
                    Pico_Rel_Rect r = G.layer->scene.src;
                    _rel_rect (
                        _sdl_rect(pct, NULL, NULL), &r, NULL
                    );
                    pico_set_scene_src(r);
                    return 1;
                }
                case SDLK_DOWN: {
                    assert(G.layer == &G.world);
                    Pico_Rel_Rect pct = {'%', {0}, PICO_ANCHOR_C};
                    _rel_rect (
                        _sdl_rect(G.layer->scene.src, NULL, NULL), &pct, NULL
                    );
                    pct.y += 0.1;
                    Pico_Rel_Rect r = G.layer->scene.src;
                    _rel_rect (
                        _sdl_rect(pct, NULL, NULL), &r, NULL
                    );
                    pico_set_scene_src(r);
                    return 1;
                }
                case SDLK_g: {
                    assert(G.layer == &G.world);
                    pico_set_effect_grid(!G.world.effect.grid);
                    return 1;
                }
                case SDLK_s: {
                    pico_output_screenshot("window", NULL, NULL);
                    return 1;
                }
            }
            break;
        }
        default:
            break;
    }
    return 0;
}

static void sdl_to_pico (SDL_Event* sdl, Pico_Event* pico) {
    assert(sdl!=NULL && pico!=NULL && "bug found");

    PICO_EVENT _enum (SDL_Event* sdl) {
        switch (sdl->type) {
            case SDL_QUIT:
                return PICO_EVENT_QUIT;
            case SDL_KEYDOWN:
                return PICO_EVENT_KEY_DN;
            case SDL_KEYUP:
                return PICO_EVENT_KEY_UP;
            case SDL_MOUSEMOTION:
                return PICO_EVENT_MOUSE_MOTION;
            case SDL_MOUSEBUTTONDOWN:
                return PICO_EVENT_MOUSE_BUTTON_DN;
            case SDL_MOUSEBUTTONUP:
                return PICO_EVENT_MOUSE_BUTTON_UP;
            case SDL_WINDOWEVENT:
                if (sdl->window.event == SDL_WINDOWEVENT_RESIZED) {
                    return PICO_EVENT_WINDOW_RESIZE;
                } else {
                    return PICO_EVENT_NONE;
                }
            default:
                return PICO_EVENT_NONE;
        }
    }

    pico->type = _enum(sdl);
    switch (pico->type) {
        case PICO_EVENT_NONE:
            break;              // TODO: complete with all possible events

        case PICO_EVENT_QUIT:
            break;
        case PICO_EVENT_WINDOW_RESIZE:
            pico->window = (typeof(pico->window)) { sdl->window.data1, sdl->window.data2 };
            break;

        case PICO_EVENT_KEY_DN:
        case PICO_EVENT_KEY_UP:
            pico->keyboard = _pico_keyboard(sdl->key.keysym.sym, sdl->key.keysym.mod);
            break;

        case PICO_EVENT_MOUSE_MOTION:
        case PICO_EVENT_MOUSE_BUTTON_DN:
        case PICO_EVENT_MOUSE_BUTTON_UP: {
            // report mouse pos in window pixels regardless of current layer
            Pico_Rel_Pos tmpl = { .mode='!', .anchor=PICO_ANCHOR_C };
            pico->mouse = pico_get_mouse("window", &tmpl);
            break;
        }

        default:
            printf(">>> %d\n", pico->type);
            assert(0 && "bug found");
            break;
    }
}

int pico_input_event_timeout (Pico_Event* evt, int type, int timeout) {
    _pico_guard();
    int t0 = SDL_GetTicks();
    while (1) {
        Pico_Event out;
        SDL_Event sdl;
        int has = (timeout == -1) ? SDL_WaitEvent(&sdl) : SDL_WaitEventTimeout(&sdl, timeout);
        int t1 = SDL_GetTicks();
        int dt = t1 - t0;
        if (!has && timeout!=-1) {
            if (evt != NULL) {
                evt->type = PICO_EVENT_NONE;
            }
            return dt;
        }

        sdl_to_pico(&sdl, &out);
        if (pico_event_handler(&out, 1)) {
            // continue
        } else if (out.type == PICO_EVENT_NONE) {
            // continue
        } else if (type==PICO_EVENT_ANY || out.type==type) {
            if (evt != NULL) {
                *evt = out;
            }
            return dt;
        } else {
            // continue
        }

        if (timeout != -1) {
            timeout = MAX(0, timeout-dt);
            t0 = t1;
        }
    }
}

int pico_input_event (Pico_Event* evt, int type) {
    _pico_guard();
    if (G.expert.ms==0 || G.expert.ms==-1) {
        return pico_input_event_timeout(evt, type, G.expert.ms);
    }

    int now = SDL_GetTicks();
    int cur = (G.expert.t0 + G.expert.ms) - now;
    if (cur <= 0) {
        while (G.expert.t0+G.expert.ms <= now) {
            G.expert.t0 += G.expert.ms;
        }
        cur = 0;
    }
    Pico_Event xevt;
    if (evt == NULL) {
        evt = &xevt;
    }
    int dt = pico_input_event_timeout(evt, type, cur);
    if (evt->type == PICO_EVENT_NONE) {
        G.expert.t0 += G.expert.ms;
    }
    return dt;
}

int pico_input_delay (int ms) {
    _pico_guard();
    return pico_input_event_timeout(NULL, PICO_EVENT_NONE, ms);
}

void pico_input_loop (void) {
    _pico_guard();
    pico_input_event(NULL, PICO_EVENT_QUIT);
}

///////////////////////////////////////////////////////////////////////////////
// OUTPUT
///////////////////////////////////////////////////////////////////////////////

void pico_output_clear (void) {
    _pico_guard();
    SDL_SetRenderDrawColor(G.window.ren,
        G.layer->effect.color.r, G.layer->effect.color.g, G.layer->effect.color.b, G.layer->effect.color.a
    );
    Pico_Abs_Rect r = _rnd_rect(_sdl_rect(G.layer->scene.clip, NULL, NULL));
    SDL_RenderFillRect(G.window.ren, &r);
    _pico_output_present(0);
}

void pico_output_draw_image (const char* path, Pico_Rel_Rect rect) {
    _pico_guard();
    Pico_Layer* layer = _pico_layer_image('=', NULL, path);
    Pico_Abs_Dim* orig = (rect.w==0 || rect.h==0) ? &layer->scene.dim : NULL;
    Pico_Rel_Dim rel = { rect.mode, {rect.w, rect.h} };
    _sdl_dim(&rel, NULL, orig);
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
    Pico_Abs_Pos i1 = _rnd_pos(_sdl_pos(p1, NULL));
    Pico_Abs_Pos i2 = _rnd_pos(_sdl_pos(p2, NULL));
    SDL_RenderDrawLine(G.window.ren, i1.x,i1.y, i2.x,i2.y);
    _pico_output_present(0);
}

void pico_output_draw_oval (Pico_Rel_Rect rect) {
    _pico_guard();
    Pico_Abs_Rect i = _rnd_rect(_sdl_rect(rect, NULL, NULL));
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
    Pico_Abs_Pos i = _rnd_pos(_sdl_pos(pos, NULL));
    SDL_RenderDrawPoint(G.window.ren, i.x, i.y);
        // TODO: could use PointF, but 4.5->4 (not 5 desired)
    _pico_output_present(0);
}

void pico_output_draw_pixels (int n, const Pico_Rel_Pos* ps) {
    _pico_guard();
    Pico_Abs_Pos vs[n];
    for (int i=0; i<n; i++) {
        vs[i] = _rnd_pos(_sdl_pos(ps[i], NULL));
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
        Pico_Abs_Pos v = _rnd_pos(_sdl_pos(ps[i], NULL));
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

    Pico_Abs_Rect i = _rnd_rect(_sdl_rect(rect, NULL, NULL));
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

    SDL_FDim dim = _sdl_dim (
        &(Pico_Rel_Dim){ rect.mode, {0, rect.h} }, NULL, NULL
    );

    Pico_Layer* layer = _pico_layer_text(mode, key, dim.h, text);
    Pico_Abs_Dim* orig = (rect.w == 0) ? &layer->scene.dim : NULL;

    Pico_Rel_Dim rel = { rect.mode, {rect.w, rect.h} };
    _sdl_dim(&rel, NULL, orig);
    rect.w = rel.w;

    _pico_output_draw_layer(layer, &rect);
}

void pico_output_draw_tri (
    Pico_Rel_Pos p1, Pico_Rel_Pos p2, Pico_Rel_Pos p3
) {
    _pico_guard();
    Pico_Abs_Pos i1 = _rnd_pos(_sdl_pos(p1, NULL));
    Pico_Abs_Pos i2 = _rnd_pos(_sdl_pos(p2, NULL));
    Pico_Abs_Pos i3 = _rnd_pos(_sdl_pos(p3, NULL));

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

static void _pico_output_present (int force) {
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
    Pico_Abs_Rect r = _rnd_rect(_sdl_rect(G.layer->scene.clip, NULL, NULL));
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

static void _pico_output_sound_cache (const char* path, int cache) {
    Mix_Chunk* mix = NULL;

    if (cache) {
        int n = strlen(path) + 1;
        mix = (Mix_Chunk*)realm_put(
            G.realm, '=', n, path,
            _free_sound, _alloc_sound, (void*)path
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
        : _rnd_rect(_sdl_rect(*rect, NULL, NULL));

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
    _pico_output_sound_cache(path, 1);
}

#define PICO_VIDEO_C
#include "video.hc"
