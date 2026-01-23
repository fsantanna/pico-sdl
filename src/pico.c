#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <stdarg.h>

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

typedef enum {
    PICO_RES_IMAGE,
    PICO_RES_SOUND,
} PICO_RES;

typedef struct {
    PICO_RES type;
    char path[];
} Pico_Res;

#define SDL_ANY PICO_ANY
#define MAX(x,y) ((x) > (y) ? (x) : (y))

#ifdef PICO_TESTS
SDL_Window* pico_win;
#endif

static SDL_Window*   WIN;
static SDL_Renderer* REN;
static SDL_Texture*  TEX = NULL;
static int FS = 0;          // fullscreen pending (ignore RESIZED event)
static int TGT = 1;         // 0:phy, 1:log

static ttl_hash* _pico_hash;

static struct {
    int alpha;
    int angle;
    struct {
        Pico_Color clear;
        Pico_Color draw;
    } color;
    Pico_Rect crop;
    int expert;
    const char* font;
    PICO_STYLE style;
    struct {
        int       grid;
        int       fs;
        Pico_Dim  phy;
        Pico_Rect dst;
        Pico_Dim  log;
        Pico_Rect src;
        Pico_Rect clip;
    } view;
} S = {
    0xFF,
    0,
    { {0x00,0x00,0x00}, {0xFF,0xFF,0xFF} },
    {},
    0,
    NULL,
    PICO_STYLE_FILL,
    {
        1,
        0,
        PICO_DIM_PHY,
        {},
        PICO_DIM_LOG,
        {},
        {},
    },
};

static int _round (float v) {
    return (v < 0.0) ? (v - 0.5) : (v + 0.5);
}

static SDL_FRect _cv_rect_pct_raw_pre (SDL_FRect dn, Pico_Pct a, SDL_FRect up) {
    int w = dn.w * up.w;
    int h = dn.h * up.h;
    return (SDL_FRect) {
        up.x + dn.x*up.w - a.x*w,
        up.y + dn.y*up.h - a.y*h,
        w, h,
    };
}

static SDL_FRect _cv_rect_pct_raw_pre_rev (const Pico_Rect_Pct* pct, SDL_FRect ref) {
    if (pct == NULL) {
        return ref;
    } else {
        SDL_FRect raw = { pct->x, pct->y, pct->w, pct->h };
        SDL_FRect tmp = _cv_rect_pct_raw_pre_rev(pct->up, ref);
        return _cv_rect_pct_raw_pre(raw, pct->anchor, tmp);
    }
}

Pico_Pos pico_cv_pos_pct_raw_ext (const Pico_Pos_Pct* p, Pico_Rect ref) {
    SDL_FRect fref = { ref.x, ref.y, ref.w, ref.h };
    SDL_FRect ret = _cv_rect_pct_raw_pre_rev(p->up, fref);
    return (Pico_Pos) {
        _round(ret.x + p->x*ret.w - p->anchor.x),
        _round(ret.y + p->y*ret.h - p->anchor.y),
    };
}

Pico_Pos pico_cv_pos_pct_raw (const Pico_Pos_Pct* p) {
    Pico_Rect ref = {
        0, 0,
        (TGT == 0) ? S.view.phy.w : S.view.log.w,
        (TGT == 0) ? S.view.phy.h : S.view.log.h,
    };
    return pico_cv_pos_pct_raw_ext(p, ref);
}

Pico_Rect pico_cv_rect_pct_raw_ext (const Pico_Rect_Pct* r, Pico_Rect ref) {
    SDL_FRect fref = { ref.x, ref.y, ref.w, ref.h };
    SDL_FRect ret = _cv_rect_pct_raw_pre_rev(r, fref);
    return (Pico_Rect) { _round(ret.x), _round(ret.y), ret.w, ret.h };
}

Pico_Rect pico_cv_rect_pct_raw (const Pico_Rect_Pct* r) {
    Pico_Rect ref = {
        0, 0,
        (TGT == 0) ? S.view.phy.w : S.view.log.w,
        (TGT == 0) ? S.view.phy.h : S.view.log.h,
    };
    return pico_cv_rect_pct_raw_ext(r, ref);
}

// INTERNAL

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

SDL_Texture* _text_tex (const char* font, int h, const char* text, Pico_Color clr) {
    SDL_Color c = { clr.r, clr.g, clr.b, 0xFF };
    TTF_Font* ttf = _font_open(font, h);
    SDL_Surface* sfc = TTF_RenderText_Solid(ttf, text, c);
    pico_assert(sfc != NULL);
    //assert(sfc->h == h);  // TODO: 11 vs 10
    SDL_Texture* tex = SDL_CreateTextureFromSurface(REN, sfc);
    pico_assert(tex != NULL);
    SDL_FreeSurface(sfc);
    TTF_CloseFont(ttf);
    return tex;
}

// UTILS

int pico_vs_pos_rect_raw (Pico_Pos pos, Pico_Rect rect) {
    return SDL_PointInRect(&pos, &rect);
}

int pico_vs_pos_rect_pct (Pico_Pos_Pct* pos, Pico_Rect_Pct* rect) {
    return pico_vs_pos_rect_raw(pico_cv_pos_pct_raw(pos), pico_cv_rect_pct_raw(rect));
}

int pico_vs_rect_rect_raw (Pico_Rect r1, Pico_Rect r2) {
    return SDL_HasIntersection(&r1, &r2);
}

int pico_vs_rect_rect_pct (Pico_Rect_Pct* r1, Pico_Rect_Pct* r2) {
    return pico_vs_rect_rect_raw(pico_cv_rect_pct_raw(r1), pico_cv_rect_pct_raw(r2));
}

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

// INIT

static void _pico_hash_clean (int n, const void* key, void* value) {
    const Pico_Res* res = (const Pico_Res*)key;
    switch (res->type) {
        case PICO_RES_IMAGE:
            SDL_DestroyTexture((SDL_Texture*)value);
            break;
        case PICO_RES_SOUND:
            Mix_FreeChunk((Mix_Chunk*)value);
            break;
        default:
            assert(0 && "invalid resource");
    }
}

void pico_init (int on) {
    if (on) {
        _pico_hash = ttl_hash_open(PICO_HASH_BUK, PICO_HASH_TTL,
                                   _pico_hash_clean);
        pico_assert(0 == SDL_Init(SDL_INIT_EVERYTHING));
        WIN = SDL_CreateWindow (
            PICO_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            S.view.phy.w, S.view.phy.h,
            (SDL_WINDOW_SHOWN /*| SDL_WINDOW_RESIZABLE*/)
        );
        pico_assert(WIN != NULL);

#ifdef PICO_TESTS
        pico_win = WIN;
        REN = SDL_CreateRenderer(WIN, -1, SDL_RENDERER_SOFTWARE);
#else
        REN = SDL_CreateRenderer(WIN, -1, SDL_RENDERER_ACCELERATED/*|SDL_RENDERER_PRESENTVSYNC*/);
#endif

        pico_assert(REN != NULL);
        SDL_SetRenderDrawBlendMode(REN, SDL_BLENDMODE_BLEND);

        TTF_Init();
        Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 1024);

        {
            Pico_Rect phy = { 0, 0, S.view.phy.w, S.view.phy.h };
            Pico_Rect log = { 0, 0, S.view.log.w, S.view.log.h };
            pico_set_view_raw(-1, -1, NULL, &phy, &S.view.log, &log, &log);
        }

        pico_output_clear();

        SDL_PumpEvents();
        SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
        SDL_SetWindowResizable(WIN, 1);
    } else {
        ttl_hash_close(_pico_hash);
        Mix_CloseAudio();
        TTF_Quit();
        if (TEX != NULL) {
            SDL_DestroyTexture(TEX);
        }
        SDL_DestroyRenderer(REN);
        SDL_DestroyWindow(WIN);
        SDL_Quit();
    }
}

// INPUT

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
                if (FS) {
                    FS = 0;
                } else {
                    Pico_Dim phy = { e->window.data1, e->window.data2 };
                    pico_set_view_raw(-1, -1, &phy, NULL, NULL, NULL, NULL);
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
                    // Zoom out
                    pico_set_view_pct(-1, -1, NULL, NULL, NULL,
                        &(Pico_Rect_Pct){0.5, 0.5, 1.1, 1.1, PICO_ANCHOR_C, NULL},
                        NULL
                    );
                    break;
                }
                case SDLK_EQUALS: {
                    // Zoom in
                    pico_set_view_pct(-1, -1, NULL, NULL, NULL,
                        &(Pico_Rect_Pct){0.5, 0.5, 0.9, 0.9, PICO_ANCHOR_C, NULL},
                        NULL
                    );
                    break;
                }
                case SDLK_LEFT: {
                    // Scroll left
                    pico_set_view_pct(-1, -1, NULL, NULL, NULL,
                        &(Pico_Rect_Pct){-0.1, 0, 1, 1, PICO_ANCHOR_NW, NULL},
                        NULL
                    );
                    break;
                }
                case SDLK_RIGHT: {
                    // Scroll right
                    pico_set_view_pct(-1, -1, NULL, NULL, NULL,
                        &(Pico_Rect_Pct){0.1, 0, 1, 1, PICO_ANCHOR_NW, NULL},
                        NULL
                    );
                    break;
                }
                case SDLK_UP: {
                    // Scroll up
                    pico_set_view_pct(-1, -1, NULL, NULL, NULL,
                        &(Pico_Rect_Pct){0, -0.1, 1, 1, PICO_ANCHOR_NW, NULL},
                        NULL
                    );
                    break;
                }
                case SDLK_DOWN: {
                    // Scroll down
                    pico_set_view_pct(-1, -1, NULL, NULL, NULL,
                        &(Pico_Rect_Pct){0, 0.1, 1, 1, PICO_ANCHOR_NW, NULL},
                        NULL
                    );
                    break;
                }
                case SDLK_g: {
                    pico_set_view_raw(!S.view.grid, -1, NULL, NULL, NULL, NULL, NULL);
                    break;
                }
                case SDLK_s: {
                    pico_output_screenshot(NULL);
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
            Pico_Pos pos;
            pico_get_mouse_raw(&pos, PICO_MOUSE_BUTTON_NONE);
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
    ttl_hash_tick(_pico_hash);
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
    ttl_hash_tick(_pico_hash);
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
    ttl_hash_tick(_pico_hash);
    int has = SDL_WaitEventTimeout(evt, timeout);
    if (!has) {
        return 0;
    }
    if (event_from_sdl(evt, type)) {
        return 1;
    }
    return 0;
}

// OUTPUT

static void _pico_output_present (int force);

static Pico_Dim tex_dim_raw (SDL_Texture* tex, Pico_Dim dim) {
    if (dim.w!=0 && dim.h!=0) {
        return dim;
    } else {
        int w, h;
        SDL_QueryTexture(tex, NULL, NULL, &w, &h);
        if (dim.w==0 && dim.h==0) {
            dim.w = w;
            dim.h = h;
        } else if (dim.w == 0) {
            dim.w = dim.h * w / h;
        } else {
            dim.h = dim.w * h / w;
        }
        return dim;
    }
}

static Pico_Rect tex_rect_pct (SDL_Texture* tex, const Pico_Rect_Pct* rect) {
    if (rect->w!=0 && rect->h!=0) {
        return pico_cv_rect_pct_raw(rect);
    } else {
        int w, h;
        SDL_QueryTexture(tex, NULL, NULL, &w, &h);

        Pico_Rect_Pct r = *rect;
        r.w = r.h = 1;
        Pico_Rect v = pico_cv_rect_pct_raw(&r);

        if (rect->w==0 && rect->h==0) {
            r.w = w / (float)v.w;
            r.h = h / (float)v.h;
        } else if (rect->w == 0) {
            r.h = rect->h;
            r.w = r.h * w * v.h / h / v.w;
        } else {
            r.w = rect->w;
            r.h = r.w * h * v.w / w / v.h;
        }
        return pico_cv_rect_pct_raw(&r);
    }
}

void pico_output_clear (void) {
    SDL_SetRenderDrawColor(REN,
        S.color.clear.r, S.color.clear.g, S.color.clear.b, 0xFF);
    SDL_RenderFillRect(REN, &S.view.clip);
    _pico_output_present(0);
}

SDL_Rect* _crop (void) {
    if (S.crop.w==0 || S.crop.h==0) {
        assert(S.crop.w==0 && S.crop.h==0 && S.crop.x==0 && S.crop.y==0);
        return NULL;
    } else {
        return &S.crop;
    }
}

void pico_output_draw_buffer_raw (Pico_Dim dim, const Pico_Color_A buffer[], const Pico_Rect rect) {
    SDL_Surface* sfc = SDL_CreateRGBSurfaceWithFormatFrom (
        (void*)buffer, dim.w, dim.h,
        32, 4*dim.w, SDL_PIXELFORMAT_RGBA32
    );
    SDL_Texture* tex = SDL_CreateTextureFromSurface(REN, sfc);
    pico_assert(tex != NULL);
    Pico_Dim d = tex_dim_raw(tex, (Pico_Dim){rect.w, rect.h});
    SDL_SetTextureAlphaMod(tex, S.alpha);
    SDL_RenderCopy(REN, tex, _crop(), &(Pico_Rect){rect.x,rect.y,d.w,d.h});
    SDL_FreeSurface(sfc);
    SDL_DestroyTexture(tex);
    _pico_output_present(0);
}

void pico_output_draw_buffer_pct (Pico_Dim dim, const Pico_Color_A buffer[], const Pico_Rect_Pct* rect) {
    SDL_Surface* sfc = SDL_CreateRGBSurfaceWithFormatFrom (
        (void*)buffer, dim.w, dim.h,
        32, 4*dim.w, SDL_PIXELFORMAT_RGBA32
    );
    SDL_Texture* tex = SDL_CreateTextureFromSurface(REN, sfc);
    pico_assert(tex != NULL);
    Pico_Rect r = tex_rect_pct(tex, rect);
    SDL_SetTextureAlphaMod(tex, S.alpha);
    SDL_RenderCopy(REN, tex, _crop(), &r);
    SDL_FreeSurface(sfc);
    SDL_DestroyTexture(tex);
    _pico_output_present(0);
}

SDL_Texture* _image (const char* path) {
    int n = sizeof(Pico_Res) + strlen(path) + 1;
    Pico_Res* res = alloca(n);
    res->type = PICO_RES_IMAGE;
    strcpy(res->path, path);

    SDL_Texture* tex = (SDL_Texture*)ttl_hash_get(_pico_hash, n, res);
    if (tex == NULL) {
        tex = IMG_LoadTexture(REN, path);
        ttl_hash_put(_pico_hash, n, res, tex);
    }
    pico_assert(tex != NULL);

    return tex;
}

void pico_output_draw_image_raw (const char* path, Pico_Rect rect) {
    SDL_Texture* tex = _image(path);
    Pico_Dim d = tex_dim_raw(tex, (Pico_Dim){rect.w, rect.h});
    SDL_SetTextureAlphaMod(tex, S.alpha);
    SDL_RenderCopy(REN, tex, _crop(), &(Pico_Rect){rect.x,rect.y,d.w,d.h});
    _pico_output_present(0);
}

void pico_output_draw_image_pct (const char* path, const Pico_Rect_Pct* rect) {
    SDL_Texture* tex = _image(path);
    SDL_SetTextureAlphaMod(tex, S.alpha);
    Pico_Rect r = tex_rect_pct(tex, rect);
    SDL_RenderCopy(REN, tex, _crop(), &r);
    _pico_output_present(0);
}

void pico_output_draw_line_raw (Pico_Pos p1, Pico_Pos p2) {
    SDL_SetRenderDrawColor(REN,
        S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha);
    SDL_RenderDrawLine(REN, p1.x,p1.y, p2.x,p2.y);
    _pico_output_present(0);
}

void pico_output_draw_line_pct (Pico_Pos_Pct* p1, Pico_Pos_Pct* p2) {
    pico_output_draw_line_raw(pico_cv_pos_pct_raw(p1), pico_cv_pos_pct_raw(p2));
}

void pico_output_draw_oval_raw (Pico_Rect rect) {
    SDL_SetRenderDrawColor(REN,
        S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha);
    switch (S.style) {
        case PICO_STYLE_FILL:
            filledEllipseRGBA (REN,
                rect.x+rect.w/2, rect.y+rect.h/2, rect.w/2, rect.h/2,
                S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha
            );
            break;
        case PICO_STYLE_STROKE:
            ellipseRGBA (REN,
                rect.x+rect.w/2, rect.y+rect.h/2, rect.w/2, rect.h/2,
                S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha
            );
            break;
    }
    _pico_output_present(0);
}

void pico_output_draw_oval_pct (const Pico_Rect_Pct* rect) {
    pico_output_draw_oval_raw(pico_cv_rect_pct_raw(rect));
}

void pico_output_draw_pixel_raw (Pico_Pos pos) {
    SDL_SetRenderDrawColor(REN,
        S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha);
    SDL_RenderDrawPoint(REN, pos.x, pos.y);
    _pico_output_present(0);
}

void pico_output_draw_pixel_pct (Pico_Pos_Pct* pos) {
    pico_output_draw_pixel_raw(pico_cv_pos_pct_raw(pos));
}

void pico_output_draw_pixels_raw (int n, const Pico_Pos* ps) {
    SDL_SetRenderDrawColor(REN,
        S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha);
    SDL_RenderDrawPoints(REN, ps, n);
    _pico_output_present(0);
}

void pico_output_draw_pixels_pct (int n, const Pico_Pos_Pct* ps) {
    Pico_Pos vs[n];
    for (int i=0; i<n; i++) {
        vs[i] = pico_cv_pos_pct_raw(&ps[i]);
    }
    pico_output_draw_pixels_raw(n, vs);
}

void pico_output_draw_rect_raw (Pico_Rect rect) {
    SDL_SetRenderDrawColor(REN,
        S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha);
    switch (S.style) {
        case PICO_STYLE_FILL:
            SDL_RenderFillRect(REN, &rect);
            break;
        case PICO_STYLE_STROKE:
            SDL_RenderDrawRect(REN, &rect);
            break;
    }
    _pico_output_present(0);
}

void pico_output_draw_rect_pct (const Pico_Rect_Pct* rect) {
    pico_output_draw_rect_raw(pico_cv_rect_pct_raw(rect));
}

void pico_output_draw_poly_raw (int n, const Pico_Pos* ps) {
    Sint16 xs[n], ys[n];
    for (int i=0; i<n; i++) {
        xs[i] = ps[i].x;
        ys[i] = ps[i].y;
    }
    SDL_SetRenderDrawColor(REN,
        S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha);
    switch (S.style) {
        case PICO_STYLE_FILL:
            filledPolygonRGBA(REN,
                xs, ys, n,
                S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha
            );
            break;
        case PICO_STYLE_STROKE:
            polygonRGBA(REN,
                xs, ys, n,
                S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha
            );
            break;
    }
    _pico_output_present(0);
}

void pico_output_draw_poly_pct (int n, const Pico_Pos_Pct* ps) {
    Pico_Pos vs[n];
    for (int i=0; i<n; i++) {
        vs[i] = pico_cv_pos_pct_raw(&ps[i]);
    }
    pico_output_draw_poly_raw(n, vs);
}

void pico_output_draw_text_raw (const char* text, Pico_Rect rect) {
    if (text[0] == '\0') return;
    SDL_Texture* tex =  _text_tex(NULL, rect.h, text, S.color.draw);
    Pico_Dim d = tex_dim_raw(tex, (Pico_Dim){rect.w, rect.h});
    SDL_SetTextureAlphaMod(tex, S.alpha);
    SDL_RenderCopy(REN, tex, _crop(), &(Pico_Rect){rect.x,rect.y,d.w,d.h});
    SDL_DestroyTexture(tex);
    _pico_output_present(0);
}

void pico_output_draw_text_pct (const char* text, Pico_Rect_Pct* rect) {
    if (text[0] == '\0') return;
    Pico_Rect r1 = pico_cv_rect_pct_raw(rect);
    SDL_Texture* tex =  _text_tex(NULL, r1.h, text, S.color.draw);
    Pico_Rect r = tex_rect_pct(tex, rect);
    SDL_SetTextureAlphaMod(tex, S.alpha);
    SDL_RenderCopy(REN, tex, _crop(), &r);
    SDL_DestroyTexture(tex);
    _pico_output_present(0);
}

void pico_output_draw_tri_raw (Pico_Pos p1, Pico_Pos p2, Pico_Pos p3) {
    SDL_SetRenderDrawColor(REN,
        S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha);
    switch (S.style) {
        case PICO_STYLE_FILL:
            filledTrigonRGBA(REN,
                p1.x, p1.y,
                p2.x, p2.y,
                p3.x, p3.y,
                S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha
            );
            break;
        case PICO_STYLE_STROKE:
            trigonRGBA(REN,
                p1.x, p1.y,
                p2.x, p2.y,
                p3.x, p3.y,
                S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha
            );
            break;
    }
    _pico_output_present(0);
}
void pico_output_draw_tri_pct (const Pico_Pos_Pct* p1, const Pico_Pos_Pct* p2, const Pico_Pos_Pct* p3) {
    pico_output_draw_tri_raw(pico_cv_pos_pct_raw(p1), pico_cv_pos_pct_raw(p2), pico_cv_pos_pct_raw(p3));

}

static void _show_grid (void) {
    if (!S.view.grid) return;

    Pico_Color x_clr = S.color.draw;
    int x_alpha = S.alpha;
    pico_set_color_draw((Pico_Color){0x77, 0x77, 0x77});

    // grid lines
    {
        pico_set_alpha(0x77);
        if ((S.view.phy.w%S.view.log.w == 0) && (S.view.log.w< S.view.phy.w)) {
            for (int i=0; i<S.view.phy.w; i+=(S.view.phy.w/S.view.log.w)) {
                if (i == 0) continue;
                pico_output_draw_line_raw((Pico_Pos){i,0}, (Pico_Pos){i,S.view.phy.h});
            }
        }
        if ((S.view.phy.h%S.view.log.h == 0) && (S.view.log.h < S.view.phy.h)) {
            for (int j=0; j<S.view.phy.h; j+=(S.view.phy.h/S.view.log.h)) {
                if (j == 0) continue;
                pico_output_draw_line_raw((Pico_Pos){0,j}, (Pico_Pos){S.view.phy.w,j});
            }
        }
    }

    // metric labels
    {
        pico_set_alpha(0xFF);
        int H = 10;

        for (int x=0; x<S.view.phy.w; x+=50) {
            if (x == 0) continue;
            int v = S.view.src.x + (x * S.view.src.w / S.view.phy.w);
            char lbl[8];
            snprintf(lbl, sizeof(lbl), "%d", v);
            int W = pico_get_text(H, lbl);
            pico_output_draw_text_raw (
                lbl,
                (Pico_Rect) { x-W/2, 10-H/2, 0, H }
            );
        }

        for (int y=0; y<S.view.phy.h; y+=50) {
            if (y == 0) continue;
            int v = S.view.src.y + (y * S.view.src.h / S.view.phy.h);
            char lbl[8];
            snprintf(lbl, sizeof(lbl), "%d", v);
            int W = pico_get_text(H, lbl);
            pico_output_draw_text_raw (
                lbl,
                (Pico_Rect) { 10-W/2, y-H/2, 0, H }
            );
        }
    }

    S.color.draw = x_clr;
    S.alpha = x_alpha;
}

static void _pico_output_present (int force) {
    if (TGT == 0) {
        return;
    } else if (force) {
        // ok
    } else if (S.expert) {
        return;
    }

    TGT = 0;
    SDL_SetRenderTarget(REN, NULL);
    SDL_SetRenderDrawColor(REN, 0x77,0x77,0x77,0x77);
    SDL_RenderClear(REN);

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
        SDL_Rect src = S.view.src;
        SDL_Rect dst = S.view.dst;
        aux(&dst, &src, S.view.phy.w, S.view.phy.h);
        aux(&src, &dst, S.view.log.w, S.view.log.h);
        SDL_RenderCopy(REN, TEX, &src, &dst);
    }

    _show_grid();
    SDL_RenderPresent(REN);

    TGT = 1;
    SDL_SetRenderTarget(REN, TEX);
    SDL_RenderSetClipRect(REN, &S.view.clip);
}

void pico_output_present (void) {
    _pico_output_present(1);
}

static void _pico_output_sound_cache (const char* path, int cache) {
    Mix_Chunk* mix = NULL;

    if (cache) {
        int n = sizeof(Pico_Res) + strlen(path) + 1;
        Pico_Res* res = alloca(n);
        res->type = PICO_RES_SOUND;
        strcpy(res->path, path);

        mix = (Mix_Chunk*)ttl_hash_get(_pico_hash, n, res);
        if (mix == NULL) {
            mix = Mix_LoadWAV(path);
            ttl_hash_put(_pico_hash, n, res, mix);
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

const char* pico_output_screenshot (const char* path) {
    return pico_output_screenshot_raw(
        path,
        (Pico_Rect){0, 0, S.view.phy.w, S.view.phy.h}
    );
}

const char* pico_output_screenshot_raw (const char* path, Pico_Rect r) {
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

    TGT = 0;
    SDL_SetRenderTarget(REN, NULL);
    pico_input_delay(5);            // TODO: bug if removed
    //SDL_RenderPresent(REN);

    void* buf = malloc(4 * r.w * r.h);
    SDL_RenderReadPixels(REN, &r, SDL_PIXELFORMAT_RGBA32, buf, 4*r.w);
    SDL_Surface* sfc = SDL_CreateRGBSurfaceWithFormatFrom (
        buf, r.w, r.h, 32, 4*r.w, SDL_PIXELFORMAT_RGBA32
    );
    pico_assert(IMG_SavePNG(sfc,ret) == 0);
    free(buf);
    SDL_FreeSurface(sfc);

    SDL_SetRenderTarget(REN, TEX);
    SDL_RenderSetClipRect(REN, &S.view.clip);
    TGT = 1;

    return ret;
}

const char* pico_output_screenshot_pct (const char* path, const Pico_Rect_Pct* rect) {
    TGT = 0;
    Pico_Rect r = pico_cv_rect_pct_raw(rect);
    TGT = 1;
    return pico_output_screenshot_raw(path, r);
}

void pico_output_sound (const char* path) {
    _pico_output_sound_cache(path, 1);
}

// STATE

// GET

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

int pico_get_fullscreen (void) {
    return S.view.fs;
}

int pico_get_key (PICO_KEY key) {
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    return keys[key];
}

int pico_get_mouse_raw (Pico_Pos* pos, int button) {
    int phy_x, phy_y;
    Uint32 masks = SDL_GetMouseState(&phy_x, &phy_y);
    if (button == 0) {
        masks = 0;
    }

    // Convert physical position to logical position considering dst and src

    // 1. Get position relative to dst (normalized 0-1)
    float rel_x = (phy_x - S.view.dst.x) / (float)S.view.dst.w;
    float rel_y = (phy_y - S.view.dst.y) / (float)S.view.dst.h;

    // 2. Convert to logical position within src (zoom/scroll viewport)
    float log_x = S.view.src.x + rel_x * S.view.src.w;
    float log_y = S.view.src.y + rel_y * S.view.src.h;
    pos->x = log_x; //(log_x >= 0) ? (log_x + 0.5) : (log_x - 0.5);
    pos->y = log_y; //(log_y >= 0) ? (log_y + 0.5) : (log_y - 0.5);

    return masks & SDL_BUTTON(button);
}

int pico_get_mouse_pct (Pico_Pos_Pct* pos, int button) {
    Pico_Pos raw;
    int ret = pico_get_mouse_raw(&raw, button);

    Pico_Rect up;
    if (pos->up == NULL) {
        up = (Pico_Rect){0, 0, S.view.log.w, S.view.log.h};
    } else {
        up = pico_cv_rect_pct_raw(pos->up);
    }

    pos->x = (float)(raw.x - up.x) / up.w;
    pos->y = (float)(raw.y - up.y) / up.h;

    return ret;
}

Pico_Rect pico_get_crop (void) {
    return S.crop;
}

Pico_Dim pico_get_image (const char* path) {
    SDL_Texture* tex = _image(path);
    Pico_Dim dim;
    SDL_QueryTexture(tex, NULL, NULL, &dim.w, &dim.h);
    return dim;
}

void pico_get_image_raw (const char* path, Pico_Dim* dim) {
    SDL_Texture* tex = _image(path);
    *dim = tex_dim_raw(tex, *dim);
}

static void _tex_dim_pct (SDL_Texture* tex, Pico_Pct* pct, Pico_Rect_Pct* ref) {
    Pico_Rect_Pct r = { 0, 0, pct->w, pct->h, PICO_ANCHOR_NW, ref };
    Pico_Rect raw = tex_rect_pct(tex, &r);
    Pico_Rect base;
    if (ref == NULL) {
        base = (Pico_Rect){ 0, 0, S.view.log.w, S.view.log.h };
    } else {
        base = pico_cv_rect_pct_raw(ref);
    }
    pct->w = raw.w / (float)base.w;
    pct->h = raw.h / (float)base.h;
}

void pico_get_image_pct (const char* path, Pico_Pct* pct, Pico_Rect_Pct* ref) {
    SDL_Texture* tex = _image(path);
    _tex_dim_pct(tex, pct, ref);
}

int pico_get_rotate (void) {
    return S.angle;
}

int pico_get_show (void) {
    return SDL_GetWindowFlags(WIN) & SDL_WINDOW_SHOWN;
}

PICO_STYLE pico_get_style (void) {
    return S.style;
}

int pico_get_text (int h, const char* text) {
    if (text[0] == '\0') {
        return 0;
    }
    SDL_Texture* tex = _text_tex(NULL, h, text, (Pico_Color){0,0,0});
    int w;
    SDL_QueryTexture(tex, NULL, NULL, &w, NULL);
    SDL_DestroyTexture(tex);
    return w;
}

void pico_get_text_raw (const char* text, Pico_Dim* dim) {
    if (text[0] == '\0') return;
    SDL_Texture* tex = _text_tex(NULL, dim->h, text, (Pico_Color){0,0,0});
    *dim = tex_dim_raw(tex, *dim);
    SDL_DestroyTexture(tex);
}

void pico_get_text_pct (const char* text, Pico_Pct* pct, Pico_Rect_Pct* ref) {
    if (text[0] == '\0') return;
    Pico_Rect_Pct r = { 0, 0, pct->w, pct->h, PICO_ANCHOR_NW, ref };
    Pico_Rect raw = pico_cv_rect_pct_raw(&r);
    SDL_Texture* tex = _text_tex(NULL, raw.h, text, (Pico_Color){0,0,0});
    _tex_dim_pct(tex, pct, ref);
    SDL_DestroyTexture(tex);
}

Uint32 pico_get_ticks (void) {
    return SDL_GetTicks();
}

const char* pico_get_title (void) {
    return SDL_GetWindowTitle(WIN);
}

void pico_get_view (
    int* grid,
    int* fs,
    Pico_Dim* phy,
    Pico_Rect* dst,
    Pico_Dim* log,
    Pico_Rect* src,
    Pico_Rect* clip
) {
    assert(dst==NULL && src==NULL && clip==NULL);
    if (grid != NULL) {
        *grid = S.view.grid;
    }
    if (fs != NULL) {
        *fs = S.view.fs;
    }
    if (phy != NULL) {
        *phy = S.view.phy;
    }
    if (log != NULL) {
        *log = S.view.log;
    }
}

// SET

void pico_set_alpha (int a) {
    S.alpha = a;
}

void pico_set_color_clear (Pico_Color color) {
    S.color.clear = color;
}

void pico_set_color_draw  (Pico_Color color) {
    S.color.draw = color;
}

void pico_set_crop (Pico_Rect crop) {
    S.crop = crop;
}

void pico_set_expert (int on) {
    S.expert = on;
}

void pico_set_font (const char* path) {
    S.font = path;
}

void pico_set_rotate (int angle) {
    S.angle = angle;
}

void pico_set_show (int on) {
    if (on) {
        SDL_ShowWindow(WIN);
        _pico_output_present(0);
    } else {
        SDL_HideWindow(WIN);
    }
}

void pico_set_style (PICO_STYLE style) {
    S.style = style;
}

void pico_set_title (const char* title) {
    SDL_SetWindowTitle(WIN, title);
}

void pico_set_view_raw (
    int        grid,
    int        fs,
    Pico_Dim*  phy,
    Pico_Rect* dst,
    Pico_Dim*  log,
    Pico_Rect* src,
    Pico_Rect* clip
) {
    Pico_Dim new;

    { // grid: toggle grid overlay
        if (grid != -1) {
            S.view.grid = grid;
        }
    }
    { // dst, src, clip: only assign (no extra processing)
        if (dst != NULL) {
            S.view.dst = *dst;
        }
        if (src != NULL) {
            S.view.src = *src;
        }
        if (clip != NULL) {
            S.view.clip = *clip;
        }
    }

    { // fs - fullscreen
        if ((fs == -1) || (fs && S.view.fs) || (!fs && !S.view.fs)) {
            goto _out1_;
        }
        assert(phy == NULL);
        static Pico_Dim _old;
        FS = 1;
        if (fs) {
            _old = S.view.phy;
            int ret = SDL_SetWindowFullscreen(WIN, SDL_WINDOW_FULLSCREEN_DESKTOP);
            pico_assert(ret == 0);
            pico_input_delay(50);    // TODO: required for some reason
            SDL_GetWindowSize(WIN, &new.w, &new.h);
        } else {
            pico_assert(0 == SDL_SetWindowFullscreen(WIN, 0));
            new = _old;
        }
        phy = &new;
        S.view.fs = fs;
        goto _phy_;
        _out1_:
    }
    { // phy - window
        if (phy == NULL) {
            goto _out2_;
        }
        assert(fs==-1 && !S.view.fs);
        _phy_:
        S.view.phy = *phy;
        if (dst == NULL) {
            S.view.dst = (SDL_Rect) { 0, 0, phy->w, phy->h };
        }
        SDL_SetWindowSize(WIN, phy->w, phy->h);
        _out2_:
    }
    { // log - world
        if (log == NULL) {
            goto _out3_;
        }
        S.view.log = *log;
        if (src == NULL) {
            S.view.src = (SDL_Rect) { 0, 0, log->w, log->h };
        }
        if (clip == NULL) {
            S.view.clip = (SDL_Rect) { 0, 0, log->w, log->h };
        }
        if (TEX != NULL) {
            SDL_DestroyTexture(TEX);
        }
        TEX = SDL_CreateTexture (
            REN, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
            log->w, log->h
        );
        pico_assert(TEX != NULL);
        SDL_SetTextureBlendMode(TEX, SDL_BLENDMODE_NONE); // prevents 2x blend
        SDL_SetRenderTarget(REN, TEX);
        SDL_RenderSetClipRect(REN, &S.view.clip);
        _out3_:
    }
    _pico_output_present(0);
}

void pico_set_view_pct (
    int            grid,
    int            fs,
    Pico_Pct*      phy,
    void*          dst_todo,
    Pico_Pct*      log,
    Pico_Rect_Pct* src,
    Pico_Rect_Pct* clip
) {
    Pico_Dim  xphy,  *xxphy  = NULL;
    Pico_Dim  xlog,  *xxlog  = NULL;
    Pico_Rect xsrc,  *xxsrc  = NULL;
    Pico_Rect xclip, *xxclip = NULL;

    if (phy != NULL) {
        xphy = (Pico_Dim) { phy->w*S.view.phy.w, phy->h*S.view.phy.h };
        xxphy = &xphy;
    }
    if (log != NULL) {
        xlog = (Pico_Dim) { log->w*S.view.log.w, log->h*S.view.log.h };
        xxlog = &xlog;
    }
    if (src != NULL) {
        xsrc = pico_cv_rect_pct_raw_ext(src, S.view.src); // relative to itself
        xxsrc = &xsrc;
    }
    if (clip != NULL) {
        xclip = pico_cv_rect_pct_raw(clip);
        xxclip = &xclip;
    }

    pico_set_view_raw(grid, fs, xxphy, dst_todo, xxlog, xxsrc, xxclip);
}
