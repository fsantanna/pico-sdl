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

#include "tiny_ttf.h"
#include "hash.h"
#include "pico.h"

#define SDL_ANY PICO_ANY
#define MAX(x,y) ((x) > (y) ? (x) : (y))

static SDL_Window*  WIN;
static SDL_Texture* TEX;
static int FS = 0;          // fullscreen pending (ignore RESIZED event)

#define REN (SDL_GetRenderer(WIN))

static pico_hash* _pico_hash;

static struct {
    int alpha;
    struct {
        Pico_Anchor pos;
        Pico_Anchor rotate;
    } anchor;
    int angle;
    struct {
        Pico_Color clear;
        Pico_Color draw;
    } color;
    Pico_Rect crop;
    int expert;
    Pico_Flip flip;
    const char* font;
    int grid;
    PICO_STYLE style;
    struct {
        int       fs;
        Pico_Dim  phy;
        Pico_Rect dst;
        Pico_Dim  log;
        Pico_Rect src;
        Pico_Rect clip;
    } view;
} S = {
    0xFF,
    { {PICO_CENTER, PICO_MIDDLE}, {PICO_CENTER, PICO_MIDDLE} },
    0,
    { {0x00,0x00,0x00}, {0xFF,0xFF,0xFF} },
    {},
    0,
    {0, 0},
    NULL,
    1,
    PICO_FILL,
    {
        0,
        PICO_DIM_PHY,
        {},
        PICO_DIM_LOG,
        {},
        {},
    },
};

static Pico_Rect RECT (const Pico_Rect_Pct* r) {
    if (r->up == NULL) {
        int w = r->w * S.view.log.x;
        int h = r->h * S.view.log.y;
        int x = r->x*S.view.log.x - r->anchor.x*w;
        int y = r->y*S.view.log.y - r->anchor.y*h;
        return (Pico_Rect) {x, y, w, h};
    } else {
        Pico_Rect up = RECT(r->up);
        int w = r->w * up.w;
        int h = r->h * up.h;
        int x = up.x + r->x*up.w - r->anchor.x*w;
        int y = up.y + r->y*up.h - r->anchor.y*h;
        return (Pico_Rect) {x, y, w, h};
    }
}

static Pico_Pos POS (const Pico_Pos_Pct* p) {
    if (p->up == NULL) {
        int x = p->x*S.view.log.x - p->anchor.x;
        int y = p->y*S.view.log.y - p->anchor.y;
        return (Pico_Pos) {x, y};
    } else {
        Pico_Rect up = RECT(p->up);
        int x = up.x + p->x*up.w - p->anchor.x;
        int y = up.y + p->y*up.h - p->anchor.y;
        return (Pico_Pos) {x, y};
    }
}

// INTERNAL

static TTF_Font* _font_open (const char* file, int h) {
    TTF_Font* ttf;
    if (file == NULL) {
        SDL_RWops* rw = SDL_RWFromConstMem(pico_tiny_ttf, pico_tiny_ttf_len);
        ttf = TTF_OpenFontRW(rw, 1, h);
    } else {
        ttf = TTF_OpenFont(file, h);
    }
    pico_assert(ttf != NULL);
    return ttf;
}

// UTILS

int pico_pos_vs_rect_raw (Pico_Pos pos, Pico_Rect rect) {
    return SDL_PointInRect(&pos, &rect);
}

int pico_pos_vs_rect_pct (Pico_Pos_Pct* pos, Pico_Rect_Pct* rect) {
    return pico_pos_vs_rect_raw(POS(pos), RECT(rect));
}

int pico_rect_vs_rect_raw (Pico_Rect r1, Pico_Rect r2) {
    return SDL_HasIntersection(&r1, &r2);
}

int pico_rect_vs_rect_pct (Pico_Rect_Pct* r1, Pico_Rect_Pct* r2) {
    return pico_rect_vs_rect_raw(RECT(r1), RECT(r2));
}

// INIT

void pico_init (int on) {
    if (on) {
        _pico_hash = pico_hash_create(PICO_HASH);
        pico_assert(0 == SDL_Init(SDL_INIT_VIDEO));
        WIN = SDL_CreateWindow (
            PICO_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            S.view.phy.x, S.view.phy.y,
            (SDL_WINDOW_SHOWN /*| SDL_WINDOW_RESIZABLE*/)
        );
        pico_assert(WIN != NULL);

        SDL_CreateRenderer(WIN, -1, SDL_RENDERER_ACCELERATED/*|SDL_RENDERER_PRESENTVSYNC*/);
        //SDL_CreateRenderer(WIN, -1, SDL_RENDERER_SOFTWARE);

        pico_assert(REN != NULL);
        SDL_SetRenderDrawBlendMode(REN, SDL_BLENDMODE_BLEND);

        TTF_Init();
        Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 1024);

        {
            Pico_Rect phy = { 0, 0, S.view.phy.x, S.view.phy.y };
            Pico_Rect log = { 0, 0, S.view.log.x, S.view.log.y };
            pico_set_view(-1, NULL, &phy, &S.view.log, &log, &log);
        }

        pico_output_clear();

        SDL_PumpEvents();
        SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
        SDL_SetWindowResizable(WIN, 1);
    } else {
        Mix_CloseAudio();
        TTF_Quit();
        SDL_DestroyRenderer(REN);
        SDL_DestroyWindow(WIN);
        SDL_Quit();
        pico_hash_destroy(_pico_hash);
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
                    pico_set_view(-1, &phy, NULL, NULL, NULL, NULL);
                }
            }
            break;
        }

        case SDL_KEYDOWN: {
            const unsigned char* state = SDL_GetKeyboardState(NULL);
            if (!state[SDL_SCANCODE_LCTRL] && !state[SDL_SCANCODE_RCTRL]) {
                break;
            }
            switch (e->key.keysym.sym) {
                case SDLK_0: {
                    assert(0 && "TODO");
                    //pico_set_zoom((Pico_Dim){0, 0});
                    //pico_set_scroll((Pico_Pos){0, 0});
                    break;
                }
                case SDLK_MINUS: {
                    assert(0 && "TODO");
                    //pico_set_zoom ((Pico_Dim) {
                    //    S.zoom.x + 10,
                    //    S.zoom.y + 10,
                    //});
                    break;
                }
                case SDLK_EQUALS: {
                    assert(0 && "TODO");
                    //pico_set_zoom ((Pico_Dim) {
                    //    S.zoom.x - 10,
                    //    S.zoom.y - 10,
                    //});
                    break;
                }
                case SDLK_LEFT: {
                    assert(0 && "TODO");
                    //pico_set_scroll ((Pico_Pos) {
                    //    S.scroll.x - MAX(1, (100+S.zoom.x)/20),
                    //    S.scroll.y
                    //});
                    break;
                }
                case SDLK_RIGHT: {
                    assert(0 && "TODO");
                    //pico_set_scroll ((Pico_Pos) {
                    //    S.scroll.x + MAX(1, (100+S.zoom.x)/20),
                    //    S.scroll.y
                    //});
                    break;
                }
                case SDLK_UP: {
                    assert(0 && "TODO");
                    //pico_set_scroll ((Pico_Pos) {
                    //    S.scroll.x,
                    //    S.scroll.y - MAX(1, (100+S.zoom.y)/20)
                    //});
                    break;
                }
                case SDLK_DOWN: {
                    assert(0 && "TODO");
                    //pico_set_scroll ((Pico_Pos) {
                    //    S.scroll.x,
                    //    S.scroll.y + MAX(1, (100+S.zoom.y)/20)
                    //});
                    break;
                }
                case SDLK_g: {
                    pico_set_grid(!S.grid);
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

static Pico_Rect tex_rect_raw (SDL_Texture* tex, Pico_Rect rect) {
    if (rect.w!=0 && rect.h!=0) {
        return rect;
    } else {
        int w, h;
        SDL_QueryTexture(tex, NULL, NULL, &w, &h);

        if (rect.w==0 && rect.h==0) {
            rect.w = w;
            rect.h = h;
        } else if (rect.w == 0) {
            rect.w = rect.h * w / h;
        } else {
            rect.h = rect.w * h / w;
        }
        return rect;
    }
}

static Pico_Rect tex_rect_pct (SDL_Texture* tex, const Pico_Rect_Pct* rect) {
    if (rect->w!=0 && rect->h!=0) {
        return RECT(rect);
    } else {
        int w, h;
        SDL_QueryTexture(tex, NULL, NULL, &w, &h);

        Pico_Rect_Pct r = *rect;
        r.w = r.h = 1;
        Pico_Rect v = RECT(&r);

        if (rect->w==0 && rect->h==0) {
            r.w = ((float)w) / v.w;
            r.h = ((float)h) / v.h;
        } else if (rect->w == 0) {
            r.h = rect->h;
            r.w = r.h * w * v.h / h / v.w;
        } else {
            r.w = rect->w;
            r.h = r.w * h * v.w / w / v.h;
        }
        return RECT(&r);
    }
}

void pico_output_clear (void) {
    SDL_SetRenderDrawColor(REN,
        S.color.clear.r, S.color.clear.g, S.color.clear.b, 0xFF);
    SDL_RenderFillRect(REN, &S.view.clip);
    _pico_output_present(0);
}

void pico_output_draw_buffer_raw (const Pico_Rect rect, const Pico_Color buffer[], Pico_Dim dim) {
    SDL_Surface* sfc = SDL_CreateRGBSurfaceWithFormatFrom (
        (void*)buffer, dim.x, dim.y,
        24, 3*dim.x, SDL_PIXELFORMAT_RGB24
    );
    SDL_Texture* tex = SDL_CreateTextureFromSurface(REN, sfc);
    pico_assert(tex != NULL);
    Pico_Rect r = tex_rect_raw(tex, rect);
    SDL_SetTextureAlphaMod(tex, S.alpha);
    SDL_RenderCopy(REN, tex, NULL, &r);
    SDL_FreeSurface(sfc);
    SDL_DestroyTexture(tex);
    _pico_output_present(0);
}

void pico_output_draw_buffer_pct (const Pico_Rect_Pct* rect, const Pico_Color buffer[], Pico_Dim dim) {
    SDL_Surface* sfc = SDL_CreateRGBSurfaceWithFormatFrom (
        (void*)buffer, dim.x, dim.y,
        24, 3*dim.x, SDL_PIXELFORMAT_RGB24
    );
    SDL_Texture* tex = SDL_CreateTextureFromSurface(REN, sfc);
    pico_assert(tex != NULL);
    Pico_Rect r = tex_rect_pct(tex, rect);
    SDL_SetTextureAlphaMod(tex, S.alpha);
    SDL_RenderCopy(REN, tex, NULL, &r);
    SDL_FreeSurface(sfc);
    SDL_DestroyTexture(tex);
    _pico_output_present(0);
}

void pico_output_draw_image_raw (Pico_Rect rect, const char* path) {
    SDL_Texture* tex = (SDL_Texture*)pico_hash_get(_pico_hash, path);
    if (tex == NULL) {
        tex = IMG_LoadTexture(REN, path);
        pico_hash_add(_pico_hash, path, tex);
    }
    pico_assert(tex != NULL);
    Pico_Rect r = tex_rect_raw(tex, rect);
    SDL_SetTextureAlphaMod(tex, S.alpha);
    SDL_RenderCopy(REN, tex, NULL, &r);
    _pico_output_present(0);
}

void pico_output_draw_image_pct (const Pico_Rect_Pct* rect, const char* path) {
    SDL_Texture* tex = (SDL_Texture*)pico_hash_get(_pico_hash, path);
    if (tex == NULL) {
        tex = IMG_LoadTexture(REN, path);
        pico_hash_add(_pico_hash, path, tex);
    }
    pico_assert(tex != NULL);
    Pico_Rect r = tex_rect_pct(tex, rect);
    SDL_SetTextureAlphaMod(tex, S.alpha);
    SDL_RenderCopy(REN, tex, NULL, &r);
    _pico_output_present(0);
}

void pico_output_draw_line_raw (Pico_Pos p1, Pico_Pos p2) {
    SDL_SetRenderDrawColor(REN,
        S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha);
    SDL_RenderDrawLine(REN, p1.x,p1.y, p2.x,p2.y);
    _pico_output_present(0);
}

void pico_output_draw_line_pct (Pico_Pos_Pct* p1, Pico_Pos_Pct* p2) {
    pico_output_draw_line_raw(POS(p1), POS(p2));
}

void pico_output_draw_pixel_raw (Pico_Pos pos) {
    SDL_SetRenderDrawColor(REN,
        S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha);
    SDL_RenderDrawPoint(REN, pos.x, pos.y);
    _pico_output_present(0);
}

void pico_output_draw_pixel_pct (Pico_Pos_Pct* pos) {
    pico_output_draw_pixel_raw(POS(pos));
}

#if 0
void pico_output_draw_pixels (const Pico_Pos* ps, int n) {
    Pico_Pos vec[n];
    for (int i=0; i<n; i++) {
        vec[i].x = X(ps[i].x,1);
        vec[i].y = Y(ps[i].y,1);
    }
    SDL_SetRenderDrawColor(REN,
        S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha);
    SDL_RenderDrawPoints(REN, vec, n);
    _pico_output_present(0);
}
#endif

void pico_output_draw_rect_raw (Pico_Rect rect) {
    SDL_SetRenderDrawColor(REN,
        S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha);
    switch (S.style) {
        case PICO_FILL:
            SDL_RenderFillRect(REN, &rect);
            break;
        case PICO_STROKE:
            SDL_RenderDrawRect(REN, &rect);
            break;
    }
    _pico_output_present(0);
}

void pico_output_draw_rect_pct (const Pico_Rect_Pct* rect) {
    pico_output_draw_rect_raw(RECT(rect));
}

void pico_output_draw_tri_raw (Pico_Pos p1, Pico_Pos p2, Pico_Pos p3) {
    SDL_SetRenderDrawColor(REN,
        S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha);
    switch (S.style) {
        case PICO_FILL:
            filledTrigonRGBA(REN,
                p1.x, p1.y,
                p2.x, p2.y,
                p3.x, p3.y,
                S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha
            );
            break;
        case PICO_STROKE:
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
    pico_output_draw_tri_raw(POS(p1), POS(p2), POS(p3));

}

void pico_output_draw_oval_raw (Pico_Rect rect) {
    SDL_SetRenderDrawColor(REN,
        S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha);
    switch (S.style) {
        case PICO_FILL:
            filledEllipseRGBA (REN,
                rect.x+rect.w/2, rect.y+rect.h/2, rect.w/2, rect.h/2,
                S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha
            );
            break;
        case PICO_STROKE:
            ellipseRGBA (REN,
                rect.x+rect.w/2, rect.y+rect.h/2, rect.w/2, rect.h/2,
                S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha
            );
            break;
    }
    _pico_output_present(0);
}

void pico_output_draw_oval_pct (const Pico_Rect_Pct* rect) {
    pico_output_draw_oval_raw(RECT(rect));
}

void pico_output_draw_poly_raw (const Pico_Pos* ps, int n) {
    Sint16 xs[n], ys[n];
    for (int i=0; i<n; i++) {
        xs[i] = ps[i].x;
        ys[i] = ps[i].y;
    }
    SDL_SetRenderDrawColor(REN,
        S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha);
    switch (S.style) {
        case PICO_FILL:
            filledPolygonRGBA(REN,
                xs, ys, n,
                S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha
            );
            break;
        case PICO_STROKE:
            polygonRGBA(REN,
                xs, ys, n,
                S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha
            );
            break;
    }
    _pico_output_present(0);
}

void pico_output_draw_poly_pct (const Pico_Pos_Pct* ps, int n) {
    Pico_Pos vs[n];
    for (int i=0; i<n; i++) {
        vs[i] = POS(&ps[i]);
    }
    pico_output_draw_poly_raw(vs, n);
}

void pico_output_draw_text_raw (Pico_Rect rect, const char* text) {
    if (text[0] == '\0') return;
    TTF_Font* ttf = _font_open(NULL, rect.h);
    SDL_Surface* sfc = TTF_RenderText_Blended(ttf, text,
        (SDL_Color) { S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha }
    );
    TTF_CloseFont(ttf);
    pico_assert(sfc != NULL);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(REN, sfc);
    pico_assert(tex != NULL);
    Pico_Rect r = tex_rect_raw(tex, rect);
    SDL_SetTextureAlphaMod(tex, S.alpha);
    SDL_RenderCopy(REN, tex, NULL, &r);
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(sfc);
    _pico_output_present(0);
}

void pico_output_draw_text_pct (Pico_Rect_Pct* rect, const char* text) {
    if (text[0] == '\0') return;
    Pico_Rect r1 = RECT(rect);
    TTF_Font* ttf = _font_open(NULL, r1.h);
    SDL_Surface* sfc = TTF_RenderText_Blended(ttf, text,
        (SDL_Color) { S.color.draw.r, S.color.draw.g, S.color.draw.b, S.alpha }
    );
    TTF_CloseFont(ttf);
    pico_assert(sfc != NULL);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(REN, sfc);
    pico_assert(tex != NULL);
    Pico_Rect r = tex_rect_pct(tex, rect);
    SDL_SetTextureAlphaMod(tex, S.alpha);
    SDL_RenderCopy(REN, tex, NULL, &r);
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(sfc);
    _pico_output_present(0);
}

static void _show_grid (void) {
    if (!S.grid) return;

    SDL_SetRenderDrawColor(REN, 0x77,0x77,0x77,0x77);

    if ((S.view.phy.x % S.view.log.x == 0) && (S.view.log.x < S.view.phy.x)) {
        for (int i=0; i<=S.view.phy.x; i+=(S.view.phy.x/S.view.log.x)) {
            SDL_RenderDrawLine(REN, i, 0, i, S.view.phy.y);
        }
    }

    if ((S.view.phy.y % S.view.log.y == 0) && (S.view.log.y < S.view.phy.y)) {
        for (int j=0; j<=S.view.phy.y; j+=(S.view.phy.y/S.view.log.y)) {
            SDL_RenderDrawLine(REN, 0, j, S.view.phy.x, j);
        }
    }
}

static void _pico_output_present (int force) {
    if (S.expert && !force) return;

    SDL_SetRenderTarget(REN, NULL);
    SDL_SetRenderDrawColor(REN, 0x77,0x77,0x77,0x77);
    SDL_RenderClear(REN);

    // Calculate clipped src and dst rectangles for viewport rendering
    {
        SDL_Rect src = S.view.src;
        SDL_Rect dst = S.view.dst;
        if (dst.x < 0) {
            int dx = -dst.x;
            src.x += dx;
            src.w -= dx;        // clip left edge (negative dst.x)
            dst.x = 0;
            dst.w -= dx;
        }
        if (dst.y < 0) {
            int dy = -dst.y;
            src.y += dy;
            src.h -= dy;        // clip top edge (negative dst.y)
            dst.y = 0;
            dst.h -= dy;
        }
        if (dst.x+dst.w > S.view.phy.x) {
            int ex = (dst.x + dst.w) - S.view.phy.x;
            src.w -= ex;        // clip right edge (dst beyond physical width)
            dst.w -= ex;
        }
        if (dst.y+dst.h > S.view.phy.y) {
            int ex = (dst.y + dst.h) - S.view.phy.y;
            src.h -= ex;        // clip bottom edge (dst beyond physical height)
            dst.h -= ex;
        }
        // Only render if there's something visible
        if (src.w>0 && src.h>0 && dst.w>0 && dst.h>0) {
            SDL_RenderCopy(REN, TEX, &src, &dst);
        }
        //SDL_RenderCopy(REN, TEX, NULL, NULL);
    }

    _show_grid();
    SDL_RenderPresent(REN);
    SDL_SetRenderTarget(REN, TEX);
    SDL_RenderSetClipRect(REN, &S.view.clip);
}

void pico_output_present (void) {
    _pico_output_present(1);
}

static void _pico_output_sound_cache (const char* path, int cache) {
    Mix_Chunk* mix = NULL;

    if (cache) {
        mix = (Mix_Chunk*)pico_hash_get(_pico_hash, path);
        if (mix == NULL) {
            mix = Mix_LoadWAV(path);
            pico_hash_add(_pico_hash, path, mix);
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
    return pico_output_screenshot_ext(
        path,
        (Pico_Rect){0,0,S.view.log.x,S.view.log.y}
    );
}

const char* pico_output_screenshot_ext (const char* path, Pico_Rect r) {
    const char* ret;
    if (path != NULL) {
        ret = path;
    } else {
        static char _path_[32] = "";
        time_t ts = time(NULL);
        struct tm* ti = localtime(&ts);
        assert(strftime(_path_,32,"pico-sdl-%Y%m%d-%H%M%S.png",ti) == 28);
        ret = _path_;
    }

    void* buf = malloc(4*r.w*r.h);
    SDL_RenderReadPixels(REN, &r, SDL_PIXELFORMAT_RGBA32, buf, 4*r.w);
    SDL_Surface *sfc = SDL_CreateRGBSurfaceWithFormatFrom(buf, r.w, r.h, 32, 4*r.w,
                                                          SDL_PIXELFORMAT_RGBA32);
    assert(IMG_SavePNG(sfc,ret)==0 && "saving screenshot");
    free(buf);
    SDL_FreeSurface(sfc);
    return ret;
}

void pico_output_sound (const char* path) {
    _pico_output_sound_cache(path, 1);
}

// STATE

// GET

Pico_Anchor pico_get_anchor_pos (void) {
    return S.anchor.pos;
}

Pico_Anchor pico_get_anchor_rotate (void) {
    return S.anchor.rotate;
}

Pico_Color pico_get_color_clear (void) {
    return S.color.clear;
}

Pico_Color pico_get_color_draw (void) {
    return S.color.draw;
}

int pico_get_expert (void) {
    return S.expert;
}

Pico_Flip pico_get_flip (void) {
    return S.flip;
}

const char* pico_get_font (void) {
    return S.font;
}

int pico_get_fullscreen (void) {
    return S.view.fs;
}

int pico_get_grid (void) {
    return S.grid;
}

int pico_get_key (PICO_KEY key) {
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    return keys[key];
}

int pico_get_mouse (Pico_Pos* pos, int button) {
    Pico_Pos local;
    if (pos == NULL) {
        pos = &local;
    }

    Uint32 masks = SDL_GetMouseState(&pos->x, &pos->y);
    if (button == 0) {
        masks = 0;
    }

    // TODO: bug in SDL?
    // https://discourse.libsdl.org/t/sdl-getmousestate-and-sdl-rendersetlogicalsize/20288/7
    pos->x = pos->x * S.view.log.x / S.view.phy.x;
    pos->y = pos->y * S.view.log.y / S.view.phy.y;
    //pos->x += S.scroll.x;
    //pos->y += S.scroll.y;

    return masks & SDL_BUTTON(button);
}

Pico_Rect pico_get_crop (void) {
    return S.crop;
}

int pico_get_rotate (void) {
    return S.angle;
}

Pico_Dim pico_get_dim_image (const char* file) {
    SDL_Texture* tex = (SDL_Texture*)pico_hash_get(_pico_hash, file);
    if (tex == NULL) {
        tex = IMG_LoadTexture(REN, file);
        pico_hash_add(_pico_hash, file, tex);
    }
    pico_assert(tex != NULL);

    Pico_Dim dim;
    SDL_QueryTexture(tex, NULL, NULL, &dim.x, &dim.y);
    return dim;
}

int pico_get_text_width (int h, const char* text) {
    if (text[0] == '\0') {
        return 0;
    }
    TTF_Font* ttf = _font_open(NULL, h);
    SDL_Surface* sfc = TTF_RenderText_Blended(ttf, text, (SDL_Color){0,0,0,255});
    TTF_CloseFont(ttf);
    pico_assert(sfc != NULL);
    assert(sfc->h == h);
    int w = sfc->w;
    SDL_FreeSurface(sfc);
    return w;
}

int pico_get_show (void) {
    return SDL_GetWindowFlags(WIN) & SDL_WINDOW_SHOWN;
}

PICO_STYLE pico_get_style (void) {
    return S.style;
}

Uint32 pico_get_ticks (void) {
    return SDL_GetTicks();
}

const char* pico_get_title (void) {
    return SDL_GetWindowTitle(WIN);
}

void pico_get_view (
    int* fs,
    Pico_Dim* phy,
    Pico_Rect* dst,
    Pico_Dim* log,
    Pico_Rect* src,
    Pico_Rect* clip
) {
    assert(dst==NULL && src==NULL && clip==NULL);
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

void pico_set_anchor_pos (Pico_Anchor anchor) {
    S.anchor.pos = anchor;
}

void pico_set_anchor_rotate (Pico_Anchor anchor) {
    S.anchor.rotate = anchor;
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

void pico_set_flip (Pico_Flip flip) {
    S.flip = flip;
}

void pico_set_font (const char* file) {
    S.font = file;
}

void pico_set_grid (int on) {
    S.grid = on;
    _pico_output_present(0);
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

void pico_set_view (
    int        fs,
    Pico_Dim*  phy,
    Pico_Rect* dst,
    Pico_Dim*  log,
    Pico_Rect* src,
    Pico_Rect* clip
) {
    Pico_Dim new;

    { // dst, src, clip: only assign (no extra processing)
        if (dst != NULL) {
            S.view.dst = *dst;
        }
        if (src != NULL) {
            S.view.src = *src;
        }
        if (clip != NULL) {
            S.view.clip = *clip;
            //S.view.clip = rect;
            //SDL_RenderSetClipRect(REN, &rect);
            //pico_set_clip_raw(RECT(rect));
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
            SDL_GetWindowSize(WIN, &new.x, &new.y);
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
            S.view.dst = (SDL_Rect) { 0, 0, phy->x, phy->y };
        }
        SDL_SetWindowSize(WIN, phy->x, phy->y);
        _out2_:
    }
    { // log - world
        if (log == NULL) {
            goto _out3_;
        }
        S.view.log = *log;
        if (src == NULL) {
            S.view.src = (SDL_Rect) { 0, 0, log->x, log->y };
        }
        if (clip == NULL) {
            S.view.clip = (SDL_Rect) { 0, 0, log->x, log->y };
        }
        SDL_DestroyTexture(TEX);
        TEX = SDL_CreateTexture (
            REN, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
            log->x, log->y
        );
        pico_assert(TEX != NULL);
        //SDL_SetTextureBlendMode(TEX, SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(REN, TEX);
        SDL_RenderSetClipRect(REN, &S.view.clip);
        _out3_:
    }
}
