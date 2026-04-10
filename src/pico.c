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

#define REALM_C
#include "realm.hc"
#include "tiny_ttf.h"
#include "pico.h"

#define PICO_COLORS_C
#include "colors.h"

#define PICO_ANCHORS_C
#include "anchors.h"

///////////////////////////////////////////////////////////////////////////////
// DATA
///////////////////////////////////////////////////////////////////////////////

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

#include "layers.hc"
#include "mem.hc"
#include "video.hc"

#define MAX(x,y) ((x) > (y) ? (x) : (y))

static struct { // internal global state
    int           init;
    int           fsing;
    realm_t*      realm;
    Pico_Layer    root;
    SDL_Renderer* ren;
    int           presenting;
    SDL_Window*   win;
} G = { 0 };

static struct { // exposed global state
    int alpha;
    struct {
        Pico_Color_A clear;
        Pico_Color draw;
    } color;
    struct {
        int on;
        int fps;
        int ms;
        int t0;
    } expert;
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
        Pico_Color_A clear;
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

static void _show_tile (Pico_View* view, SDL_Rect dst);

static void _pico_output_present (int force);

static void _pico_guard (void) {
    if (!G.init) {
        fprintf(stderr, "ERROR : pico-sdl is not initialized\n");
        abort();
    }
}

static SDL_Texture* _tex_create (Pico_Abs_Dim dim) {
    SDL_Texture* tex = SDL_CreateTexture (
        G.ren, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
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

///////////////////////////////////////////////////////////////////////////////
// CV
///////////////////////////////////////////////////////////////////////////////

#include "aux.hc"

SDL_Point pico_cv_pos_rel_win (const Pico_Rel_Pos* pos, Pico_Abs_Rect* base) {
    SDL_FPoint fp = _sdl_pos(pos, base);
    SDL_FPoint win = _pos_wld_to_win(fp);
    return (SDL_Point) { floorf(win.x + 0.5f), floorf(win.y + 0.5f) };
}

void pico_cv_pos_win_rel (SDL_Point phy, Pico_Rel_Pos* to, Pico_Abs_Rect* base) {
    SDL_FPoint wld = _pos_win_to_wld((SDL_FPoint){phy.x, phy.y});
    _rel_pos(wld, to, base);
}

Pico_Abs_Dim pico_cv_dim_rel_abs (Pico_Rel_Dim* dim, Pico_Abs_Rect* base) {
    SDL_FDim df = _sdl_dim(dim, base, NULL);
    return _abs_dim(&df);
}

void pico_cv_dim_abs_rel (
    const Pico_Abs_Dim* fr, Pico_Rel_Dim* to, Pico_Abs_Rect* base
) {
    _rel_dim((SDL_FDim){fr->w, fr->h}, to, base);
}

void pico_cv_dim_rel_rel (
    Pico_Rel_Dim* fr, Pico_Rel_Dim* to, Pico_Abs_Rect* base
) {
    Pico_Abs_Dim abs = pico_cv_dim_rel_abs(fr, base);
    pico_cv_dim_abs_rel(&abs, to, base);
}

Pico_Abs_Pos pico_cv_pos_rel_abs (const Pico_Rel_Pos* pos, Pico_Abs_Rect* base) {
    SDL_FPoint pf = _sdl_pos(pos, base);
    return (Pico_Abs_Pos) _abs_pos(&pf);
}

Pico_Abs_Rect pico_cv_rect_rel_abs (const Pico_Rel_Rect* rect, Pico_Abs_Rect* base) {
    SDL_FRect rf = _sdl_rect(rect, base, NULL);
    return (Pico_Abs_Rect) _abs_rect(&rf);
}

void pico_cv_pos_abs_rel (
    const Pico_Abs_Pos* fr, Pico_Rel_Pos* to, Pico_Abs_Rect* base
) {
    _rel_pos((SDL_FPoint){fr->x, fr->y}, to, base);
}

void pico_cv_pos_rel_rel (
    const Pico_Rel_Pos* fr, Pico_Rel_Pos* to, Pico_Abs_Rect* base
) {
    Pico_Abs_Pos abs = pico_cv_pos_rel_abs(fr, base);
    pico_cv_pos_abs_rel(&abs, to, base);
}

void pico_cv_rect_abs_rel (
    const Pico_Abs_Rect* fr, Pico_Rel_Rect* to, Pico_Abs_Rect* base
) {
    _rel_rect((SDL_FRect){fr->x, fr->y, fr->w, fr->h}, to, base);
}

void pico_cv_rect_rel_rel (
    const Pico_Rel_Rect* fr, Pico_Rel_Rect* to, Pico_Abs_Rect* base
) {
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

Pico_Color pico_color_hex (uint32_t hex) {
    uint8_t r = (hex >> 16) & 0xFF;
    uint8_t g = (hex >> 8)  & 0xFF;
    uint8_t b =  hex        & 0xFF;
    return (Pico_Color) { r, g, b };
}

///////////////////////////////////////////////////////////////////////////////
// INIT
///////////////////////////////////////////////////////////////////////////////

void pico_init (int on) {
    if (on) {
        assert(G.init == 0);
        pico_assert(0 == SDL_Init(SDL_INIT_EVERYTHING));
        TTF_Init();
        Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 1024);

        G = (typeof(G)) {
            .init  = 0,
            .fsing = 0,
            .realm = realm_open(PICO_HASH_BUK),
            .root  = {
                .type = PICO_LAYER_ROOT,
                .name = "root",
                .tex  = NULL,   // needs G.ren
                .hier = { NULL, NULL, {NULL,NULL} },
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
            .win = SDL_CreateWindow (
                       PICO_TITLE,
                       SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                       PICO_DIM_PHY.w, PICO_DIM_PHY.h,
                       SDL_WINDOW_SHOWN
                   ),
        };
        assert(G.realm != NULL);
        pico_assert(G.win != NULL);
        realm_enter(G.realm);

        S = (typeof(S)) {
            .alpha  = 0xFF,
            .color  = { {0, 0, 0, 0xFF}, PICO_COLOR_WHITE },
            .expert = {0, 0, 0, 0},
            .font   = NULL,
            .layer  = &G.root,
            .style  = PICO_STYLE_FILL,
            .win    = {
                .dim = PICO_DIM_PHY,
                .fs  = 0,
            },
        };

        STACK.n = 0;

        // realm_get("root") resolves
        realm_put(G.realm, '!', strlen("root")+1, "root", NULL, NULL, &G.root);

        // create ren after win
        {
#ifdef PICO_TESTS
            G.ren = SDL_CreateRenderer(G.win, -1, SDL_RENDERER_SOFTWARE);
#else
            G.ren = SDL_CreateRenderer(G.win, -1, SDL_RENDERER_ACCELERATED/*|SDL_RENDERER_PRESENTVSYNC*/);
#endif
            pico_assert(G.ren != NULL);
            SDL_SetRenderDrawBlendMode(G.ren, SDL_BLENDMODE_BLEND);
        }

        G.init = 1;

        // create tex after ren
        {
            G.root.tex = _tex_create(PICO_DIM_LOG);
            SDL_SetTextureBlendMode(G.root.tex, SDL_BLENDMODE_NONE);
            SDL_SetRenderTarget(G.ren, G.root.tex);
            Pico_Abs_Rect r = pico_cv_rect_rel_abs(&G.root.view.clip, NULL);
            SDL_RenderSetClipRect(G.ren, &r);
            pico_output_clear();
        }

        // prevents WMs to resize window at start
        SDL_PumpEvents();
        SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
        SDL_SetWindowResizable(G.win, 1);
    }
    else {
        assert(G.init == 1);
        G.init = 0;

        assert(G.realm != NULL);
        realm_close(G.realm);

        Mix_CloseAudio();
        TTF_Quit();
        if (G.root.tex != NULL) {
            SDL_DestroyTexture(G.root.tex);
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
    _pico_guard();
    SDL_Event e = { .type = SDL_QUIT };
    SDL_PushEvent(&e);
}

///////////////////////////////////////////////////////////////////////////////
// GET
///////////////////////////////////////////////////////////////////////////////

int pico_get_alpha (void) {
    _pico_guard();
    return S.alpha;
}

Pico_Color pico_get_color_clear (void) {
    _pico_guard();
    return _pico_color(S.color.clear);
}

Pico_Color_A pico_get_color_clear_alpha (void) {
    _pico_guard();
    return S.color.clear;
}

Pico_Color pico_get_color_draw (void) {
    _pico_guard();
    return S.color.draw;
}

int pico_get_expert (int* fps) {
    _pico_guard();
    if (fps != NULL) {
        *fps = S.expert.fps;
    }
    return S.expert.on;
}

const char* pico_get_font (void) {
    _pico_guard();
    return S.font;
}

Pico_Abs_Dim pico_get_image (const char* path, Pico_Rel_Dim* rel) {
    _pico_guard();
    if (rel == NULL) {
        Pico_Layer* layer = _pico_layer_image('=', NULL, path);
        return layer->view.dim;
    } else if (rel->w==0 || rel->h==0) {
        Pico_Layer* layer = _pico_layer_image('=', NULL, path);
        SDL_FDim fd = _sdl_dim(rel, NULL, &layer->view.dim);
        return (Pico_Abs_Dim){fd.w, fd.h};
    } else {
        SDL_FDim fd = _sdl_dim(rel, NULL, NULL);
        return (Pico_Abs_Dim){fd.w, fd.h};
    }
}

const char* pico_get_layer (void) {
    _pico_guard();
    return S.layer->name;
}

Pico_Mouse pico_get_mouse (char mode, Pico_Rel_Rect* rect) {
    _pico_guard();

    SDL_Point phy;
    Uint32 masks = SDL_GetMouseState(&phy.x, &phy.y);

    Pico_Mouse m = {
        .mode   = mode,
        .left   = !!(masks & SDL_BUTTON(SDL_BUTTON_LEFT)),
        .right  = !!(masks & SDL_BUTTON(SDL_BUTTON_RIGHT)),
        .middle = !!(masks & SDL_BUTTON(SDL_BUTTON_MIDDLE)),
    };

    if (mode == 'w') {
        m.x = phy.x;
        m.y = phy.y;
    } else {
        Pico_Rel_Pos rel = { .mode=mode, .anchor=PICO_ANCHOR_NW, .up=rect };
        pico_cv_pos_win_rel(phy, &rel, NULL);
        m.x = rel.x;
        m.y = rel.y;
    }

    return m;
}

void pico_set_mouse (Pico_Rel_Pos* pos) {
    _pico_guard();
    SDL_Point phy = pico_cv_pos_rel_win(pos, NULL);
    SDL_WarpMouseInWindow(G.win, phy.x, phy.y);
    SDL_PumpEvents();
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

Uint32 pico_get_now (void) {
    _pico_guard();
    return SDL_GetTicks();
}

int pico_get_show (void) {
    _pico_guard();
    return SDL_GetWindowFlags(G.win) & SDL_WINDOW_SHOWN;
}

PICO_STYLE pico_get_style (void) {
    _pico_guard();
    return S.style;
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
        Pico_Rel_Dim rel_h = { rel->mode, {0, rel->h}, rel->up };
        SDL_FDim fd_h = _sdl_dim(&rel_h, NULL, NULL);
        int height = (int)fd_h.h;
        Pico_Layer* layer = _pico_layer_text(mode, key, height, text);
        SDL_FDim fd = _sdl_dim(rel, NULL, &layer->view.dim);
        return (Pico_Abs_Dim){fd.w, fd.h};
    } else {
        SDL_FDim fd = _sdl_dim(rel, NULL, NULL);
        return (Pico_Abs_Dim){fd.w, fd.h};
    }
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
    _pico_guard();
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
    _pico_guard();
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
    _pico_guard();
    S.alpha = a;
}

void pico_set_color_clear (Pico_Color color) {
    _pico_guard();
    S.color.clear = pico_color_alpha(color, 0xFF);
}

void pico_set_color_clear_alpha (Pico_Color_A color) {
    _pico_guard();
    S.color.clear = color;
}

void pico_set_color_draw  (Pico_Color color) {
    _pico_guard();
    S.color.draw = color;
}

void pico_set_dim (Pico_Rel_Dim* dim) {
    _pico_guard();
    assert(S.layer==&G.root && "can only set dim from main layer");
    pico_set_window(NULL, -1, dim);
    pico_set_view(-1, dim, NULL, NULL, NULL, NULL, NULL, NULL);
}

int pico_set_expert (int on, int fps) {
    _pico_guard();
    assert(fps >= -1);
    S.expert.on  = on;
    S.expert.fps = fps;
    if (on && fps!=0) {
        S.expert.ms = (fps == -1) ? 0 : 1000/fps;
        S.expert.t0 = SDL_GetTicks();
    }
    G.root.view.grid = 0;
    return S.expert.ms;
}

void pico_set_font (const char* path) {
    _pico_guard();
    S.font = path;
}

void pico_set_layer (const char* key) {
    _pico_guard();
    if (key == NULL) {
        S.layer = &G.root;
    } else {
        Pico_Layer* data = (Pico_Layer*)realm_get (
            G.realm, strlen(key)+1, key
        );
        pico_assert(data!=NULL && "layer does not exist");
        pico_assert(data->type!=PICO_LAYER_SUB &&
            "cannot set render target to sub-layer");
        S.layer = data;
    }

    SDL_SetRenderTarget(G.ren, S.layer->tex);
    Pico_Abs_Rect r = pico_cv_rect_rel_abs(&S.layer->view.clip, NULL);
    SDL_RenderSetClipRect(G.ren, &r);
}

void pico_set_show (int on) {
    _pico_guard();
    if (on) {
        SDL_ShowWindow(G.win);
        _pico_output_present(0);
    } else {
        SDL_HideWindow(G.win);
    }
}

void pico_set_style (PICO_STYLE style) {
    _pico_guard();
    S.style = style;
}

///////////////////////////////////////////////////////////////////////////////
// PUSH / POP
///////////////////////////////////////////////////////////////////////////////

void pico_push (void) {
    _pico_guard();
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
    _pico_guard();
    assert(STACK.n>0 && "stack underflow");
    Pico_State* st = &STACK.buf[--STACK.n];
    S.alpha = st->alpha;
    S.color.clear = st->color.clear;
    S.color.draw  = st->color.draw;
    S.font        = st->font;
    S.style       = st->style;
    if (S.layer != st->layer) {
        S.layer = st->layer;
        SDL_SetRenderTarget(G.ren, S.layer->tex);
        Pico_Abs_Rect r = pico_cv_rect_rel_abs(&S.layer->view.clip, NULL);
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
    _pico_guard();
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
        S.layer->tex = _tex_create(di);
        // main layer uses BLENDMODE_NONE to prevent 2x blend
        SDL_BlendMode mode = (S.layer == &G.root) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND;
        SDL_SetTextureBlendMode(S.layer->tex, mode);
        SDL_SetRenderTarget(G.ren, S.layer->tex);
        Pico_Abs_Rect r = pico_cv_rect_rel_abs(&S.layer->view.clip, NULL);
        SDL_RenderSetClipRect(G.ren, &r);
        pico_output_clear();
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
    _pico_guard();
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
        Pico_Abs_Dim di = pico_cv_dim_rel_abs (
            dim, &(Pico_Abs_Rect){0, 0, S.win.dim.w, S.win.dim.h}
        );
        S.win.dim = di;
        SDL_SetWindowSize(G.win, di.w, di.h);
    }

    _pico_output_present(0);
}

#define PICO_MEM_C
#include "mem.hc"

#define PICO_LAYERS_C
#include "layers.hc"

///////////////////////////////////////////////////////////////////////////////
// LAYER
///////////////////////////////////////////////////////////////////////////////

void pico_layer_buffer (
    const char* up,
    const char* key,
    Pico_Abs_Dim dim,
    const Pico_Color_A* pixels
) {
    _pico_guard();
    pico_layer_buffer_mode('!', up, key, dim, pixels);
}

void pico_layer_buffer_mode (
    int mode,
    const char* up,
    const char* key,
    Pico_Abs_Dim dim,
    const Pico_Color_A* pixels
) {
    _pico_guard();
    Pico_Layer* ret = _pico_layer_buffer(mode, key, dim, pixels);
    if (up != NULL) {
        Pico_Layer* UP = (Pico_Layer*) realm_get(G.realm, strlen(up)+1, up);
        assert(UP!=NULL && "invalid up layer");
        _layer_attach(UP, ret);
    }
}

void pico_layer_empty (const char* up, const char* key, Pico_Abs_Dim dim, Pico_Abs_Dim* tile) {
    _pico_guard();
    pico_layer_empty_mode('!', up, key, dim, tile);
}

void pico_layer_empty_mode (int mode, const char* up, const char* key, Pico_Abs_Dim dim, Pico_Abs_Dim* tile) {
    _pico_guard();
    assert(key!=NULL && "layer key required");
    _alloc_empty_t ctx = { dim, tile };
    Pico_Layer* DN = (Pico_Layer*) realm_put (
        G.realm, mode, strlen(key)+1, key,
        _free_layer, _alloc_layer_empty, &ctx
    );
    assert(DN != NULL);
    if (up != NULL) {
        Pico_Layer* UP = (Pico_Layer*) realm_get(G.realm, strlen(up)+1, up);
        assert(UP!=NULL && "invalid up layer");
        _layer_attach(UP, DN);
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
    Pico_Layer* DN = _pico_layer_image(mode, key, path);
    if (up != NULL) {
        Pico_Layer* UP = (Pico_Layer*) realm_get(G.realm, strlen(up)+1, up);
        assert(UP!=NULL && "invalid up layer");
        _layer_attach(UP, DN);
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
    (void)up;
    assert(key!=NULL      && "sub-layer key required");
    assert(parent!=NULL   && "parent key required");
    assert(crop!=NULL     && "crop rect required");
    assert(crop->up==NULL && "crop must not have up chain");

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
    (void)up;
    assert(key!=NULL && "layer key required");
    _pico_layer_text(mode, key, height, text);
}

///////////////////////////////////////////////////////////////////////////////
// INPUT
///////////////////////////////////////////////////////////////////////////////

// Handles auto aids: quit/exit, window resize, ctrl+zoom/scroll/grid.
//

static int pico_event_handler (Pico_Event* pico, int do_exit) {
    switch (pico->type) {
        case PICO_EVENT_QUIT: {
            if (!S.expert.on && do_exit) {
                exit(0);
            }
            break;
        }

        case PICO_EVENT_WIN_RESIZE: {
            if (G.fsing) {
                G.fsing = 0;
            } else {
                Pico_Rel_Dim phy = { '!', {pico->window.w, pico->window.h}, NULL };
                pico_set_window(NULL, -1, &phy);
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
                    assert(S.layer == &G.root);
                    Pico_Rel_Rect pct = {'%', {0}, PICO_ANCHOR_C, NULL};
                    pico_cv_rect_rel_rel(&S.layer->view.src, &pct, NULL);
                    pct.w += 0.1;
                    pct.h += 0.1;
                    Pico_Rel_Rect r = S.layer->view.src;
                    pico_cv_rect_rel_rel(&pct, &r, NULL);
                    pico_set_view(-1, NULL, NULL, NULL, &r, NULL, NULL, NULL);
                    return 1;
                }
                case SDLK_EQUALS: {
                    assert(S.layer == &G.root);
                    Pico_Rel_Rect pct = {'%', {0}, PICO_ANCHOR_C, NULL};
                    pico_cv_rect_rel_rel(&S.layer->view.src, &pct, NULL);
                    pct.w -= 0.1;
                    pct.h -= 0.1;
                    Pico_Rel_Rect r = S.layer->view.src;
                    pico_cv_rect_rel_rel(&pct, &r, NULL);
                    pico_set_view(-1, NULL, NULL, NULL, &r, NULL, NULL, NULL);
                    return 1;
                }
                case SDLK_LEFT: {
                    assert(S.layer == &G.root);
                    Pico_Rel_Rect pct = {'%', {0}, PICO_ANCHOR_C, NULL};
                    pico_cv_rect_rel_rel(&S.layer->view.src, &pct, NULL);
                    pct.x -= 0.1;
                    Pico_Rel_Rect r = S.layer->view.src;
                    pico_cv_rect_rel_rel(&pct, &r, NULL);
                    pico_set_view(-1, NULL, NULL, NULL, &r, NULL, NULL, NULL);
                    return 1;
                }
                case SDLK_RIGHT: {
                    assert(S.layer == &G.root);
                    Pico_Rel_Rect pct = {'%', {0}, PICO_ANCHOR_C, NULL};
                    pico_cv_rect_rel_rel(&S.layer->view.src, &pct, NULL);
                    pct.x += 0.1;
                    Pico_Rel_Rect r = S.layer->view.src;
                    pico_cv_rect_rel_rel(&pct, &r, NULL);
                    pico_set_view(-1, NULL, NULL, NULL, &r, NULL, NULL, NULL);
                    return 1;
                }
                case SDLK_UP: {
                    assert(S.layer == &G.root);
                    Pico_Rel_Rect pct = {'%', {0}, PICO_ANCHOR_C, NULL};
                    pico_cv_rect_rel_rel(&S.layer->view.src, &pct, NULL);
                    pct.y -= 0.1;
                    Pico_Rel_Rect r = S.layer->view.src;
                    pico_cv_rect_rel_rel(&pct, &r, NULL);
                    pico_set_view(-1, NULL, NULL, NULL, &r, NULL, NULL, NULL);
                    return 1;
                }
                case SDLK_DOWN: {
                    assert(S.layer == &G.root);
                    Pico_Rel_Rect pct = {'%', {0}, PICO_ANCHOR_C, NULL};
                    pico_cv_rect_rel_rel(&S.layer->view.src, &pct, NULL);
                    pct.y += 0.1;
                    Pico_Rel_Rect r = S.layer->view.src;
                    pico_cv_rect_rel_rel(&pct, &r, NULL);
                    pico_set_view(-1, NULL, NULL, NULL, &r, NULL, NULL, NULL);
                    return 1;
                }
                case SDLK_g: {
                    assert(S.layer == &G.root);
                    pico_set_view(!G.root.view.grid, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
                    return 1;
                }
                case SDLK_s: {
                    pico_output_screenshot(NULL, NULL);
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
                    return PICO_EVENT_WIN_RESIZE;
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
        case PICO_EVENT_WIN_RESIZE:
            pico->window = (typeof(pico->window)) { sdl->window.data1, sdl->window.data2 };
            break;

        case PICO_EVENT_KEY_DN:
        case PICO_EVENT_KEY_UP:
            pico->keyboard = _pico_keyboard(sdl->key.keysym.sym, sdl->key.keysym.mod);
            break;

        case PICO_EVENT_MOUSE_MOTION:
        case PICO_EVENT_MOUSE_BUTTON_DN:
        case PICO_EVENT_MOUSE_BUTTON_UP:
            pico->mouse = pico_get_mouse('w', NULL);
            break;

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
    if (S.expert.fps == 0) {
        return pico_input_event_timeout(evt, type, -1);
    } else if (S.expert.fps == -1) {
        return pico_input_event_timeout(evt, type, 0);
    }

    int now = SDL_GetTicks();
    int cur = (S.expert.t0 + S.expert.ms) - now;
    if (cur <= 0) {
        while (S.expert.t0+S.expert.ms <= now) {
            S.expert.t0 += S.expert.ms;
        }
        cur = 0;
    }
    Pico_Event xevt;
    if (evt == NULL) {
        evt = &xevt;
    }
    int dt = pico_input_event_timeout(evt, type, cur);
    if (evt->type == PICO_EVENT_NONE) {
        S.expert.t0 += S.expert.ms;
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
    SDL_SetRenderDrawColor(G.ren,
        S.color.clear.r, S.color.clear.g, S.color.clear.b, S.color.clear.a);
    Pico_Abs_Rect r = pico_cv_rect_rel_abs(&S.layer->view.clip, NULL);
    SDL_RenderFillRect(G.ren, &r);
    _pico_output_present(0);
}

void pico_output_draw_buffer (
    const char* key,
    Pico_Abs_Dim dim,
    const Pico_Color_A buffer[],
    const Pico_Rel_Rect* rect
) {
    _pico_guard();
    assert(key!=NULL && "layer key required");
    pico_layer_buffer_mode('=', NULL, key, dim, buffer);
    pico_output_draw_layer(key, (Pico_Rel_Rect*)rect);
}

void pico_output_draw_image (const char* path, Pico_Rel_Rect* rect) {
    _pico_guard();
    Pico_Layer* layer = _pico_layer_image('=', NULL, path);
    Pico_Rel_Dim rel = { rect->mode, {rect->w, rect->h}, rect->up };
    Pico_Abs_Dim* orig = (rel.w==0 || rel.h==0) ? &layer->view.dim : NULL;
    _sdl_dim(&rel, NULL, orig);
    rect->w = rel.w;
    rect->h = rel.h;
    _pico_output_draw_layer(layer, rect);
}

void pico_output_draw_layer (const char* key, Pico_Rel_Rect* rect) {
    _pico_guard();
    assert(key!=NULL && "layer key required");

    Pico_Layer* layer = (Pico_Layer*)realm_get(
        G.realm, strlen(key)+1, key);
    pico_assert(layer!=NULL && "layer does not exist");

    _pico_output_draw_layer(layer, rect);
}

void pico_output_draw_line (Pico_Rel_Pos* p1, Pico_Rel_Pos* p2) {
    _pico_guard();
    SDL_SetRenderDrawColor(G.ren,
        S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha);
    Pico_Abs_Pos i1 = pico_cv_pos_rel_abs(p1, NULL);
    Pico_Abs_Pos i2 = pico_cv_pos_rel_abs(p2, NULL);
    SDL_RenderDrawLine(G.ren, i1.x,i1.y, i2.x,i2.y);
    _pico_output_present(0);
}

void pico_output_draw_oval (Pico_Rel_Rect* rect) {
    _pico_guard();
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
    _pico_guard();
    SDL_SetRenderDrawColor(G.ren,
        S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha);
    Pico_Abs_Pos i = pico_cv_pos_rel_abs(pos, NULL);
    SDL_RenderDrawPoint(G.ren, i.x, i.y);
        // TODO: could use PointF, but 4.5->4 (not 5 desired)
    _pico_output_present(0);
}

void pico_output_draw_pixels (int n, const Pico_Rel_Pos* ps) {
    _pico_guard();
    Pico_Abs_Pos vs[n];
    for (int i=0; i<n; i++) {
        vs[i] = pico_cv_pos_rel_abs(&ps[i], NULL);
    }
    SDL_SetRenderDrawColor(G.ren,
        S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha);
    SDL_RenderDrawPoints(G.ren, vs, n);
    _pico_output_present(0);
}

void pico_output_draw_rect (Pico_Rel_Rect* rect) {
    _pico_guard();
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
    _pico_guard();
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
    _pico_guard();
    pico_output_draw_text_mode('=', NULL, text, rect);
}

void pico_output_draw_text_mode (
    int mode, const char* key,
    const char* text, Pico_Rel_Rect* rect
) {
    _pico_guard();
    if (text[0] == '\0') return;

    assert(rect->h != 0);
    Pico_Rel_Dim rel_h = { rect->mode, {0, rect->h}, rect->up };
    SDL_FDim fd_h = _sdl_dim(&rel_h, NULL, NULL);
    int height = (int)fd_h.h;
    Pico_Layer* layer = _pico_layer_text(mode, key, height, text);
    Pico_Rel_Dim rel = { rect->mode, {rect->w, rect->h}, rect->up };
    Pico_Abs_Dim* orig = (rel.w == 0) ? &layer->view.dim : NULL;
    _sdl_dim(&rel, NULL, orig);
    rect->w = rel.w;
    _pico_output_draw_layer(layer, rect);
}

void pico_output_draw_tri (
    Pico_Rel_Pos* p1, Pico_Rel_Pos* p2, Pico_Rel_Pos* p3
) {
    _pico_guard();
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

    Pico_Color x_clr   = pico_get_color_draw();
    int        x_alpha = pico_get_alpha();
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

    // tile grid lines
    _show_tile(&S.layer->view, (SDL_Rect){0, 0, S.win.dim.w, S.win.dim.h});

    // metric labels
    {
        pico_set_color_draw((Pico_Color){0x77, 0x77, 0x77});
        pico_set_alpha(0xFF);
        int H = 10;
        Pico_Abs_Rect src = pico_cv_rect_rel_abs (
                &S.layer->view.src,
                &(Pico_Abs_Rect){0, 0, S.layer->view.dim.w, S.layer->view.dim.h}
        );

        for (int x=0; x<S.win.dim.w; x+=50) {
            if (x == 0) continue;
            int v = src.x + (x * src.w / S.win.dim.w);
            char lbl[8];
            snprintf(lbl, sizeof(lbl), "%d", v);
            Pico_Abs_Dim dim = pico_get_text (
                lbl,
                &(Pico_Rel_Dim){ '!', {0, H}, NULL }
            );
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
            Pico_Abs_Dim dim = pico_get_text(
                lbl,
                &(Pico_Rel_Dim){ '!', {0, H}, NULL });
            pico_output_draw_text (
                lbl,
                &(Pico_Rel_Rect){ '!', {10-dim.w/2, y-dim.h/2, 0, dim.h}, PICO_ANCHOR_NW, NULL }
            );
        }
    }

    pico_set_color_draw(x_clr);
    pico_set_alpha(x_alpha);
}

static void _show_tile (Pico_View* view, SDL_Rect dst) {
    if (view->tile.w<=0 || view->tile.h<=0) return;

    Pico_Color x_clr   = pico_get_color_draw();
    int        x_alpha = pico_get_alpha();
    PICO_STYLE x_style = pico_get_style();

    pico_set_color_draw((Pico_Color){0xFF, 0xFF, 0xFF});
    pico_set_alpha(0xAA);
    pico_set_style(PICO_STYLE_STROKE);

    // grid
    int dx = dst.w * view->tile.w / view->dim.w;
    int dy = dst.h * view->tile.h / view->dim.h;
    if (dx > 0) {
        for (int i=dx; i<dst.w; i+=dx) {
            pico_output_draw_line (
                &(Pico_Rel_Pos){ '!', {dst.x+i, dst.y}, PICO_ANCHOR_NW, NULL },
                &(Pico_Rel_Pos){ '!', {dst.x+i, dst.y+dst.h}, PICO_ANCHOR_NW, NULL }
            );
        }
    }
    if (dy > 0) {
        for (int j=dy; j<dst.h; j+=dy) {
            pico_output_draw_line (
                &(Pico_Rel_Pos){ '!', {dst.x, dst.y+j}, PICO_ANCHOR_NW, NULL },
                &(Pico_Rel_Pos){ '!', {dst.x+dst.w, dst.y+j}, PICO_ANCHOR_NW, NULL }
            );
        }
    }

    // surrounding rect
    pico_output_draw_rect (
        &(Pico_Rel_Rect){ '!', {dst.x, dst.y, dst.w, dst.h}, PICO_ANCHOR_NW, NULL }
    );

    pico_set_color_draw(x_clr);
    pico_set_alpha(x_alpha);
    pico_set_style(x_style);
}

static void _pico_output_present (int force) {
    if (G.presenting) {
        return;
    } else if (force) {
        // ok
    } else if (S.expert.on) {
        return;
    } else if (S.layer != &G.root) {
        return;  // auto-present only on main layer
    }
    if (!G.init) {
        return;
    }

    G.presenting = 1;

    // composite scene graph onto root.tex
    _layer_traverse(&G.root);

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
        Pico_Abs_Rect src = pico_cv_rect_rel_abs (
            &G.root.view.src,
            &(Pico_Abs_Rect){0, 0, G.root.view.dim.w, G.root.view.dim.h}
        );
        Pico_Abs_Rect dst = pico_cv_rect_rel_abs (
            &G.root.view.dst, &(Pico_Abs_Rect){0, 0, S.win.dim.w, S.win.dim.h}
        );
        aux(&dst, &src, S.win.dim.w, S.win.dim.h);
        aux(&src, &dst, G.root.view.dim.w, G.root.view.dim.h);
        SDL_RenderCopy(G.ren, G.root.tex, &src, &dst);
    }

    _show_grid();
    SDL_RenderPresent(G.ren);

    G.presenting = 0;
    SDL_SetRenderTarget(G.ren, G.root.tex);
    Pico_Abs_Rect r = pico_cv_rect_rel_abs(&G.root.view.clip, NULL);
    SDL_RenderSetClipRect(G.ren, &r);
}

void pico_output_present (void) {
    _pico_guard();
    assert(S.layer==&G.root && "can only present from main layer");
    _pico_output_present(1);
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

const char* pico_output_screenshot (const char* path, const Pico_Rel_Rect* rect) {
    _pico_guard();
    assert(S.layer == &G.root);
    Pico_Abs_Rect phy = {0, 0, S.win.dim.w, S.win.dim.h};
    Pico_Abs_Rect ri = (rect == NULL) ? phy : pico_cv_rect_rel_abs(rect, &phy);

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
    assert(buf != NULL);
    SDL_RenderReadPixels(G.ren, &ri, SDL_PIXELFORMAT_RGBA32, buf, 4*ri.w);
    SDL_Surface* sfc = SDL_CreateRGBSurfaceWithFormatFrom (
        buf, ri.w, ri.h, 32, 4*ri.w, SDL_PIXELFORMAT_RGBA32
    );
    pico_assert(IMG_SavePNG(sfc,ret) == 0);
    free(buf);
    SDL_FreeSurface(sfc);

    SDL_SetRenderTarget(G.ren, G.root.tex);
    Pico_Abs_Rect r = pico_cv_rect_rel_abs(&G.root.view.clip, NULL);
    SDL_RenderSetClipRect(G.ren, &r);

    return ret;
}

void pico_output_sound (const char* path) {
    _pico_guard();
    _pico_output_sound_cache(path, 1);
}

#define PICO_VIDEO_C
#include "video.hc"
