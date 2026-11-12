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

#include "tiny_ttf.h"
#include "pico.h"
#include "_pico.h"

const Pico_Anchor PICO_ANCHOR_X  = { .x=-1, .y=-1 };
const Pico_Anchor PICO_ANCHOR_C  = { .x=PICO_ANCHOR_CENTER, .y=PICO_ANCHOR_MIDDLE };
const Pico_Anchor PICO_ANCHOR_NW = { .x=PICO_ANCHOR_LEFT,   .y=PICO_ANCHOR_TOP    };
const Pico_Anchor PICO_ANCHOR_N  = { .x=PICO_ANCHOR_CENTER, .y=PICO_ANCHOR_TOP    };
const Pico_Anchor PICO_ANCHOR_NE = { .x=PICO_ANCHOR_RIGHT,  .y=PICO_ANCHOR_TOP    };
const Pico_Anchor PICO_ANCHOR_E  = { .x=PICO_ANCHOR_RIGHT,  .y=PICO_ANCHOR_MIDDLE };
const Pico_Anchor PICO_ANCHOR_SE = { .x=PICO_ANCHOR_RIGHT,  .y=PICO_ANCHOR_BOTTOM };
const Pico_Anchor PICO_ANCHOR_S  = { .x=PICO_ANCHOR_CENTER, .y=PICO_ANCHOR_BOTTOM };
const Pico_Anchor PICO_ANCHOR_SW = { .x=PICO_ANCHOR_LEFT,   .y=PICO_ANCHOR_BOTTOM };
const Pico_Anchor PICO_ANCHOR_W  = { .x=PICO_ANCHOR_LEFT,   .y=PICO_ANCHOR_MIDDLE };

///////////////////////////////////////////////////////////////////////////////
// DATA
///////////////////////////////////////////////////////////////////////////////


PicoState G = { 0 };

///////////////////////////////////////////////////////////////////////////////
// AUX
///////////////////////////////////////////////////////////////////////////////

void _pico_guard (void) {
    if (!G.flags.init) {
        fprintf(stderr, "ERROR : pico-sdl is not initialized\n");
        abort();
    }
}

SDL_Texture* _pico_tex_create (Pico_Abs_Dim dim) {
    SDL_Texture* tex = SDL_CreateTexture (
        G.window.ren, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
        dim.w, dim.h
    );

    SDL_BlendMode mode;
    if (!G.flags.init) {
        mode = SDL_BLENDMODE_NONE;
    } else if (G.layer == &G.window.layer) {
        mode = SDL_BLENDMODE_NONE;
    } else if (G.layer == &G.world) {
        mode = SDL_BLENDMODE_NONE;
    } else {
        mode = SDL_BLENDMODE_BLEND;
    }
    pico_assert_0 (
        SDL_SetTextureBlendMode(tex, mode)
    );

    return tex;
}

///////////////////////////////////////////////////////////////////////////////
// INIT
///////////////////////////////////////////////////////////////////////////////

void pico_open (const Pico_Window* cfg) {
    {
        assert(G.flags.init == 0);

        Pico_Window xcfg = { .fs=0, .show=1, .title=PICO_TITLE };
        if (cfg != NULL) {
            xcfg = *cfg;
            if (xcfg.title == NULL) {
                xcfg.title = PICO_TITLE;
            }
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
                xcfg.title,
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                PICO_DIM_PHY.w, PICO_DIM_PHY.h,
                xcfg.show ? SDL_WINDOW_SHOWN : SDL_WINDOW_HIDDEN
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
            log = _pico_tex_create(PICO_DIM_LOG);
        }

        G = (typeof(G)) {
            .flags = { .init=1, .aids=1 },
            .world = {
                .type = PICO_LAYER_PLAIN,
                .tex  = log,
                .pencil = {
                    .color = PICO_COLOR_WHITE,
                    .font  = NULL,
                    .style = PICO_STYLE_FILL,
                },
                .effect = {
                    .alpha  = 0xFF,
                    .color  = PICO_COLOR_BLACK,
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
            .expert = {0, -1, 0},
            .window = {
                .win = win,
                .ren = ren,
                .layer = {
                    .type = PICO_LAYER_PLAIN,
                    .tex  = phy,
                    .pencil = {
                        .color = PICO_COLOR_WHITE,
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

        _pico_layer_attach(&G.window.layer, &G.world);
        _pico_set_layer(&G.world);
        pico_output_clear();

        // prevents WMs to resize window at start
        SDL_PumpEvents();
        SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
        SDL_SetWindowResizable(G.window.win, 1);

        if (xcfg.fs) {
            pico_set_window_fs(1);
        }
    }
}

// strict: assert that every user-owned resource was freed. The
// auto-exit aid (window close) passes 0 and has them released here,
// since the program gets no chance to free its resources before the
// process exits.
void _pico_close (int strict) {
    assert(G.flags.init == 1);
    if (strict) {
        assert(G.live==0 &&
            "pico_close: layers, fonts or sounds still alive");
    } else {
        _pico_mem_free_all();
    }

    // easy-path resources, then the built-in font sizes
    _pico_cache_close();
    _pico_mem_font_done(&G.font);

    G.flags.init = 0;

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

void pico_close (void) {
    _pico_guard();
    _pico_close(1);
}

void pico_quit (void) {
    _pico_guard();
    SDL_Event e = { .type = SDL_QUIT };
    SDL_PushEvent(&e);
}
