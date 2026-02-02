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

typedef enum {
    PICO_RES_IMAGE,
    PICO_RES_SOUND,
    PICO_RES_LAYER,
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

static struct { // internal global state
    int           init;
    ttl_hash*     hash;
    int           fs;
    int           tgt;
    const char*   layer;
    SDL_Window*   win;
    SDL_Renderer* ren;
    SDL_Texture*  tex;
} G = { 0 };

static struct { // exposed global state
    int alpha;
    int angle;
    struct {
        Pico_Color clear;
        Pico_Color draw;
    } color;
    Pico_Abs_Rect crop;
    int expert;
    const char* font;
    PICO_STYLE style;
    struct {
        int           grid;
        int           fs;
        Pico_Abs_Dim  phy;
        Pico_Abs_Rect dst;
        Pico_Abs_Dim  log;
        Pico_Abs_Rect src;
        Pico_Abs_Rect clip;
        Pico_Abs_Dim  tile;
    } view;
} S;

///////////////////////////////////////////////////////////////////////////////

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

static SDL_Texture* _tex_image (const char* path) {
    int n = sizeof(Pico_Res) + strlen(path) + 1;
    Pico_Res* res = alloca(n);
    res->type = PICO_RES_IMAGE;
    strcpy(res->path, path);

    SDL_Texture* tex = (SDL_Texture*)ttl_hash_get(G.hash, n, res);
    if (tex == NULL) {
        tex = IMG_LoadTexture(G.ren, path);
        ttl_hash_put(G.hash, n, res, tex);
    }
    pico_assert(tex != NULL);

    return tex;
}

static SDL_Texture* _tex_text (const char* font, int h, const char* text, Pico_Color clr) {
    SDL_Color c = { clr.r, clr.g, clr.b, 0xFF };
    TTF_Font* ttf = _font_open(font, h);
    SDL_Surface* sfc = TTF_RenderText_Solid(ttf, text, c);
    pico_assert(sfc != NULL);
    //assert(sfc->h == h);  // TODO: 11 vs 10
    SDL_Texture* tex = SDL_CreateTextureFromSurface(G.ren, sfc);
    pico_assert(tex != NULL);
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
    SDL_FRect ref,
    const Pico_Abs_Dim* ratio
) {
    if (r == NULL) {
        return ref;
    } else {
        SDL_FRect abs = { r->x, r->y, r->w, r->h };
        SDL_FRect tmp = _f1(r->up, ref, NULL);
        return _f2(abs, r->anchor, tmp, ratio);
    }
}

static SDL_FDim _sdl_dim (
    Pico_Rel_Dim*        dim,
    const Pico_Abs_Rect* ref,
    const Pico_Abs_Dim*  ratio
) {
    SDL_FRect r0;
    if (ref == NULL) {
        r0 = (SDL_FRect) {
            0, 0,
            (G.tgt == 0) ? S.view.phy.w : S.view.log.w,
            (G.tgt == 0) ? S.view.phy.h : S.view.log.h,
        };
    } else {
        r0 = (SDL_FRect) { ref->x, ref->y, ref->w, ref->h };
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
            ret = _f3(dim->w * S.view.tile.w, dim->h * S.view.tile.h, ratio);
            if (dim->w == 0) dim->w = ret.w / S.view.tile.w;
            if (dim->h == 0) dim->h = ret.h / S.view.tile.h;
            break;
        default:
            assert(0 && "invalid mode");
    }
    return ret;
}

static SDL_FPoint _sdl_pos (
    const Pico_Rel_Pos*  pos,
    const Pico_Abs_Rect* ref
) {
    SDL_FPoint ret;
    SDL_FRect r0;
    if (ref == NULL) {
        r0 = (SDL_FRect) {
            0, 0,
            (G.tgt == 0) ? S.view.phy.w : S.view.log.w,
            (G.tgt == 0) ? S.view.phy.h : S.view.log.h,
        };
    } else {
        r0 = (SDL_FRect) { ref->x, ref->y, ref->w, ref->h };
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
                r1.x + (pos->x - 1 + pos->anchor.x)*S.view.tile.w,
                r1.y + (pos->y - 1 + pos->anchor.y)*S.view.tile.h,
            };
            break;
        default:
            assert(0 && "invalid mode");
    }
    return ret;
}

static SDL_FRect _sdl_rect (
    const Pico_Rel_Rect* rect,
    const Pico_Abs_Rect* ref,
    const Pico_Abs_Dim* ratio
) {
    SDL_FRect ret;
    SDL_FRect r0;
    if (ref == NULL) {
        r0 = (SDL_FRect) {
            0, 0,
            (G.tgt == 0) ? S.view.phy.w : S.view.log.w,
            (G.tgt == 0) ? S.view.phy.h : S.view.log.h,
        };
    } else {
        r0 = (SDL_FRect) { ref->x, ref->y, ref->w, ref->h };
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
                rect->w * S.view.tile.w,
                rect->h * S.view.tile.h,
                ratio
            );
            ret = (SDL_FRect) {
                r1.x + (rect->x - 1 + rect->anchor.x)*S.view.tile.w - rect->anchor.x*d.w,
                r1.y + (rect->y - 1 + rect->anchor.y)*S.view.tile.h - rect->anchor.y*d.h,
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

Pico_Abs_Rect* _crop (void) {
    if (S.crop.w==0 || S.crop.h==0) {
        assert(S.crop.w==0 && S.crop.h==0 && S.crop.x==0 && S.crop.y==0);
        return NULL;
    } else {
        return &S.crop;
    }
}

///////////////////////////////////////////////////////////////////////////////

Pico_Abs_Pos pico_cv_pos_rel_abs (const Pico_Rel_Pos* pos, Pico_Abs_Rect* ref) {
    SDL_FPoint pf = _sdl_pos(pos, ref);
    return (Pico_Abs_Pos) _fi_pos(&pf);
}

Pico_Abs_Rect pico_cv_rect_rel_abs (const Pico_Rel_Rect* rect, Pico_Abs_Rect* ref) {
    SDL_FRect rf = _sdl_rect(rect, ref, NULL);
    return (Pico_Abs_Rect) _fi_rect(&rf);
}

int pico_vs_pos_rect (Pico_Rel_Pos* pos, Pico_Rel_Rect* rect) {
    SDL_FPoint pf = _sdl_pos(pos, NULL);
    SDL_FRect  rf = _sdl_rect(rect, NULL, NULL);
    SDL_Point  pi = _fi_pos(&pf);
    SDL_Rect   ri = _fi_rect(&rf);
    return SDL_PointInRect(&pi, &ri);
}

int pico_vs_rect_rect (Pico_Rel_Rect* r1, Pico_Rel_Rect* r2) {
    SDL_FRect f1 = _sdl_rect(r1, NULL, NULL);
    SDL_FRect f2 = _sdl_rect(r2, NULL, NULL);
    SDL_Rect  i1 = _fi_rect(&f1);
    SDL_Rect  i2 = _fi_rect(&f2);
    return SDL_HasIntersection(&i1, &i2);
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
        case PICO_RES_LAYER:
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
        G = (typeof(G)) {
            0, NULL, 0, 1, NULL, NULL, NULL, NULL
        };
        S = (typeof(S)) {
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
                {},
            },
        };

        G.hash = ttl_hash_open(PICO_HASH_BUK, PICO_HASH_TTL, _pico_hash_clean);
        assert(G.hash != NULL);

        pico_assert(0 == SDL_Init(SDL_INIT_EVERYTHING));
        G.win = SDL_CreateWindow (
            PICO_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            S.view.phy.w, S.view.phy.h,
            (SDL_WINDOW_SHOWN /*| SDL_WINDOW_RESIZABLE*/)
        );
        pico_assert(G.win != NULL);

#ifdef PICO_TESTS
        pico_win = G.win;
        G.ren = SDL_CreateRenderer(G.win, -1, SDL_RENDERER_SOFTWARE);
#else
        G.ren = SDL_CreateRenderer(G.win, -1, SDL_RENDERER_ACCELERATED/*|SDL_RENDERER_PRESENTVSYNC*/);
#endif

        pico_assert(G.ren != NULL);
        SDL_SetRenderDrawBlendMode(G.ren, SDL_BLENDMODE_BLEND);

        TTF_Init();
        Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 1024);

        {
            Pico_Rel_Dim phy = { '!', {S.view.phy.w, S.view.phy.h}, NULL };
            Pico_Rel_Dim log = { '!', {S.view.log.w, S.view.log.h}, NULL };
            pico_set_window(NULL, -1, &phy);
            pico_set_view(-1, &log, NULL, NULL, NULL, NULL);
        }
        G.init = 1;

        pico_output_clear();

        SDL_PumpEvents();
        SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
        SDL_SetWindowResizable(G.win, 1);
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
        if (G.tex != NULL) {
            SDL_DestroyTexture(G.tex);
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
                if (G.fs) {
                    G.fs = 0;
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
                    // Zoom out
                    pico_set_view(-1, NULL, NULL,
                        &(Pico_Rel_Rect){'%', {0.5, 0.5, 1.1, 1.1}, PICO_ANCHOR_C, NULL},
                        NULL, NULL
                    );
                    break;
                }
                case SDLK_EQUALS: {
                    // Zoom in
                    pico_set_view(-1, NULL, NULL,
                        &(Pico_Rel_Rect){'%', {0.5, 0.5, 0.9, 0.9}, PICO_ANCHOR_C, NULL},
                        NULL, NULL
                    );
                    break;
                }
                case SDLK_LEFT: {
                    // Scroll left
                    pico_set_view(-1, NULL, NULL,
                        &(Pico_Rel_Rect){'%', {-0.1, 0, 1, 1}, PICO_ANCHOR_NW, NULL},
                        NULL, NULL
                    );
                    break;
                }
                case SDLK_RIGHT: {
                    // Scroll right
                    pico_set_view(-1, NULL, NULL,
                        &(Pico_Rel_Rect){'%', {0.1, 0, 1, 1}, PICO_ANCHOR_NW, NULL},
                        NULL, NULL
                    );
                    break;
                }
                case SDLK_UP: {
                    // Scroll up
                    pico_set_view(-1, NULL, NULL,
                        &(Pico_Rel_Rect){'%', {0, -0.1, 1, 1}, PICO_ANCHOR_NW, NULL},
                        NULL, NULL
                    );
                    break;
                }
                case SDLK_DOWN: {
                    // Scroll down
                    pico_set_view(-1, NULL, NULL,
                        &(Pico_Rel_Rect){'%', {0, 0.1, 1, 1}, PICO_ANCHOR_NW, NULL},
                        NULL, NULL
                    );
                    break;
                }
                case SDLK_g: {
                    pico_set_view(!S.view.grid, NULL, NULL, NULL, NULL, NULL);
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
            pico_get_mouse(&pos, PICO_MOUSE_BUTTON_NONE);
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

// OUTPUT

static void _pico_output_present (int force);

void pico_output_clear (void) {
    SDL_SetRenderDrawColor(G.ren,
        S.color.clear.r, S.color.clear.g, S.color.clear.b, 0xFF);
    SDL_RenderFillRect(G.ren, &S.view.clip);
    _pico_output_present(0);
}

void pico_output_draw_buffer (
    Pico_Abs_Dim dim,
    const Pico_Color_A buffer[],
    const Pico_Rel_Rect* rect
) {
    SDL_Surface* sfc = SDL_CreateRGBSurfaceWithFormatFrom (
        (void*)buffer, dim.w, dim.h,
        32, 4*dim.w, SDL_PIXELFORMAT_RGBA32
    );
    SDL_Texture* tex = SDL_CreateTextureFromSurface(G.ren, sfc);
    pico_assert(tex != NULL);
    SDL_FreeSurface(sfc);

    Pico_Abs_Dim  d;
    Pico_Abs_Dim* dp = NULL;
    if (rect->w == 0 || rect->h == 0) {
        pico_assert(0 == SDL_QueryTexture(tex, NULL, NULL, &d.w, &d.h));
        dp = &d;
    }
    SDL_SetTextureAlphaMod(tex, S.alpha);
    SDL_FRect rf = _sdl_rect(rect, NULL, dp);
    SDL_Rect  ri = _fi_rect(&rf);
    SDL_RenderCopy(G.ren, tex, _crop(), &ri);
    _pico_output_present(0);
}

void pico_output_draw_image (const char* path, Pico_Rel_Rect* rect) {
    Pico_Rel_Dim dim = { rect->mode, {rect->w, rect->h}, rect->up };
    pico_get_image(path, &dim);
    rect->w = dim.w;
    rect->h = dim.h;

    SDL_Texture* tex = _tex_image(path);
    SDL_SetTextureAlphaMod(tex, S.alpha);
    SDL_FRect rf = _sdl_rect(rect, NULL, NULL);
    SDL_Rect  ri = _fi_rect(&rf);
    SDL_RenderCopy(G.ren, tex, _crop(), &ri);
    _pico_output_present(0);
}

void pico_output_draw_line (Pico_Rel_Pos* p1, Pico_Rel_Pos* p2) {
    SDL_SetRenderDrawColor(G.ren,
        S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha);
    SDL_FPoint f1 = _sdl_pos(p1, NULL);
    SDL_FPoint f2 = _sdl_pos(p2, NULL);
    SDL_Point  i1 = _fi_pos(&f1);
    SDL_Point  i2 = _fi_pos(&f2);
    SDL_RenderDrawLine(G.ren, i1.x,i1.y, i2.x,i2.y);
    _pico_output_present(0);
}

void pico_output_draw_oval (Pico_Rel_Rect* rect) {
    SDL_FRect f = _sdl_rect(rect, NULL, NULL);
    SDL_Rect  i = _fi_rect(&f);
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
    SDL_FPoint f = _sdl_pos(pos, NULL);
    SDL_Point  i = _fi_pos(&f);
    SDL_RenderDrawPoint(G.ren, i.x, i.y);
        // TODO: could use PointF, but 4.5->4 (not 5 desired)
    _pico_output_present(0);
}

void pico_output_draw_pixels (int n, const Pico_Rel_Pos* ps) {
    SDL_Point vs[n];
    for (int i=0; i<n; i++) {
        SDL_FPoint f = _sdl_pos(&ps[i], NULL);
        vs[i] = _fi_pos(&f);
    }
    SDL_SetRenderDrawColor(G.ren,
        S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha);
    SDL_RenderDrawPoints(G.ren, vs, n);
    _pico_output_present(0);
}

void pico_output_draw_rect (Pico_Rel_Rect* rect) {
    SDL_SetRenderDrawColor(G.ren,
        S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha);

    SDL_FRect f = _sdl_rect(rect, NULL, NULL);
    SDL_Rect  i = _fi_rect(&f);
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
        SDL_FPoint f = _sdl_pos(&ps[i], NULL);
        SDL_Point  v = _fi_pos(&f);
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
    if (text[0] == '\0') return;
    assert(rect->h != 0);

    Pico_Rel_Dim dim = { rect->mode, {rect->w, rect->h}, rect->up };
    pico_get_text(text, &dim);
    rect->w = dim.w;

    SDL_FRect rf = _sdl_rect(rect, NULL, NULL);
    SDL_Texture* tex = _tex_text(NULL, rf.h, text, S.color.draw);
    SDL_SetTextureAlphaMod(tex, S.alpha);
    SDL_Rect ri = _fi_rect(&rf);
    SDL_RenderCopy(G.ren, tex, _crop(), &ri);
    SDL_DestroyTexture(tex);
    _pico_output_present(0);
}

void pico_output_draw_tri (Pico_Rel_Pos* p1, Pico_Rel_Pos* p2, Pico_Rel_Pos* p3) {
    SDL_FPoint f1 = _sdl_pos(p1, NULL);
    SDL_FPoint f2 = _sdl_pos(p2, NULL);
    SDL_FPoint f3 = _sdl_pos(p3, NULL);
    SDL_Point  i1 = _fi_pos(&f1);
    SDL_Point  i2 = _fi_pos(&f2);
    SDL_Point  i3 = _fi_pos(&f3);

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
                pico_output_draw_line (
                    &(Pico_Rel_Pos){ '!', {i,0}, PICO_ANCHOR_NW, NULL },
                    &(Pico_Rel_Pos){ '!', {i, S.view.phy.h}, PICO_ANCHOR_NW, NULL }
                );
            }
        }
        if ((S.view.phy.h%S.view.log.h == 0) && (S.view.log.h < S.view.phy.h)) {
            for (int j=0; j<S.view.phy.h; j+=(S.view.phy.h/S.view.log.h)) {
                if (j == 0) continue;
                pico_output_draw_line (
                    &(Pico_Rel_Pos){ '!', {0,j}, PICO_ANCHOR_NW, NULL },
                    &(Pico_Rel_Pos){ '!', {S.view.phy.w,j}, PICO_ANCHOR_NW, NULL }
                );
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
            Pico_Abs_Dim dim = pico_get_text(lbl, &(Pico_Rel_Dim){ '!', {0, H}, NULL });
            pico_output_draw_text (
                lbl,
                &(Pico_Rel_Rect){ '!', {x-dim.w/2, 10-dim.h/2, 0, dim.h}, PICO_ANCHOR_NW, NULL }
            );
        }

        for (int y=0; y<S.view.phy.h; y+=50) {
            if (y == 0) continue;
            int v = S.view.src.y + (y * S.view.src.h / S.view.phy.h);
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
        SDL_Rect src = S.view.src;
        SDL_Rect dst = S.view.dst;
        aux(&dst, &src, S.view.phy.w, S.view.phy.h);
        aux(&src, &dst, S.view.log.w, S.view.log.h);
        SDL_RenderCopy(G.ren, G.tex, &src, &dst);
    }

    _show_grid();
    SDL_RenderPresent(G.ren);

    G.tgt = 1;
    SDL_SetRenderTarget(G.ren, G.tex);
    SDL_RenderSetClipRect(G.ren, &S.view.clip);
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

const char* pico_output_screenshot (const char* path, const Pico_Rel_Rect* r) {
    Pico_Abs_Rect phy = {0, 0, S.view.phy.w, S.view.phy.h};
    SDL_Rect ri;
    if (r == NULL) {
        ri = phy;
    } else {
        SDL_FRect rf = _sdl_rect(r, &phy, NULL);
        ri = _fi_rect(&rf);
    }

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

    SDL_SetRenderTarget(G.ren, G.tex);
    SDL_RenderSetClipRect(G.ren, &S.view.clip);

    return ret;
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

int pico_get_key (PICO_KEY key) {
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    return keys[key];
}

int pico_get_mouse (Pico_Rel_Pos* pos, int button) {
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

    switch (pos->mode) {
        case '!':
            pos->x = log_x;
            pos->y = log_y;
            break;
        case '%': {
            Pico_Rel_Rect up;
            if (pos->up == NULL) {
                up = (Pico_Rel_Rect){ '!', {0, 0, S.view.log.w, S.view.log.h}, PICO_ANCHOR_NW, NULL };
            } else {
                assert(0 && "TODO");
            }
            pos->x = (log_x - up.x) / up.w;
            pos->y = (log_y - up.y) / up.h;
            break;
        }
        case '#':
            pos->x = (log_x / (float)S.view.tile.w) + (1 - pos->anchor.x);
            pos->y = (log_y / (float)S.view.tile.h) + (1 - pos->anchor.y);
            break;
        default:
            assert(0 && "invalid mode");
    }

    return masks & SDL_BUTTON(button);
}

Pico_Abs_Rect pico_get_crop (void) {
    return S.crop;
}

Pico_Abs_Dim pico_get_image (const char* path, Pico_Rel_Dim* dim) {
    SDL_Texture* tex = _tex_image(path);
    Pico_Abs_Dim ratio;
    SDL_QueryTexture(tex, NULL, NULL, &ratio.w, &ratio.h);
    if (dim == NULL) {
        return ratio;
    } else {
        SDL_FDim fd = _sdl_dim(dim, NULL, &ratio);
        return (Pico_Abs_Dim){fd.w, fd.h};
    }
}

const char* pico_get_layer (void) {
    return G.layer;
}

const char* pico_layer_empty (const char* name, Pico_Abs_Dim dim) {
    assert(name != NULL && "layer name required");
    assert(name[0] != '/' && "layer name cannot start with '/'");

    // create texture
    SDL_Texture* tex = SDL_CreateTexture(
        G.ren, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
        dim.w, dim.h
    );
    pico_assert(tex != NULL);
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);

    // store in hash
    int n = sizeof(Pico_Res) + strlen(name) + 1;
    Pico_Res* res = alloca(n);
    res->type = PICO_RES_LAYER;
    strcpy(res->path, name);
    ttl_hash_put(G.hash, n, res, tex);

    return name;
}

int pico_get_rotate (void) {
    return S.angle;
}

int pico_get_show (void) {
    return SDL_GetWindowFlags(G.win) & SDL_WINDOW_SHOWN;
}

PICO_STYLE pico_get_style (void) {
    return S.style;
}

Pico_Abs_Dim pico_get_text (const char* text, Pico_Rel_Dim* dim) {
    if (text[0] == '\0') {
        return (Pico_Abs_Dim){0, 0};
    }

    Pico_Abs_Dim  ratio;
    Pico_Abs_Dim* rp = NULL;
    if (dim->w == 0) {
        // texture with any height to get aspect ratio
        SDL_Texture* tex = _tex_text(NULL, 10, text, (Pico_Color){0,0,0});
        pico_assert(0 == SDL_QueryTexture(tex, NULL, NULL, &ratio.w, &ratio.h));
        SDL_DestroyTexture(tex);
        rp = &ratio;
    }

    SDL_FDim fd = _sdl_dim(dim, NULL, rp);
    return (Pico_Abs_Dim){fd.w, fd.h};
}

Uint32 pico_get_ticks (void) {
    return SDL_GetTicks();
}

void pico_get_view (
    int* grid,
    Pico_Abs_Dim* dim,
    Pico_Rel_Rect* target,
    Pico_Rel_Rect* source,
    Pico_Rel_Rect* clip,
    Pico_Abs_Dim* tile
) {
    assert(target==NULL && source==NULL && clip==NULL);
    if (grid != NULL) {
        *grid = S.view.grid;
    }
    if (dim != NULL) {
        *dim = S.view.log;
    }
    if (tile != NULL) {
        *tile = S.view.tile;
    }
}

void pico_get_window (const char** title, int* fs, Pico_Abs_Dim* dim) {
    if (title != NULL) {
        *title = SDL_GetWindowTitle(G.win);
    }
    if (fs != NULL) {
        *fs = S.view.fs;
    }
    if (dim != NULL) {
        *dim = S.view.phy;
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

void pico_set_crop (Pico_Abs_Rect crop) {
    S.crop = crop;
}

void pico_set_expert (int on) {
    S.expert = on;
    S.view.grid = 0;
}

void pico_set_font (const char* path) {
    S.font = path;
}

void pico_set_layer (const char* name) {
    SDL_Texture* tex;
    if (name == NULL) {
        tex = G.tex;
    }
    else {
        int n = sizeof(Pico_Res) + strlen(name) + 1;
        Pico_Res* res = alloca(n);
        res->type = PICO_RES_LAYER;
        strcpy(res->path, name);
        tex = (SDL_Texture*)ttl_hash_get(G.hash, n, res);
        pico_assert(tex != NULL && "layer does not exist");
    }
    G.layer = name;
    SDL_SetRenderTarget(G.ren, tex);
}

void pico_set_rotate (int angle) {
    S.angle = angle;
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

void pico_set_view (
    int            grid,
    Pico_Rel_Dim*  dim,
    Pico_Rel_Rect* target,
    Pico_Rel_Rect* source,
    Pico_Rel_Rect* clip,
    Pico_Abs_Dim*  tile
) {
    { // grid: toggle grid overlay
        if (grid != -1) {
            S.view.grid = grid;
        }
    }
    { // target, source, clip: only assign (no extra processing)
        if (target != NULL) {
            SDL_FRect rf = _sdl_rect(target, NULL, NULL);
            SDL_Rect  ri = _fi_rect(&rf);
            S.view.dst = ri;
        }
        if (source != NULL) {
            SDL_FRect rf;
            switch (source->mode) {
                case '!':
                    rf = _sdl_rect(source, NULL, NULL);
                    break;
                case '%':
                    rf = _sdl_rect(source, &S.view.src, NULL);
                    break;
                default:
                    assert(0 && "TODO");
            }
            SDL_Rect  ri = _fi_rect(&rf);
            S.view.src = ri;
        }
        if (clip != NULL) {
            SDL_FRect rf = _sdl_rect(clip, NULL, NULL);
            SDL_Rect  ri = _fi_rect(&rf);
            S.view.clip = ri;
        }
    }
    { // tile: store tile size (must be set before dim with '#' mode)
        if (tile != NULL) {
            S.view.tile = *tile;
        }
    }
    { // dim - world
        if (dim == NULL) {
            goto _out_;
        }
        SDL_FDim df = _sdl_dim(dim, NULL, NULL);
        Pico_Abs_Dim di = _fi_dim(&df);
        S.view.log = di;
        if (source == NULL) {
            S.view.src = (SDL_Rect) { 0, 0, di.w, di.h };
        }
        if (clip == NULL) {
            S.view.clip = (SDL_Rect) { 0, 0, di.w, di.h };
        }
        if (G.tex != NULL) {
            SDL_DestroyTexture(G.tex);
        }
        G.tex = SDL_CreateTexture (
            G.ren, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
            di.w, di.h
        );
        pico_assert(G.tex != NULL);
        SDL_SetTextureBlendMode(G.tex, SDL_BLENDMODE_NONE); // prevents 2x blend
        SDL_SetRenderTarget(G.ren, G.tex);
        SDL_RenderSetClipRect(G.ren, &S.view.clip);
        _out_:
    }
    _pico_output_present(0);
}

void pico_set_window (const char* title, int fs, Pico_Rel_Dim* dim) {
    Pico_Abs_Dim new;

    { // title: set window title
        if (title != NULL) {
            SDL_SetWindowTitle(G.win, title);
        }
    }
    { // fs - fullscreen
        if ((fs == -1) || (fs && S.view.fs) || (!fs && !S.view.fs)) {
            goto _out1_;
        }
        assert(dim == NULL);
        static Pico_Abs_Dim _old;
        G.fs = 1;
        if (fs) {
            _old = S.view.phy;
            int ret = SDL_SetWindowFullscreen(G.win, SDL_WINDOW_FULLSCREEN_DESKTOP);
            pico_assert(ret == 0);
            pico_input_delay(50);    // TODO: required for some reason
            SDL_GetWindowSize(G.win, &new.w, &new.h);
        } else {
            pico_assert(0 == SDL_SetWindowFullscreen(G.win, 0));
            new = _old;
        }
        S.view.fs = fs;
        goto _dim_;
        _out1_:
    }
    { // dim - window
        if (dim == NULL) {
            goto _out2_;
        }
        assert(fs==-1 && !S.view.fs);
        G.tgt = 0;
        SDL_FDim df = _sdl_dim(dim, NULL, NULL);
        G.tgt = 1;
        Pico_Abs_Dim di = _fi_dim(&df);
        new = di;
        _dim_:
        S.view.phy = new;
        S.view.dst = (SDL_Rect) { 0, 0, new.w, new.h };
        SDL_SetWindowSize(G.win, new.w, new.h);
        _out2_:
    }
    _pico_output_present(0);
}

void pico_set_dim (Pico_Rel_Dim* dim) {
    pico_set_window(NULL, -1, dim);
    pico_set_view(-1, dim, NULL, NULL, NULL, NULL);
}
