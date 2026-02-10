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

#define TTL_HASH_C
#include "hash.h"
#include "tiny_ttf.h"
#include "pico.h"

#define PICO_COLORS_C
#include "colors.h"

#define PICO_ANCHORS_C
#include "anchors.h"

///////////////////////////////////////////////////////////////////////////////
// DATA
///////////////////////////////////////////////////////////////////////////////

typedef enum {
    PICO_KEY_SOUND,
    PICO_KEY_LAYER,
} PICO_KEY_TYPE;

typedef struct {
    PICO_KEY_TYPE type;
    char key[];
} Pico_Key;

typedef struct {
    Pico_Abs_Dim  dim;
    Pico_Rel_Rect dst;
    int           grid;
    Pico_Rel_Rect src;
    Pico_Rel_Rect clip;
    Pico_Abs_Dim  tile;
    Pico_Rot      rot;
    PICO_FLIP     flip;
} Pico_View;

typedef struct {
    const Pico_Key* key;   // NULL for main layer
    SDL_Texture*    tex;
    Pico_View       view;
} Pico_Layer;

#define SDL_ANY PICO_EVENT_ANY
#define MAX(x,y) ((x) > (y) ? (x) : (y))

#ifdef PICO_TESTS
SDL_Window* pico_win;
#endif

static struct { // internal global state
    int           init;
    int           fsing;
    ttl_hash*     hash;
    Pico_Layer    main;
    SDL_Renderer* ren;
    int           tgt;
    SDL_Window*   win;
} G = { 0 };

static struct { // exposed global state
    int alpha;
    struct {
        Pico_Color clear;
        Pico_Color draw;
    } color;
    int expert;
    const char* font;
    Pico_Layer* layer;
    PICO_STYLE style;
    struct {
        Pico_Abs_Dim dim;
        int          fs;
    } win;
} S;

#define PICO_STACK_MAX 16

typedef struct {
    int           alpha;
    struct {
        Pico_Color clear;
        Pico_Color draw;
    } color;
    const char*   font;
    PICO_STYLE    style;
    Pico_Layer*   layer;
} Pico_State;

static struct {
    Pico_State buf[PICO_STACK_MAX];
    int        n;
} STACK = {
    {}, 0
};

///////////////////////////////////////////////////////////////////////////////
// AUX
///////////////////////////////////////////////////////////////////////////////

static Pico_Layer* _pico_layer_image (const char* name, const char* path);
static Pico_Layer* _pico_layer_text (const char* name, int height, const char* text);
static void _pico_output_draw_layer (Pico_Layer* layer, Pico_Rel_Rect* rect);
static void _pico_output_present (int force);


static SDL_Texture* _tex_create (Pico_Abs_Dim dim) {
    SDL_Texture* tex = SDL_CreateTexture (
        G.ren, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
        dim.w, dim.h
    );
    pico_assert(tex != NULL);
    return tex;
}

static TTF_Font* _font_open (const char* path, int h) {
    TTF_Font* ttf;
    if (path == NULL) {
        SDL_RWops* rw = SDL_RWFromConstMem(pico_tiny_ttf, pico_tiny_ttf_len);
        ttf = TTF_OpenFontRW(rw, 1, h);
    } else {
        ttf = TTF_OpenFont(path, h);
    }
    pico_assert(ttf != NULL);
    return ttf;
}

static SDL_Texture* _tex_text (int height, const char* text, Pico_Abs_Dim* dim) {
    SDL_Color c = { S.color.draw.r, S.color.draw.g, S.color.draw.b, 0xFF };
    TTF_Font* ttf = _font_open(S.font, height);
    SDL_Surface* sfc = TTF_RenderText_Solid(ttf, text, c);
    pico_assert(sfc != NULL);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(G.ren, sfc);
    pico_assert(tex != NULL);
    *dim = (Pico_Abs_Dim){ sfc->w, sfc->h };
    SDL_FreeSurface(sfc);
    TTF_CloseFont(ttf);
    return tex;
}

static SDL_FDim _f3 (float w, float h, const Pico_Abs_Dim* ratio) {
    if (ratio!=NULL && (w==0 || h==0)) {
        if (w == 0 && h == 0) {
            w = ratio->w;
            h = ratio->h;
        } else if (w == 0) {
            w = h * ratio->w / ratio->h;
        } else {
            h = w * ratio->h / ratio->w;
        }
    }
    return (SDL_FDim) { w, h };
}

static SDL_FRect _f2 (
    SDL_FRect   dn,
    Pico_Anchor anc,
    SDL_FRect   up,
    const Pico_Abs_Dim* ratio
) {
    SDL_FDim d = _f3(dn.w*up.w, dn.h*up.h, ratio);
    return (SDL_FRect) {
        up.x + dn.x*up.w - anc.x*d.w,
        up.y + dn.y*up.h - anc.y*d.h,
        d.w, d.h,
    };
}

static SDL_FRect _f1 (
    const Pico_Rel_Rect* r,
    SDL_FRect base,
    const Pico_Abs_Dim* ratio
) {
    if (r == NULL) {
        return base;
    } else {
        SDL_FRect abs = { r->x, r->y, r->w, r->h };
        SDL_FRect tmp = _f1(r->up, base, NULL);
        return _f2(abs, r->anchor, tmp, ratio);
    }
}

static SDL_FDim _sdl_dim (
    Pico_Rel_Dim*        dim,
    const Pico_Abs_Rect* base,
    const Pico_Abs_Dim*  ratio
) {
    SDL_FRect r0;
    if (base == NULL) {
        r0 = (SDL_FRect) {
            0, 0,
            (G.tgt == 0) ? S.win.dim.w : S.layer->view.dim.w,
            (G.tgt == 0) ? S.win.dim.h : S.layer->view.dim.h,
        };
    } else {
        r0 = (SDL_FRect) { base->x, base->y, base->w, base->h };
    }
    SDL_FRect r1 = _f1(dim->up, r0, NULL);
    SDL_FDim ret;
    switch (dim->mode) {
        case '!':
            ret = _f3(dim->w, dim->h, ratio);
            if (dim->w == 0) dim->w = ret.w;
            if (dim->h == 0) dim->h = ret.h;
            break;
        case '%':
            ret = _f3(dim->w * r1.w, dim->h * r1.h, ratio);
            if (dim->w == 0) dim->w = ret.w / r1.w;
            if (dim->h == 0) dim->h = ret.h / r1.h;
            break;
        case '#':
            ret = _f3(dim->w * S.layer->view.tile.w, dim->h * S.layer->view.tile.h, ratio);
            if (dim->w == 0) dim->w = ret.w / S.layer->view.tile.w;
            if (dim->h == 0) dim->h = ret.h / S.layer->view.tile.h;
            break;
        default:
            assert(0 && "invalid mode");
    }
    return ret;
}

static SDL_FPoint _sdl_pos (
    const Pico_Rel_Pos*  pos,
    const Pico_Abs_Rect* base
) {
    SDL_FPoint ret;
    SDL_FRect r0;
    if (base == NULL) {
        r0 = (SDL_FRect) {
            0, 0,
            (G.tgt == 0) ? S.win.dim.w : S.layer->view.dim.w,
            (G.tgt == 0) ? S.win.dim.h : S.layer->view.dim.h,
        };
    } else {
        r0 = (SDL_FRect) { base->x, base->y, base->w, base->h };
    }
    SDL_FRect r1 = _f1(pos->up, r0, NULL);
    switch (pos->mode) {
        case '!':
            ret = (SDL_FPoint) {
                r1.x + pos->x - pos->anchor.x,
                r1.y + pos->y - pos->anchor.y,
            };
            break;
        case '%':
            ret = (SDL_FPoint) {
                r1.x + pos->x*r1.w - pos->anchor.x,
                r1.y + pos->y*r1.h - pos->anchor.y,
            };
            break;
        case '#':
            ret = (SDL_FPoint) {
                r1.x + (pos->x - 1 + pos->anchor.x)*S.layer->view.tile.w,
                r1.y + (pos->y - 1 + pos->anchor.y)*S.layer->view.tile.h,
            };
            break;
        default:
            assert(0 && "invalid mode");
    }
    return ret;
}

static SDL_FRect _sdl_rect (
    const Pico_Rel_Rect* rect,
    const Pico_Abs_Rect* base,
    const Pico_Abs_Dim* ratio
) {
    SDL_FRect ret;
    SDL_FRect r0;
    if (base == NULL) {
        r0 = (SDL_FRect) {
            0, 0,
            (G.tgt == 0) ? S.win.dim.w : S.layer->view.dim.w,
            (G.tgt == 0) ? S.win.dim.h : S.layer->view.dim.h,
        };
    } else {
        r0 = (SDL_FRect) { base->x, base->y, base->w, base->h };
    }
    SDL_FRect r1 = _f1(rect->up, r0, NULL);
    switch (rect->mode) {
        case '!': {
            SDL_FDim d = _f3(rect->w, rect->h, ratio);
            ret = (SDL_FRect) {
                r1.x + rect->x - rect->anchor.x*d.w,
                r1.y + rect->y - rect->anchor.y*d.h,
                d.w,
                d.h
            };
            break;
        }
        case '%':
            ret = _f1(rect, r0, ratio);
            break;
        case '#': {
            SDL_FDim d = _f3 (
                rect->w * S.layer->view.tile.w,
                rect->h * S.layer->view.tile.h,
                ratio
            );
            ret = (SDL_FRect) {
                r1.x + (rect->x - 1 + rect->anchor.x)*S.layer->view.tile.w - rect->anchor.x*d.w,
                r1.y + (rect->y - 1 + rect->anchor.y)*S.layer->view.tile.h - rect->anchor.y*d.h,
                d.w,
                d.h
            };
            break;
        }
        default:
            assert(0 && "invalid mode");
    }
    return ret;
}

static Pico_Abs_Dim _fi_dim (const SDL_FDim* f) {
    return (Pico_Abs_Dim) { roundf(f->w), roundf(f->h) };
}

static SDL_Point _fi_pos (const SDL_FPoint* f) {
    return (SDL_Point) { roundf(f->x), roundf(f->y) };
}

static SDL_Rect _fi_rect (const SDL_FRect* f) {
    return (SDL_Rect) { roundf(f->x), roundf(f->y), roundf(f->w), roundf(f->h) };
}

///////////////////////////////////////////////////////////////////////////////
// CV
///////////////////////////////////////////////////////////////////////////////

Pico_Abs_Dim pico_cv_dim_rel_abs (Pico_Rel_Dim* dim, Pico_Abs_Rect* base) {
    SDL_FDim df = _sdl_dim(dim, base, NULL);
    return _fi_dim(&df);
}

Pico_Abs_Pos pico_cv_pos_rel_abs (const Pico_Rel_Pos* pos, Pico_Abs_Rect* base) {
    SDL_FPoint pf = _sdl_pos(pos, base);
    return (Pico_Abs_Pos) _fi_pos(&pf);
}

Pico_Abs_Rect pico_cv_rect_rel_abs (const Pico_Rel_Rect* rect, Pico_Abs_Rect* base) {
    SDL_FRect rf = _sdl_rect(rect, base, NULL);
    return (Pico_Abs_Rect) _fi_rect(&rf);
}

void pico_cv_pos_abs_rel (const Pico_Abs_Pos* fr,
                          Pico_Rel_Pos* to,
                          Pico_Abs_Rect* base)
{
    SDL_FRect r0;
    if (base == NULL) {
        r0 = (SDL_FRect) {
            0, 0,
            (G.tgt == 0) ? S.win.dim.w : S.layer->view.dim.w,
            (G.tgt == 0) ? S.win.dim.h : S.layer->view.dim.h,
        };
    } else {
        r0 = (SDL_FRect) { base->x, base->y, base->w, base->h };
    }

    Pico_Rel_Rect* up_rect = (Pico_Rel_Rect*) to->up;
    SDL_FRect r1 = _f1(up_rect, r0, NULL);

    switch (to->mode) {
        case '!':
            to->x = fr->x - r1.x + to->anchor.x;
            to->y = fr->y - r1.y + to->anchor.y;
            break;
        case '%':
            to->x = (fr->x - r1.x + to->anchor.x) / r1.w;
            to->y = (fr->y - r1.y + to->anchor.y) / r1.h;
            break;
        case '#':
            to->x = (fr->x - r1.x) / S.layer->view.tile.w + 1 - to->anchor.x;
            to->y = (fr->y - r1.y) / S.layer->view.tile.h + 1 - to->anchor.y;
            break;
        default:
            assert(0 && "invalid mode");
    }
}

void pico_cv_pos_rel_rel (const Pico_Rel_Pos* fr,
                          Pico_Rel_Pos* to,
                          Pico_Abs_Rect* base)
{
    Pico_Abs_Pos abs = pico_cv_pos_rel_abs(fr, base);
    pico_cv_pos_abs_rel(&abs, to, base);
}

void pico_cv_rect_abs_rel (const Pico_Abs_Rect* fr,
                           Pico_Rel_Rect* to,
                           Pico_Abs_Rect* base)
{
    SDL_FRect r0;
    if (base == NULL) {
        r0 = (SDL_FRect) {
            0, 0,
            (G.tgt == 0) ? S.win.dim.w : S.layer->view.dim.w,
            (G.tgt == 0) ? S.win.dim.h : S.layer->view.dim.h,
        };
    } else {
        r0 = (SDL_FRect) { base->x, base->y, base->w, base->h };
    }

    SDL_FRect r1 = _f1(to->up, r0, NULL);

    switch (to->mode) {
        case '!':
            to->w = fr->w;
            to->h = fr->h;
            to->x = fr->x - r1.x + to->anchor.x * to->w;
            to->y = fr->y - r1.y + to->anchor.y * to->h;
            break;
        case '%':
            to->w = fr->w / r1.w;
            to->h = fr->h / r1.h;
            to->x = (fr->x - r1.x + to->anchor.x * fr->w) / r1.w;
            to->y = (fr->y - r1.y + to->anchor.y * fr->h) / r1.h;
            break;
        case '#':
            to->w = fr->w / (float)S.layer->view.tile.w;
            to->h = fr->h / (float)S.layer->view.tile.h;
            to->x = (fr->x - r1.x) / S.layer->view.tile.w + 1
                    - to->anchor.x + to->anchor.x * to->w;
            to->y = (fr->y - r1.y) / S.layer->view.tile.h + 1
                    - to->anchor.y + to->anchor.y * to->h;
            break;
        default:
            assert(0 && "invalid mode");
    }
}

void pico_cv_rect_rel_rel (const Pico_Rel_Rect* fr,
                           Pico_Rel_Rect* to,
                           Pico_Abs_Rect* base)
{
    Pico_Abs_Rect abs = pico_cv_rect_rel_abs(fr, base);
    pico_cv_rect_abs_rel(&abs, to, base);
}

///////////////////////////////////////////////////////////////////////////////
// VS
///////////////////////////////////////////////////////////////////////////////

int pico_vs_pos_rect (Pico_Rel_Pos* pos, Pico_Rel_Rect* rect) {
    Pico_Abs_Pos  pi = pico_cv_pos_rel_abs(pos, NULL);
    Pico_Abs_Rect ri = pico_cv_rect_rel_abs(rect, NULL);
    return SDL_PointInRect(&pi, &ri);
}

int pico_vs_rect_rect (Pico_Rel_Rect* r1, Pico_Rel_Rect* r2) {
    Pico_Abs_Rect i1 = pico_cv_rect_rel_abs(r1, NULL);
    Pico_Abs_Rect i2 = pico_cv_rect_rel_abs(r2, NULL);
    return SDL_HasIntersection(&i1, &i2);
}

///////////////////////////////////////////////////////////////////////////////
// COLOR
///////////////////////////////////////////////////////////////////////////////

Pico_Color pico_color_darker (Pico_Color clr, float pct) {
    if (pct < 0) {
        return pico_color_lighter(clr, -pct);
    } else {
        float X = MAX(0, 1-pct);
        return (Pico_Color) { clr.r*X, clr.g*X, clr.b*X };
    }
}

Pico_Color pico_color_lighter (Pico_Color clr, float pct) {
    if (pct < 0) {
        return pico_color_darker(clr, -pct);
    }
    return (Pico_Color) {
        (clr.r + (255 - clr.r) * pct),
        (clr.g + (255 - clr.g) * pct),
        (clr.b + (255 - clr.b) * pct)
    };
}

Pico_Color pico_color_mix (Pico_Color c1, Pico_Color c2) {
    return (Pico_Color) {
        (c1.r + c2.r) / 2,
        (c1.g + c2.g) / 2,
        (c1.b + c2.b) / 2
    };
}

Pico_Color_A pico_color_alpha (Pico_Color clr, Uint8 a) {
    return (Pico_Color_A) { clr.r, clr.g, clr.b, a };
}

///////////////////////////////////////////////////////////////////////////////
// INIT
///////////////////////////////////////////////////////////////////////////////

static void _pico_hash_clean (int n, const void* key, void* value) {
    const Pico_Key* res = (const Pico_Key*)key;
    switch (res->type) {
        case PICO_KEY_LAYER: {
            Pico_Layer* data = (Pico_Layer*)value;
            SDL_DestroyTexture(data->tex);
            free(data);
            break;
        }
        case PICO_KEY_SOUND:
            Mix_FreeChunk((Mix_Chunk*)value);
            break;
        default:
            assert(0 && "invalid resource");
    }
}

void pico_init (int on) {
    if (on) {
        assert(G.init == 0);
        pico_assert(0 == SDL_Init(SDL_INIT_EVERYTHING));
        TTF_Init();
        Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 1024);

        G = (typeof(G)) {
            .init  = 0,
            .fsing = 0,
            .hash  = ttl_hash_open(PICO_HASH_BUK, PICO_HASH_TTL, _pico_hash_clean),
            .main  = {
                .key  = NULL,
                .tex  = NULL,   // needs G.ren
                .view = {
                    .grid = 1,
                    .dim  = PICO_DIM_LOG,
                    .dst  = {'%', {.5,.5,1,1}, PICO_ANCHOR_C, NULL},
                    .src  = {'%', {.5,.5,1,1}, PICO_ANCHOR_C, NULL},
                    .clip = {'%', {.5,.5,1,1}, PICO_ANCHOR_C, NULL},
                    .tile = {0, 0},
                    .rot  = {0, PICO_ANCHOR_C},
                    .flip = PICO_FLIP_NONE,
                },
            },
            .ren = NULL,        // needs G.win
            .tgt = 1,
            .win = SDL_CreateWindow (
                       PICO_TITLE,
                       SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                       PICO_DIM_PHY.w, PICO_DIM_PHY.h,
                       (SDL_WINDOW_SHOWN /*| SDL_WINDOW_RESIZABLE*/)
                   ),
        };
        assert(G.hash != NULL);
        pico_assert(G.win != NULL);
        SDL_SetWindowResizable(G.win, 1);

        S = (typeof(S)) {
            .alpha  = 0xFF,
            .color  = { PICO_COLOR_BLACK, PICO_COLOR_WHITE },
            .expert = 0,
            .font   = NULL,
            .layer  = &G.main,
            .style  = PICO_STYLE_FILL,
            .win    = {
                .dim = PICO_DIM_PHY,
                .fs  = 0,
            },
        };

        STACK.n = 0;

        // create ren after win
        {
#ifdef PICO_TESTS
            pico_win = G.win;
            G.ren = SDL_CreateRenderer(G.win, -1, SDL_RENDERER_SOFTWARE);
#else
            G.ren = SDL_CreateRenderer(G.win, -1, SDL_RENDERER_ACCELERATED/*|SDL_RENDERER_PRESENTVSYNC*/);
#endif
            pico_assert(G.ren != NULL);
            SDL_SetRenderDrawBlendMode(G.ren, SDL_BLENDMODE_BLEND);
        }

        // create tex after ren
        {
            G.main.tex = _tex_create(PICO_DIM_LOG);
            SDL_SetTextureBlendMode(G.main.tex, SDL_BLENDMODE_NONE);
            SDL_SetRenderTarget(G.ren, G.main.tex);
            SDL_Rect r = pico_cv_rect_rel_abs (
                &G.main.view.clip, NULL
            );
            SDL_RenderSetClipRect(G.ren, &r);
            pico_output_clear();
        }

        SDL_PumpEvents();
        SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
        G.init = 1;
    }
    else {
        if (!G.init) {
            return;
        }
        G.init = 0;

        if (G.hash != NULL) {
            ttl_hash_close(G.hash);
        }
        Mix_CloseAudio();
        TTF_Quit();
        if (G.main.tex != NULL) {
            SDL_DestroyTexture(G.main.tex);
        }
        if (G.ren != NULL) {
            SDL_DestroyRenderer(G.ren);
        }
        if (G.win != NULL) {
            SDL_DestroyWindow(G.win);
        }
        SDL_Quit();
    }
}

void pico_quit (void) {
    SDL_Event e = { .type = SDL_QUIT };
    SDL_PushEvent(&e);
}

///////////////////////////////////////////////////////////////////////////////
// GET
///////////////////////////////////////////////////////////////////////////////

Pico_Color pico_get_color_clear (void) {
    return S.color.clear;
}

Pico_Color pico_get_color_draw (void) {
    return S.color.draw;
}

int pico_get_expert (void) {
    return S.expert;
}

const char* pico_get_font (void) {
    return S.font;
}

Pico_Abs_Dim pico_get_image (const char* path, Pico_Rel_Dim* rel) {
    if (rel == NULL) {
        Pico_Layer* layer = _pico_layer_image(NULL, path);
        return layer->view.dim;
    } else if (rel->w==0 || rel->h==0) {
        Pico_Layer* layer = _pico_layer_image(NULL, path);
        SDL_FDim fd = _sdl_dim(rel, NULL, &layer->view.dim);
        return (Pico_Abs_Dim){fd.w, fd.h};
    } else {
        SDL_FDim fd = _sdl_dim(rel, NULL, NULL);
        return (Pico_Abs_Dim){fd.w, fd.h};
    }
}

int pico_get_key (PICO_KEY key) {
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    return keys[key];
}

const char* pico_get_layer (void) {
    return S.layer->key ? S.layer->key->key : NULL;
}

int pico_get_mouse (Pico_Rel_Pos* pos, int button) {
    SDL_Point phy;
    Uint32 masks = SDL_GetMouseState(&phy.x, &phy.y);
    if (button == 0) {
        masks = 0;
    }

    // rel view.dst/src -> abs dst/src
    SDL_Rect dst = pico_cv_rect_rel_abs (
        &S.layer->view.dst,
        &(Pico_Abs_Rect){0, 0, S.win.dim.w, S.win.dim.h}
    );
    SDL_Rect src = pico_cv_rect_rel_abs (
        &S.layer->view.src, NULL
    );

    // 1. Get position relative to dst (normalized 0-1)
    SDL_FPoint rel = {
        (phy.x - dst.x) / (float)dst.w,
        (phy.y - dst.y) / (float)dst.h,
    };

    // 2. Convert to logical position within src (zoom/scroll viewport)
    SDL_FPoint log = {
        src.x + rel.x*src.w,
        src.y + rel.y*src.h,
    };

    switch (pos->mode) {
        case '!':
            pos->x = log.x;
            pos->y = log.y;
            break;
        case '%': {
            Pico_Rel_Rect up;
            if (pos->up == NULL) {
                up = (Pico_Rel_Rect){ '!', {0, 0, S.layer->view.dim.w, S.layer->view.dim.h}, PICO_ANCHOR_NW, NULL };
            } else {
                assert(0 && "TODO");
            }
            pos->x = (log.x - up.x) / up.w;
            pos->y = (log.y - up.y) / up.h;
            break;
        }
        case '#':
            pos->x = (log.x / (float)S.layer->view.tile.w) + (1 - pos->anchor.x);
            pos->y = (log.y / (float)S.layer->view.tile.h) + (1 - pos->anchor.y);
            break;
        default:
            assert(0 && "invalid mode");
    }

    return masks & SDL_BUTTON(button);
}


int pico_get_show (void) {
    return SDL_GetWindowFlags(G.win) & SDL_WINDOW_SHOWN;
}

PICO_STYLE pico_get_style (void) {
    return S.style;
}

Pico_Abs_Dim pico_get_text (const char* text, Pico_Rel_Dim* rel) {
    assert(text[0] != '\0');
    assert(rel!=NULL && rel->h!=0);
    if (rel->w == 0) {
        Pico_Abs_Dim orig;
        SDL_Texture* tex = _tex_text(10, text, &orig);
        SDL_DestroyTexture(tex);
        SDL_FDim fd = _sdl_dim(rel, NULL, &orig);
        return (Pico_Abs_Dim){fd.w, fd.h};
    } else {
        SDL_FDim fd = _sdl_dim(rel, NULL, NULL);
        return (Pico_Abs_Dim){fd.w, fd.h};
    }
}

Uint32 pico_get_ticks (void) {
    return SDL_GetTicks();
}

void pico_get_view (
    int* grid,
    Pico_Abs_Dim*  dim,
    Pico_Abs_Dim*  tile,
    Pico_Rel_Rect* dst,
    Pico_Rel_Rect* src,
    Pico_Rel_Rect* clip,
    Pico_Rot*      rot,
    PICO_FLIP*     flip
) {
    if (grid != NULL) {
        *grid = S.layer->view.grid;
    }
    if (dim != NULL) {
        *dim = S.layer->view.dim;
    }
    if (dst != NULL) {
        *dst = S.layer->view.dst;
    }
    if (src != NULL) {
        *src = S.layer->view.src;
    }
    if (clip != NULL) {
        *clip = S.layer->view.clip;
    }
    if (tile != NULL) {
        *tile = S.layer->view.tile;
    }
    if (rot != NULL) {
        *rot = S.layer->view.rot;
    }
    if (flip != NULL) {
        *flip = S.layer->view.flip;
    }
}

void pico_get_window (const char** title, int* fs, Pico_Abs_Dim* dim) {
    if (title != NULL) {
        *title = SDL_GetWindowTitle(G.win);
    }
    if (fs != NULL) {
        *fs = S.win.fs;
    }
    if (dim != NULL) {
        *dim = S.win.dim;
    }
}

///////////////////////////////////////////////////////////////////////////////
// SET
///////////////////////////////////////////////////////////////////////////////

void pico_set_alpha (int a) {
    S.alpha = a;
}

void pico_set_color_clear (Pico_Color color) {
    S.color.clear = color;
}

void pico_set_color_draw  (Pico_Color color) {
    S.color.draw = color;
}

void pico_set_dim (Pico_Rel_Dim* dim) {
    assert(S.layer==&G.main && "can only set dim from main layer");
    pico_set_window(NULL, -1, dim);
    pico_set_view(-1, dim, NULL, NULL, NULL, NULL, NULL, NULL);
}

void pico_set_expert (int on) {
    S.expert = on;
    G.main.view.grid = 0;
}

void pico_set_font (const char* path) {
    S.font = path;
}

void pico_set_layer (const char* name) {
    if (name == NULL) {
        S.layer = &G.main;
    } else {
        int n = sizeof(Pico_Key) + strlen(name) + 1;
        Pico_Key* res = alloca(n);
        res->type = PICO_KEY_LAYER;
        strcpy(res->key, name);
        Pico_Layer* data = (Pico_Layer*)ttl_hash_get(G.hash, n, res);
        pico_assert(data!=NULL && "layer does not exist");
        S.layer = data;
    }

    SDL_SetRenderTarget(G.ren, S.layer->tex);
    SDL_Rect r = pico_cv_rect_rel_abs (
        &S.layer->view.clip, NULL
    );
    SDL_RenderSetClipRect(G.ren, &r);
}

void pico_set_show (int on) {
    if (on) {
        SDL_ShowWindow(G.win);
        _pico_output_present(0);
    } else {
        SDL_HideWindow(G.win);
    }
}

void pico_set_style (PICO_STYLE style) {
    S.style = style;
}

///////////////////////////////////////////////////////////////////////////////
// PUSH / POP
///////////////////////////////////////////////////////////////////////////////

void pico_push (void) {
    assert(STACK.n<PICO_STACK_MAX && "stack overflow");
    STACK.buf[STACK.n++] = (Pico_State) {
        .alpha = S.alpha,
        .color = { S.color.clear, S.color.draw },
        .font  = S.font,
        .style = S.style,
        .layer = S.layer,
    };
}

void pico_pop (void) {
    assert(STACK.n>0 && "stack underflow");
    Pico_State* st = &STACK.buf[--STACK.n];
    S.alpha = st->alpha;
    S.color.clear = st->color.clear;
    S.color.draw  = st->color.draw;
    S.font  = st->font;
    S.style = st->style;
    if (S.layer != st->layer) {
        S.layer = st->layer;
        SDL_SetRenderTarget(G.ren, S.layer->tex);
        SDL_Rect r = pico_cv_rect_rel_abs (
            &S.layer->view.clip, NULL
        );
        SDL_RenderSetClipRect(G.ren, &r);
    }
}

void pico_set_view (
    int            grid,
    Pico_Rel_Dim*  dim,
    Pico_Abs_Dim*  tile,
    Pico_Rel_Rect* dst,
    Pico_Rel_Rect* src,
    Pico_Rel_Rect* clip,
    Pico_Rot*      rot,
    PICO_FLIP*     flip
) {
    if (grid != -1) {
        S.layer->view.grid = grid;
    }
    if (tile != NULL) {
        S.layer->view.tile = *tile; // (must be set before dim)
    }
    if (dim != NULL) { // recreates texture
        assert(dim->mode!='%' && dim->up==NULL);
        Pico_Abs_Dim di = pico_cv_dim_rel_abs(dim, NULL);
        S.layer->view.dim = di;
        if (S.layer->tex != NULL) {
            SDL_DestroyTexture(S.layer->tex);
        }
        S.layer->tex = SDL_CreateTexture (
            G.ren, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
            di.w, di.h
        );
        pico_assert(S.layer->tex != NULL);
        // main layer uses BLENDMODE_NONE to prevent 2x blend
        SDL_BlendMode mode = (S.layer == &G.main) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND;
        SDL_SetTextureBlendMode(S.layer->tex, mode);
        SDL_SetRenderTarget(G.ren, S.layer->tex);
        SDL_Rect r = pico_cv_rect_rel_abs (
            &S.layer->view.clip, NULL
        );
        SDL_RenderSetClipRect(G.ren, &r);
    }

    if (dst != NULL) {
        S.layer->view.dst = *dst;
    }
    if (src != NULL) {
        S.layer->view.src = *src;
    }
    if (clip != NULL) {
        S.layer->view.clip = *clip;
    }
    if (rot != NULL) {
        S.layer->view.rot = *rot;
    }
    if (flip != NULL) {
        S.layer->view.flip = *flip;
    }
    _pico_output_present(0);
}

void pico_set_window (const char* title, int fs, Pico_Rel_Dim* dim) {
    Pico_Abs_Dim new;

    // title: set window title
    if (title != NULL) {
        SDL_SetWindowTitle(G.win, title);
    }

    // fs: fullscreen
    if (fs!=-1 && fs!=S.win.fs) {
        assert(dim == NULL);
        static Pico_Abs_Dim _old;
        G.fsing = 1;
        if (fs) {
            _old = S.win.dim;
            int ret = SDL_SetWindowFullscreen(G.win, SDL_WINDOW_FULLSCREEN_DESKTOP);
            pico_assert(ret == 0);
            pico_input_delay(50);    // TODO: required for some reason
            SDL_GetWindowSize(G.win, &new.w, &new.h);
        }
        else {
            pico_assert(0 == SDL_SetWindowFullscreen(G.win, 0));
            new = _old;
        }
        S.win.fs = fs;
        S.win.dim = new;
        SDL_SetWindowSize(G.win, new.w, new.h);
    }

    // dim: window dimensions
    if (dim != NULL) {
        assert(fs==-1 && !S.win.fs);
        assert(dim->mode!='%' && dim->up==NULL);
        G.tgt = 0;
        Pico_Abs_Dim di = pico_cv_dim_rel_abs(dim, NULL);
        G.tgt = 1;
        S.win.dim = di;
        SDL_SetWindowSize(G.win, di.w, di.h);
    }

    _pico_output_present(0);
}

///////////////////////////////////////////////////////////////////////////////
// LAYER
///////////////////////////////////////////////////////////////////////////////

static Pico_Layer* _pico_layer_buffer (
    const char* name,
    Pico_Abs_Dim dim,
    const Pico_Color_A* pixels
) {
    assert(name!=NULL && "layer name required");
    assert(pixels!=NULL && "pixels required");

    int n = sizeof(Pico_Key) + strlen(name) + 1;

    Pico_Key* key = alloca(n);
    key->type = PICO_KEY_LAYER;
    strcpy(key->key, name);

    Pico_Layer* data = (Pico_Layer*)ttl_hash_get(G.hash, n, key);
    if (data != NULL) {
        return data;
    }

    SDL_Surface* sfc = SDL_CreateRGBSurfaceWithFormatFrom(
        (void*)pixels, dim.w, dim.h,
        32, 4 * dim.w, SDL_PIXELFORMAT_RGBA32
    );
    SDL_Texture* tex = SDL_CreateTextureFromSurface(G.ren, sfc);
    pico_assert(tex != NULL);
    SDL_FreeSurface(sfc);

    data = malloc(sizeof(Pico_Layer));
    *data = (Pico_Layer) {
        .key  = ttl_hash_put(G.hash, n, key, data),
        .tex  = tex,
        .view = {
            .grid = 0,
            .dim  = dim,
            .dst  = {'%', {.5,.5,1,1}, PICO_ANCHOR_C, NULL},
            .src  = {'%', {.5,.5,1,1}, PICO_ANCHOR_C, NULL},
            .clip = {'%', {.5,.5,1,1}, PICO_ANCHOR_C, NULL},
            .tile = {0, 0},
            .rot  = {0, PICO_ANCHOR_C},
            .flip = PICO_FLIP_NONE,
        },
    };
    assert(data->key != NULL);
    SDL_SetTextureBlendMode(data->tex, SDL_BLENDMODE_BLEND);

    return data;
}

const char* pico_layer_buffer (
    const char* name,
    Pico_Abs_Dim dim,
    const Pico_Color_A* pixels
) {
    return _pico_layer_buffer(name, dim, pixels)->key->key;
}

const char* pico_layer_empty (const char* name, Pico_Abs_Dim dim) {
    assert(name!=NULL && "layer name required");

    int n = sizeof(Pico_Key) + strlen(name) + 1;
    Pico_Key* key = alloca(n);
    key->type = PICO_KEY_LAYER;
    strcpy(key->key, name);

    Pico_Layer* data = (Pico_Layer*)ttl_hash_get(G.hash, n, key);
    if (data != NULL) {
        return data->key->key;
    }

    data = malloc(sizeof(Pico_Layer));
    *data = (Pico_Layer) {
        .key  = ttl_hash_put(G.hash, n, key, data),
        .tex  = _tex_create(dim),
        .view = {
            .grid = 0,
            .dim  = dim,
            .dst  = {'%', {.5,.5,1,1}, PICO_ANCHOR_C, NULL},
            .src  = {'%', {.5,.5,1,1}, PICO_ANCHOR_C, NULL},
            .clip = {'%', {.5,.5,1,1}, PICO_ANCHOR_C, NULL},
            .tile = {0, 0},
            .rot  = {0, PICO_ANCHOR_C},
            .flip = PICO_FLIP_NONE,
        },
    };
    assert(data->key != NULL);
    SDL_SetTextureBlendMode(data->tex, SDL_BLENDMODE_BLEND);

    return data->key->key;
}

static Pico_Layer* _pico_layer_image (const char* name, const char* path) {
    assert(path!=NULL && "image path required");

    const char* str = (name != NULL) ? name : path;
    int n = sizeof(Pico_Key) + strlen(str) + 1;

    Pico_Key* key = alloca(n);
    key->type = PICO_KEY_LAYER;
    strcpy(key->key, str);

    Pico_Layer* data = (Pico_Layer*)ttl_hash_get(G.hash, n, key);
    if (data != NULL) {
        return data;
    }

    SDL_Texture* tex = IMG_LoadTexture(G.ren, path);
    pico_assert(tex != NULL);
    Pico_Abs_Dim dim;
    SDL_QueryTexture(tex, NULL, NULL, &dim.w, &dim.h);

    data = malloc(sizeof(Pico_Layer));
    *data = (Pico_Layer) {
        .key  = ttl_hash_put(G.hash, n, key, data),
        .tex  = tex,
        .view = {
            .grid = 0,
            .dim  = dim,
            .dst  = {'%', {.5,.5,1,1}, PICO_ANCHOR_C, NULL},
            .src  = {'%', {.5,.5,1,1}, PICO_ANCHOR_C, NULL},
            .clip = {'%', {.5,.5,1,1}, PICO_ANCHOR_C, NULL},
            .tile = {0, 0},
            .rot  = {0, PICO_ANCHOR_C},
            .flip = PICO_FLIP_NONE,
        },
    };
    assert(data->key != NULL);
    SDL_SetTextureBlendMode(data->tex, SDL_BLENDMODE_BLEND);

    return data;
}

const char* pico_layer_image (const char* name, const char* path) {
    return _pico_layer_image(name, path)->key->key;
}

static Pico_Layer* _pico_layer_text (
    const char* name,
    int height,
    const char* text
) {
    assert(text!=NULL && text[0]!='\0' && "text required");

    const char* font = S.font;
    Pico_Color clr = S.color.draw;

    int n;
    char* key_buf = NULL;
    if (name == NULL) {
        // /text/<font>/<height>/<r>.<g>.<b>/<text>
        const char* font_str = font ? font : "null";
        n = sizeof(Pico_Key) + strlen("/text/") + strlen(font_str) + 1
            + 10 + 1 + 3+1+3+1+3 + 1 + strlen(text) + 1;
        key_buf = alloca(n - sizeof(Pico_Key));
        snprintf(key_buf, n - sizeof(Pico_Key), "/text/%s/%d/%d.%d.%d/%s",
                 font_str, height, clr.r, clr.g, clr.b, text);
        n = sizeof(Pico_Key) + strlen(key_buf) + 1;
    } else {
        n = sizeof(Pico_Key) + strlen(name) + 1;
    }

    Pico_Key* key = alloca(n);
    key->type = PICO_KEY_LAYER;
    if (name == NULL) {
        strcpy(key->key, key_buf);
    } else {
        strcpy(key->key, name);
    }

    Pico_Layer* data = (Pico_Layer*)ttl_hash_get(G.hash, n, key);
    if (data != NULL) {
        return data;
    }

    Pico_Abs_Dim dim;
    SDL_Texture* tex = _tex_text(height, text, &dim);

    data = malloc(sizeof(Pico_Layer));
    *data = (Pico_Layer) {
        .key  = ttl_hash_put(G.hash, n, key, data),
        .tex  = tex,
        .view = {
            .grid = 0,
            .dim  = dim,
            .dst  = {'%', {.5,.5,1,1}, PICO_ANCHOR_C, NULL},
            .src  = {'%', {.5,.5,1,1}, PICO_ANCHOR_C, NULL},
            .clip = {'%', {.5,.5,1,1}, PICO_ANCHOR_C, NULL},
            .tile = {0, 0},
        },
    };
    assert(data->key != NULL);
    SDL_SetTextureBlendMode(data->tex, SDL_BLENDMODE_BLEND);

    return data;
}

const char* pico_layer_text (const char* name, int height, const char* text) {
    return _pico_layer_text(name, height, text)->key->key;
}

///////////////////////////////////////////////////////////////////////////////
// INPUT
///////////////////////////////////////////////////////////////////////////////

// Pre-handles input from environment:
//  - SDL_QUIT: quit
//  - CTRL_-/=: zoom
//  - CTRL_L/R/U/D: scroll
//  - receives:
//      - e:  actual input
//      - xp: input I was expecting
//  - returns
//      - 1: if e matches xp
//      - 0: otherwise
static int event_from_sdl (Pico_Event* e, int xp) {
    switch (e->type) {
        case SDL_QUIT: {
            if (!S.expert) {
                exit(0);
            }
            break;
        }

        case SDL_WINDOWEVENT: {
            if (e->window.event == SDL_WINDOWEVENT_RESIZED) {
                if (G.fsing) {
                    G.fsing = 0;
                } else {
                    Pico_Rel_Dim phy = { '!', {e->window.data1, e->window.data2}, NULL };
                    pico_set_window(NULL, -1, &phy);
                }
            }
            break;
        }

        case SDL_KEYDOWN: {
            int ctrl = (e->key.keysym.mod & KMOD_CTRL);
            if (!ctrl) {
                break;
            }
            switch (e->key.keysym.sym) {
                case SDLK_0: {
                    // TODO: how to restore initial view?
                    break;
                }
                case SDLK_MINUS: {
                    // Zoom out - expand src by 10%
                    assert(S.layer == &G.main);
                    Pico_Rel_Rect pct = {'%', {0}, PICO_ANCHOR_C, NULL};
                    pico_cv_rect_rel_rel(&S.layer->view.src, &pct, NULL);
                    pct.w += 0.1;
                    pct.h += 0.1;
                    Pico_Rel_Rect out = S.layer->view.src;
                    pico_cv_rect_rel_rel(&pct, &out, NULL);
                    pico_set_view(-1, NULL, NULL, NULL, &out, NULL, NULL, NULL);
                    break;
                }
                case SDLK_EQUALS: {
                    // Zoom in - shrink src by 10%
                    assert(S.layer == &G.main);
                    Pico_Rel_Rect pct = {'%', {0}, PICO_ANCHOR_C, NULL};
                    pico_cv_rect_rel_rel(&S.layer->view.src, &pct, NULL);
                    pct.w -= 0.1;
                    pct.h -= 0.1;
                    Pico_Rel_Rect out = S.layer->view.src;
                    pico_cv_rect_rel_rel(&pct, &out, NULL);
                    pico_set_view(-1, NULL, NULL, NULL, &out, NULL, NULL, NULL);
                    break;
                }
                case SDLK_LEFT: {
                    // Scroll left by 10%
                    assert(S.layer == &G.main);
                    Pico_Rel_Rect pct = {'%', {0}, PICO_ANCHOR_C, NULL};
                    pico_cv_rect_rel_rel(&S.layer->view.src, &pct, NULL);
                    pct.x -= 0.1;
                    Pico_Rel_Rect out = S.layer->view.src;
                    pico_cv_rect_rel_rel(&pct, &out, NULL);
                    pico_set_view(-1, NULL, NULL, NULL, &out, NULL, NULL, NULL);
                    break;
                }
                case SDLK_RIGHT: {
                    // Scroll right by 10%
                    assert(S.layer == &G.main);
                    Pico_Rel_Rect pct = {'%', {0}, PICO_ANCHOR_C, NULL};
                    pico_cv_rect_rel_rel(&S.layer->view.src, &pct, NULL);
                    pct.x += 0.1;
                    Pico_Rel_Rect out = S.layer->view.src;
                    pico_cv_rect_rel_rel(&pct, &out, NULL);
                    pico_set_view(-1, NULL, NULL, NULL, &out, NULL, NULL, NULL);
                    break;
                }
                case SDLK_UP: {
                    // Scroll up by 10%
                    assert(S.layer == &G.main);
                    Pico_Rel_Rect pct = {'%', {0}, PICO_ANCHOR_C, NULL};
                    pico_cv_rect_rel_rel(&S.layer->view.src, &pct, NULL);
                    pct.y -= 0.1;
                    Pico_Rel_Rect out = S.layer->view.src;
                    pico_cv_rect_rel_rel(&pct, &out, NULL);
                    pico_set_view(-1, NULL, NULL, NULL, &out, NULL, NULL, NULL);
                    break;
                }
                case SDLK_DOWN: {
                    // Scroll down by 10%
                    assert(S.layer == &G.main);
                    Pico_Rel_Rect pct = {'%', {0}, PICO_ANCHOR_C, NULL};
                    pico_cv_rect_rel_rel(&S.layer->view.src, &pct, NULL);
                    pct.y += 0.1;
                    Pico_Rel_Rect out = S.layer->view.src;
                    pico_cv_rect_rel_rel(&pct, &out, NULL);
                    pico_set_view(-1, NULL, NULL, NULL, &out, NULL, NULL, NULL);
                    break;
                }
                case SDLK_g: {
                    assert(S.layer == &G.main);
                    pico_set_view(!G.main.view.grid, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
                    break;
                }
                case SDLK_s: {
                    pico_output_screenshot(NULL, NULL);
                    break;
                }
            }
        }
        default:
            // others are not handled automatically
            break;
    }

    if (xp == (int)e->type) {
        // OK
    } else if (xp == SDL_ANY) {
        // MAYBE
        int ok1 = (e->type==SDL_KEYDOWN || e->type==SDL_KEYUP);
        int ok2 = ok1 || e->type==SDL_MOUSEBUTTONDOWN || e->type==SDL_MOUSEBUTTONUP || e->type==SDL_MOUSEMOTION;
        int ok3 = ok2 || e->type==SDL_QUIT;
        int ok4 = ok3 || (e->type==SDL_WINDOWEVENT && e->window.event==SDL_WINDOWEVENT_RESIZED);
        if (ok4) {
            // OK
        } else {
            // NO
            return 0;   // not the one I was expecting
        }
    } else {
        // NO
        return 0;   // not the one I was expecting
    }

    // adjusts SDL -> logical positions
    switch (e->type) {
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEMOTION:  {
            Pico_Rel_Pos pos = { '!' };
            pico_get_mouse(&pos, PICO_EVENT_MOUSE_BUTTON_NONE);
            e->button.x = pos.x;
            e->button.y = pos.y;
            break;
        }
        default:
            break;
    }
    return 1;
}

void pico_input_delay (int ms) {
    ttl_hash_tick(G.hash);
    while (1) {
        int old = SDL_GetTicks();
        Pico_Event e;
        int has = SDL_WaitEventTimeout(&e, ms);
        if (has) {
            event_from_sdl(&e, SDL_ANY);
        }
        int dt = SDL_GetTicks() - old;
        ms -= dt;
        if (ms <= 0) {
            return;
        }
    }
}

void pico_input_event (Pico_Event* evt, int type) {
    ttl_hash_tick(G.hash);
    while (1) {
        Pico_Event x;
        SDL_WaitEvent(&x);
        if (event_from_sdl(&x, type)) {
            if (evt != NULL) {
                *evt = x;
            }
            return;
        }
    }
}

int pico_input_event_ask (Pico_Event* evt, int type) {
    int has = SDL_PollEvent(evt);
    if (!has) return 0;
    return event_from_sdl(evt, type);
}

int pico_input_event_timeout (Pico_Event* evt, int type, int timeout) {
    ttl_hash_tick(G.hash);
    int has = SDL_WaitEventTimeout(evt, timeout);
    if (!has) {
        return 0;
    }
    if (event_from_sdl(evt, type)) {
        return 1;
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// OUTPUT
///////////////////////////////////////////////////////////////////////////////

void pico_output_clear (void) {
    SDL_SetRenderDrawColor(G.ren,
        S.color.clear.r, S.color.clear.g, S.color.clear.b, 0xFF);
    SDL_Rect r = pico_cv_rect_rel_abs(&S.layer->view.clip, NULL);
    SDL_RenderFillRect(G.ren, &r);
    _pico_output_present(0);
}

void pico_output_draw_buffer (
    const char* name,
    Pico_Abs_Dim dim,
    const Pico_Color_A buffer[],
    const Pico_Rel_Rect* rect
) {
    assert(name!=NULL && "layer name required");
    const char* key = pico_layer_buffer(name, dim, buffer);
    pico_output_draw_layer(key, (Pico_Rel_Rect*)rect);
}

void pico_output_draw_image (const char* path, Pico_Rel_Rect* rect) {
    Pico_Layer* layer = _pico_layer_image(NULL, path);
    Pico_Rel_Dim rel = { rect->mode, {rect->w, rect->h}, rect->up };
    Pico_Abs_Dim* orig = (rel.w==0 || rel.h==0) ? &layer->view.dim : NULL;
    _sdl_dim(&rel, NULL, orig);
    rect->w = rel.w;
    rect->h = rel.h;
    _pico_output_draw_layer(layer, rect);
}

static void _pico_output_draw_layer (Pico_Layer* layer, Pico_Rel_Rect* rect) {
    SDL_Rect dst;
    if (rect == NULL) {
        dst = pico_cv_rect_rel_abs (
            &layer->view.dst,
            &(Pico_Abs_Rect){0, 0, S.win.dim.w, S.win.dim.h}
        );
    } else {
        Pico_Abs_Dim* dp = NULL;
        if (rect->w == 0 || rect->h == 0) {
            dp = &layer->view.dim;
        }
        SDL_FRect rf = _sdl_rect(rect, NULL, dp);
        dst = _fi_rect(&rf);
    }

    SDL_SetTextureAlphaMod(layer->tex, S.alpha);
    SDL_Rect src = pico_cv_rect_rel_abs (
        &layer->view.src,
        &(Pico_Abs_Rect){0, 0, layer->view.dim.w, layer->view.dim.h}
    );
    SDL_Point center = {
        dst.w * layer->view.rot.anchor.x,
        dst.h * layer->view.rot.anchor.y
    };
    SDL_RenderCopyEx(G.ren, layer->tex, &src, &dst,
                     layer->view.rot.angle, &center, layer->view.flip);
    _pico_output_present(0);
}

void pico_output_draw_layer (const char* name, Pico_Rel_Rect* rect) {
    assert(name!=NULL && "layer name required");

    int n = sizeof(Pico_Key) + strlen(name) + 1;
    Pico_Key* key = alloca(n);
    key->type = PICO_KEY_LAYER;
    strcpy(key->key, name);
    Pico_Layer* layer = (Pico_Layer*)ttl_hash_get(G.hash, n, key);
    pico_assert(layer!=NULL && "layer does not exist");

    _pico_output_draw_layer(layer, rect);
}

void pico_output_draw_line (Pico_Rel_Pos* p1, Pico_Rel_Pos* p2) {
    SDL_SetRenderDrawColor(G.ren,
        S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha);
    Pico_Abs_Pos i1 = pico_cv_pos_rel_abs(p1, NULL);
    Pico_Abs_Pos i2 = pico_cv_pos_rel_abs(p2, NULL);
    SDL_RenderDrawLine(G.ren, i1.x,i1.y, i2.x,i2.y);
    _pico_output_present(0);
}

void pico_output_draw_oval (Pico_Rel_Rect* rect) {
    Pico_Abs_Rect i = pico_cv_rect_rel_abs(rect, NULL);
    SDL_SetRenderDrawColor(G.ren,
        S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha);
    switch (S.style) {
        case PICO_STYLE_FILL:
            filledEllipseRGBA (G.ren,
                i.x+i.w/2, i.y+i.h/2, i.w/2, i.h/2,
                S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha
            );
            break;
        case PICO_STYLE_STROKE:
            ellipseRGBA (G.ren,
                i.x+i.w/2, i.y+i.h/2, i.w/2, i.h/2,
                S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha
            );
            break;
    }
    _pico_output_present(0);
}

void pico_output_draw_pixel (Pico_Rel_Pos* pos) {
    SDL_SetRenderDrawColor(G.ren,
        S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha);
    Pico_Abs_Pos i = pico_cv_pos_rel_abs(pos, NULL);
    SDL_RenderDrawPoint(G.ren, i.x, i.y);
        // TODO: could use PointF, but 4.5->4 (not 5 desired)
    _pico_output_present(0);
}

void pico_output_draw_pixels (int n, const Pico_Rel_Pos* ps) {
    SDL_Point vs[n];
    for (int i=0; i<n; i++) {
        vs[i] = pico_cv_pos_rel_abs(&ps[i], NULL);
    }
    SDL_SetRenderDrawColor(G.ren,
        S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha);
    SDL_RenderDrawPoints(G.ren, vs, n);
    _pico_output_present(0);
}

void pico_output_draw_rect (Pico_Rel_Rect* rect) {
    SDL_SetRenderDrawColor(G.ren,
        S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha);

    Pico_Abs_Rect i = pico_cv_rect_rel_abs(rect, NULL);
    switch (S.style) {
        case PICO_STYLE_FILL:
            SDL_RenderFillRect(G.ren, &i);
            break;
        case PICO_STYLE_STROKE:
            SDL_RenderDrawRect(G.ren, &i);
            break;
    }
    _pico_output_present(0);
}

void pico_output_draw_poly (int n, const Pico_Rel_Pos* ps) {
    Sint16 xs[n], ys[n];
    for (int i=0; i<n; i++) {
        Pico_Abs_Pos v = pico_cv_pos_rel_abs(&ps[i], NULL);
        xs[i] = v.x;
        ys[i] = v.y;
    }
    SDL_SetRenderDrawColor(G.ren,
        S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha);
    switch (S.style) {
        case PICO_STYLE_FILL:
            filledPolygonRGBA(G.ren,
                xs, ys, n,
                S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha
            );
            break;
        case PICO_STYLE_STROKE:
            polygonRGBA(G.ren,
                xs, ys, n,
                S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha
            );
            break;
    }
    _pico_output_present(0);
}

void pico_output_draw_text (const char* text, Pico_Rel_Rect* rect) {
    assert(text[0] != '\0');
    assert(rect->h != 0);
    Pico_Rel_Dim rel_h = { rect->mode, {0, rect->h}, rect->up };
    SDL_FDim fd_h = _sdl_dim(&rel_h, NULL, NULL);
    int height = (int)fd_h.h;
    Pico_Layer* layer = _pico_layer_text(NULL, height, text);
    Pico_Rel_Dim rel = { rect->mode, {rect->w, rect->h}, rect->up };
    Pico_Abs_Dim* orig = (rel.w == 0) ? &layer->view.dim : NULL;
    _sdl_dim(&rel, NULL, orig);
    rect->w = rel.w;
    _pico_output_draw_layer(layer, rect);
}

void pico_output_draw_tri (Pico_Rel_Pos* p1, Pico_Rel_Pos* p2, Pico_Rel_Pos* p3) {
    Pico_Abs_Pos i1 = pico_cv_pos_rel_abs(p1, NULL);
    Pico_Abs_Pos i2 = pico_cv_pos_rel_abs(p2, NULL);
    Pico_Abs_Pos i3 = pico_cv_pos_rel_abs(p3, NULL);

    SDL_SetRenderDrawColor(G.ren,
        S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha);
    switch (S.style) {
        case PICO_STYLE_FILL:
            filledTrigonRGBA(G.ren,
                i1.x, i1.y,
                i2.x, i2.y,
                i3.x, i3.y,
                S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha
            );
            break;
        case PICO_STYLE_STROKE:
            trigonRGBA(G.ren,
                i1.x, i1.y,
                i2.x, i2.y,
                i3.x, i3.y,
                S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha
            );
            break;
    }
    _pico_output_present(0);
}

static void _show_grid (void) {
    if (!S.layer->view.grid) return;

    Pico_Color x_clr = S.color.draw;
    int x_alpha = S.alpha;
    pico_set_color_draw((Pico_Color){0x77, 0x77, 0x77});

    // grid lines
    {
        pico_set_alpha(0x77);
        if ((S.win.dim.w%S.layer->view.dim.w == 0) && (S.layer->view.dim.w< S.win.dim.w)) {
            for (int i=0; i<S.win.dim.w; i+=(S.win.dim.w/S.layer->view.dim.w)) {
                if (i == 0) continue;
                pico_output_draw_line (
                    &(Pico_Rel_Pos){ '!', {i,0}, PICO_ANCHOR_NW, NULL },
                    &(Pico_Rel_Pos){ '!', {i, S.win.dim.h}, PICO_ANCHOR_NW, NULL }
                );
            }
        }
        if ((S.win.dim.h%S.layer->view.dim.h == 0) && (S.layer->view.dim.h < S.win.dim.h)) {
            for (int j=0; j<S.win.dim.h; j+=(S.win.dim.h/S.layer->view.dim.h)) {
                if (j == 0) continue;
                pico_output_draw_line (
                    &(Pico_Rel_Pos){ '!', {0,j}, PICO_ANCHOR_NW, NULL },
                    &(Pico_Rel_Pos){ '!', {S.win.dim.w,j}, PICO_ANCHOR_NW, NULL }
                );
            }
        }
    }

    // metric labels
    {
        pico_set_alpha(0xFF);
        int H = 10;
        SDL_Rect src = pico_cv_rect_rel_abs (
                &S.layer->view.src,
                &(Pico_Abs_Rect){0, 0, S.layer->view.dim.w, S.layer->view.dim.h}
        );

        for (int x=0; x<S.win.dim.w; x+=50) {
            if (x == 0) continue;
            int v = src.x + (x * src.w / S.win.dim.w);
            char lbl[8];
            snprintf(lbl, sizeof(lbl), "%d", v);
            Pico_Abs_Dim dim = pico_get_text(lbl, &(Pico_Rel_Dim){ '!', {0, H}, NULL });
            pico_output_draw_text (
                lbl,
                &(Pico_Rel_Rect){ '!', {x-dim.w/2, 10-dim.h/2, 0, dim.h}, PICO_ANCHOR_NW, NULL }
            );
        }

        for (int y=0; y<S.win.dim.h; y+=50) {
            if (y == 0) continue;
            int v = src.y + (y * src.h / S.win.dim.h);
            char lbl[8];
            snprintf(lbl, sizeof(lbl), "%d", v);
            Pico_Abs_Dim dim = pico_get_text(lbl, &(Pico_Rel_Dim){ '!', {0, H}, NULL });
            pico_output_draw_text (
                lbl,
                &(Pico_Rel_Rect){ '!', {10-dim.w/2, y-dim.h/2, 0, dim.h}, PICO_ANCHOR_NW, NULL }
            );
        }
    }

    S.color.draw = x_clr;
    S.alpha = x_alpha;
}

static void _pico_output_present (int force) {
    if (G.tgt == 0) {
        return;
    } else if (force) {
        // ok
    } else if (S.expert) {
        return;
    } else if (S.layer != &G.main) {
        return;  // auto-present only on main layer
    }
    if (!G.init) {
        return;
    }

    G.tgt = 0;
    SDL_SetRenderTarget(G.ren, NULL);
    SDL_SetRenderDrawColor(G.ren, 0x77,0x77,0x77,0x77);
    SDL_RenderClear(G.ren);

    // Clip src/dst rectangles to their respective bounds
    {
        // Clips rect 'a' to bounds [0,0,max_w,max_h] and propagates changes to 'b'
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
        SDL_Rect src = pico_cv_rect_rel_abs (
            &G.main.view.src,
            &(Pico_Abs_Rect){0, 0, G.main.view.dim.w, G.main.view.dim.h}
        );
        SDL_Rect dst = pico_cv_rect_rel_abs (
            &G.main.view.dst, NULL
        );
        aux(&dst, &src, S.win.dim.w, S.win.dim.h);
        aux(&src, &dst, G.main.view.dim.w, G.main.view.dim.h);
        SDL_RenderCopy(G.ren, G.main.tex, &src, &dst);
    }

    _show_grid();
    SDL_RenderPresent(G.ren);

    G.tgt = 1;
    SDL_SetRenderTarget(G.ren, G.main.tex);
    SDL_Rect r = pico_cv_rect_rel_abs (
        &G.main.view.clip, NULL
    );
    SDL_RenderSetClipRect(G.ren, &r);
}

void pico_output_present (void) {
    assert(S.layer==&G.main && "can only present from main layer");
    _pico_output_present(1);
}

static void _pico_output_sound_cache (const char* path, int cache) {
    Mix_Chunk* mix = NULL;

    if (cache) {
        int n = sizeof(Pico_Key) + strlen(path) + 1;
        Pico_Key* res = alloca(n);
        res->type = PICO_KEY_SOUND;
        strcpy(res->key, path);

        mix = (Mix_Chunk*)ttl_hash_get(G.hash, n, res);
        if (mix == NULL) {
            mix = Mix_LoadWAV(path);
            ttl_hash_put(G.hash, n, res, mix);
        }
    } else {
        mix = Mix_LoadWAV(path);
    }
    pico_assert(mix != NULL);

    Mix_PlayChannel(-1, mix, 0);

    if (!cache) {
        Mix_FreeChunk(mix);
    }
}

const char* pico_output_screenshot (const char* path, const Pico_Rel_Rect* rect) {
    assert(S.layer == &G.main);
    Pico_Abs_Rect phy = {0, 0, S.win.dim.w, S.win.dim.h};
    SDL_Rect ri = (rect == NULL) ? phy : pico_cv_rect_rel_abs(rect, &phy);

    const char* ret;
    if (path != NULL) {
        ret = path;
    } else {
        static char _path_[32] = "";
        time_t ts = time(NULL);
        struct tm* ti = localtime(&ts);
        assert(strftime(_path_, 32, "pico-sdl-%Y%m%d-%H%M%S.png", ti) == 28);
        ret = _path_;
    }

    SDL_SetRenderTarget(G.ren, NULL);
    pico_input_delay(5);            // TODO: bug if removed
    //SDL_RenderPresent(G.ren);

    void* buf = malloc(4 * ri.w * ri.h);
    SDL_RenderReadPixels(G.ren, &ri, SDL_PIXELFORMAT_RGBA32, buf, 4*ri.w);
    SDL_Surface* sfc = SDL_CreateRGBSurfaceWithFormatFrom (
        buf, ri.w, ri.h, 32, 4*ri.w, SDL_PIXELFORMAT_RGBA32
    );
    pico_assert(IMG_SavePNG(sfc,ret) == 0);
    free(buf);
    SDL_FreeSurface(sfc);

    SDL_SetRenderTarget(G.ren, G.main.tex);
    SDL_Rect r = pico_cv_rect_rel_abs (
        &G.main.view.clip, NULL
    );
    SDL_RenderSetClipRect(G.ren, &r);

    return ret;
}

void pico_output_sound (const char* path) {
    _pico_output_sound_cache(path, 1);
}


