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
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))

static SDL_Window*  WIN;
static int FS = 0;          // fullscreen pending (ignore RESIZED event)

#define REN (SDL_GetRenderer(WIN))

static pico_hash* _pico_hash;

typedef struct Pico_Panel {
    int          alpha;
    Pico_Rect    crop;
    struct {
        Pico_Dim phy;
        Pico_Dim log;
    }            dim;
    int          grid;
    char*        name;
    Pico_Pos     pos;
    SDL_Texture* tex;
} Pico_Panel;

Pico_Panel _ctx = {
    0xFF,
    {0, 0, 0, 0},
    { PICO_DIM_PHY, PICO_DIM_LOG },
    1,
    NULL,
    {0, 0},
    NULL,
};

static struct {
    struct {
        Pico_Anchor pos;
        Pico_Anchor rotate;
    } anchor;
    int angle;
    Pico_Rect clip;
    struct {
        Pico_Color clear;
        Pico_Color draw;
    } color;
    Pico_Rect crop;
    Pico_Panel* panel;
    struct {
        int x;
        Pico_Pos cur;
    } cursor;
    int expert;
    Pico_Flip flip;
    struct {
        TTF_Font* ttf;
        int h;
    } font;
    int fullscreen;
    PICO_STYLE style;
    Pico_Pct scale;
} S = {
    { {PICO_CENTER, PICO_MIDDLE}, {PICO_CENTER, PICO_MIDDLE} },
    0,
    {0, 0, 0, 0},
    { {0x00,0x00,0x00,0xFF}, {0xFF,0xFF,0xFF,0xFF} },
    {0, 0, 0, 0},
    &_ctx,
    {0, {0,0}},
    0,
    {0, 0},
    {NULL, 0},
    0,
    PICO_FILL,
    {100, 100},
};

static int _noclip () {
    return (S.clip.w == PICO_CLIP_RESET.w) ||
           (S.clip.h == PICO_CLIP_RESET.h);
}

static int _anchor_x_ext (int x, int w, int a) {
    return x - (a*w)/100;
}

static int _anchor_y_ext (int y, int h, int a) {
    return y - (a*h)/100;
}

static int _anchor_x (int x, int w) {
    return _anchor_x_ext(x, w, S.anchor.pos.x);
}

static int _anchor_y (int y, int h) {
    return _anchor_y_ext(y, h, S.anchor.pos.y);
}

static int X (int x, int w) {
    return _anchor_x(x,w);
}

static int Y (int y, int h) {
    return _anchor_y(y,h);
}

// UTILS

Pico_Dim pico_dim_log (Pico_Pct pct) {
    return pico_dim_ext(pct, S.panel->dim.log);
}

Pico_Dim pico_dim_phy (Pico_Pct pct) {
    return pico_dim_ext(pct, S.panel->dim.phy);
}

Pico_Dim pico_dim_ext (Pico_Pct pct, Pico_Dim d) {
    assert(0 <= pct.x && 0 <= pct.y && "negative dimentions");
    return (Pico_Dim){ (pct.x*d.x)/100, (pct.y*d.y)/100};
}

Pico_Pos pico_pos_log (Pico_Pct pct) {
    return pico_pos_ext (
        pct,
        (Pico_Rect){ 0, 0, S.panel->dim.log.x, S.panel->dim.log.y },
        (Pico_Anchor) {PICO_LEFT, PICO_TOP}
    );
}

Pico_Pos pico_pos_phy (Pico_Pct pct) {
    return pico_pos_ext (
        pct,
        (Pico_Rect){ 0, 0, S.panel->dim.phy.x, S.panel->dim.phy.y },
        (Pico_Anchor) {PICO_LEFT, PICO_TOP}
    );
}

Pico_Pos pico_pos_ext (Pico_Pct pct, Pico_Rect r, Pico_Anchor anc) {
    return (Pico_Pos) {
        _anchor_x_ext(r.x,r.w,anc.x) + (pct.x*r.w)/100,
        _anchor_y_ext(r.y,r.h,anc.y) + (pct.y*r.h)/100
    };
}

int pico_pos_vs_rect (Pico_Pos pt, Pico_Rect r) {
    return pico_pos_vs_rect_ext(pt, r, S.anchor.pos, S.anchor.pos);
}

int pico_pos_vs_rect_ext (Pico_Pos pt, Pico_Rect r, Pico_Anchor ap, Pico_Anchor ar) {
    return pico_rect_vs_rect_ext((Pico_Rect){pt.x, pt.y, 1, 1}, r, ap, ar);
}

Pico_Rect pico_rect_log (Pico_Pct pos, Pico_Pct dim) {
    return pico_rect_ext (
        pos, dim,
        (Pico_Rect){ 0, 0, S.panel->dim.log.x, S.panel->dim.log.y},
        (Pico_Anchor) {PICO_LEFT, PICO_TOP}
    );
}

Pico_Rect pico_rect_phy (Pico_Pct pos, Pico_Pct dim) {
    return pico_rect_ext (
        pos, dim,
        (Pico_Rect){ 0, 0, S.panel->dim.phy.x, S.panel->dim.phy.y },
        (Pico_Anchor) {PICO_LEFT, PICO_TOP}
    );
}

Pico_Rect pico_rect_ext (Pico_Pct pos, Pico_Pct dim, Pico_Rect r, Pico_Anchor anc) {
    Pico_Pos xy = {
        _anchor_x_ext(r.x,r.w,anc.x) + (pos.x*r.w)/100,
        _anchor_y_ext(r.y,r.h,anc.y) + (pos.y*r.h)/100
    };
    Pico_Dim wh = pico_dim_ext(dim, (Pico_Dim){r.w,r.h});
    return (Pico_Rect) { xy.x,xy.y, wh.x,wh.y };
}

int pico_rect_vs_rect (Pico_Rect r1, Pico_Rect r2) {
    return pico_rect_vs_rect_ext(r1, r2, S.anchor.pos, S.anchor.pos);
}

int pico_rect_vs_rect_ext (Pico_Rect r1, Pico_Rect r2, Pico_Anchor a1, Pico_Anchor a2) {
    assert(S.angle == 0 && "rotation angle != 0");
    r1.x = _anchor_x_ext(r1.x, r1.w, a1.x);
    r1.y = _anchor_y_ext(r1.y, r1.h, a1.y);
    r2.x = _anchor_x_ext(r2.x, r2.w, a2.x);
    r2.y = _anchor_y_ext(r2.y, r2.h, a2.y);
    return SDL_HasIntersection(&r1, &r2);
}

// INIT

void pico_init (int on) {
    if (on) {
        _pico_hash = pico_hash_create(PICO_HASH);
        pico_assert(0 == SDL_Init(SDL_INIT_VIDEO));
        WIN = SDL_CreateWindow (
            PICO_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            S.panel->dim.phy.x, S.panel->dim.phy.y,
            (SDL_WINDOW_SHOWN /*| SDL_WINDOW_RESIZABLE*/)
        );
        pico_assert(WIN != NULL);

        SDL_CreateRenderer(WIN, -1, SDL_RENDERER_ACCELERATED/*|SDL_RENDERER_PRESENTVSYNC*/);
        //SDL_CreateRenderer(WIN, -1, SDL_RENDERER_SOFTWARE);

        pico_assert(REN != NULL);
        SDL_SetRenderDrawBlendMode(REN, SDL_BLENDMODE_BLEND);

        TTF_Init();
        Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 1024);

        pico_set_dim_log(S.panel->dim.log);
        pico_set_font(NULL, 0);

        SDL_PumpEvents();
        SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
        SDL_SetWindowResizable(WIN, 1);
    } else {
        if (S.font.ttf != NULL) {
            TTF_CloseFont(S.font.ttf);
        }
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
                    pico_set_dim_phy((Pico_Dim){e->window.data1,e->window.data2});
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
#if 0
                case SDLK_0: {
                    pico_set_zoom((Pico_Dim){0, 0});
                    pico_set_scroll((Pico_Pos){0, 0});
                    break;
                }
                case SDLK_MINUS: {
                    pico_set_zoom ((Pico_Dim) {
                        S.panel->zoom.x - 10,
                        S.panel->zoom.y - 10,
                    });
                    break;
                }
                case SDLK_EQUALS: {
                    pico_set_zoom ((Pico_Dim) {
                        S.panel->zoom.x + 10,
                        S.panel->zoom.y + 10,
                    });
                    break;
                }
                case SDLK_LEFT: {
                    pico_set_scroll ((Pico_Pos) {
                        S.panel->scroll.x - MAX(1, (100+S.panel->zoom.x)/20),
                        S.panel->scroll.y
                    });
                    break;
                }
                case SDLK_RIGHT: {
                    pico_set_scroll ((Pico_Pos) {
                        S.panel->scroll.x + MAX(1, (100+S.panel->zoom.x)/20),
                        S.panel->scroll.y
                    });
                    break;
                }
                case SDLK_UP: {
                    pico_set_scroll ((Pico_Pos) {
                        S.panel->scroll.x,
                        S.panel->scroll.y - MAX(1, (100+S.panel->zoom.y)/20)
                    });
                    break;
                }
                case SDLK_DOWN: {
                    pico_set_scroll ((Pico_Pos) {
                        S.panel->scroll.x,
                        S.panel->scroll.y + MAX(1, (100+S.panel->zoom.y)/20)
                    });
                    break;
                }
#endif
                case SDLK_g: {
                    pico_set_grid(!S.panel->grid);
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
        case SDL_MOUSEMOTION: {
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

static void _pico_output_present (int force, Pico_Panel* panel);

void pico_output_clear (void) {
    Pico_Color clear = S.color.clear;
    SDL_SetRenderDrawColor(REN, clear.r, clear.g, clear.b, clear.a);
    if (_noclip()) {
        SDL_RenderClear(REN);
    } else {
        SDL_Rect r;
        SDL_RenderGetClipRect(REN, &r);
        SDL_RenderFillRect(REN, &r);
    }
    Pico_Color draw = S.color.draw;
    SDL_SetRenderDrawColor(REN, draw.r, draw.g, draw.b, draw.a);
    _pico_output_present(0, S.panel);
}

static SDL_Texture* _draw_aux (int w, int h) {
    SDL_Texture* aux = SDL_CreateTexture (
        REN, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
        w, h
    );
    SDL_SetTextureBlendMode(aux, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(REN, aux);
    SDL_SetRenderDrawColor(REN, 0, 0, 0, 0);   // transparent
    SDL_RenderClear(REN);
    SDL_RenderFillRect(REN, NULL);
    SDL_SetRenderDrawColor (REN,
        S.color.draw.r,
        S.color.draw.g,
        S.color.draw.b,
        S.color.draw.a
    );
    return aux;
}

static void _pico_output_draw_tex (Pico_Pos pos, SDL_Texture* tex, Pico_Dim dim);

void pico_output_draw_buffer (Pico_Pos pos, const Pico_Color buffer[], Pico_Dim dim) {
    SDL_Surface* sfc = SDL_CreateRGBSurfaceWithFormatFrom (
        (void*)buffer, dim.x, dim.y,
        32, 4*dim.x, SDL_PIXELFORMAT_RGBA32
    );
    SDL_Texture *aux = SDL_CreateTextureFromSurface(REN, sfc);
    _pico_output_draw_tex(pos, aux, dim);
    SDL_FreeSurface(sfc);
    SDL_DestroyTexture(aux);
}

static void _pico_output_draw_tex (Pico_Pos pos, SDL_Texture* tex, Pico_Dim dim) {
    Pico_Rect rct;
    SDL_QueryTexture(tex, NULL, NULL, &rct.w, &rct.h);

    Pico_Rect crp = S.crop;
    if (S.crop.w == 0) {
        crp.w = rct.w;
    }
    if (S.crop.h == 0) {
        crp.h = rct.h;
    }

    if (dim.x==PICO_DIM_KEEP.x && dim.y==PICO_DIM_KEEP.y) {
        // normal image size
        rct.w = crp.w;  // (or copy from crop)
        rct.h = crp.h;  // (or copy from crop)
    } else if (dim.x == 0) {
        // adjust w based on h
        rct.w = rct.w * (dim.y / (float)rct.h);
        rct.h = dim.y;
    } else if (dim.y == 0) {
        // adjust h based on w
        rct.h = rct.h * (dim.x / (float)rct.w);
        rct.w = dim.x;
    } else {
        rct.w = dim.x;
        rct.h = dim.y;
    }

    // SCALE
    rct.w = (S.scale.x*rct.w)/100; // * GRAPHICS_SET_SCALE_W;
    rct.h = (S.scale.y*rct.h)/100; // * GRAPHICS_SET_SCALE_H;

    // ANCHOR / PAN
    rct.x = X(pos.x, rct.w);
    rct.y = Y(pos.y, rct.h);

    // ROTATE
    Pico_Pos rot = {
        (S.anchor.rotate.x*rct.w)/100,
        (S.anchor.rotate.y*rct.h)/100
    };

    SDL_RenderCopyEx(REN, tex,
        &crp, &rct,
        S.angle + (S.flip.x && S.flip.y ? 180 : 0),
        &rot,
        S.flip.y ? SDL_FLIP_VERTICAL : (S.flip.x ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE)
    );
    _pico_output_present(0, S.panel);
}

void pico_output_draw_image (Pico_Pos pos, const char* path) {
    pico_output_draw_image_ext(pos, path, PICO_DIM_KEEP);
}

void pico_output_draw_image_ext (Pico_Pos pos, const char* path, Pico_Dim dim) {
    SDL_Texture* tex = (SDL_Texture*)pico_hash_get(_pico_hash, path);
    if (tex == NULL) {
        tex = IMG_LoadTexture(REN, path);
        pico_hash_add(_pico_hash, path, tex);
    }
    pico_assert(tex != NULL);
    _pico_output_draw_tex(pos, tex, dim);
}

// TODO: Test me for flip and rotate
void pico_output_draw_line (Pico_Pos p1, Pico_Pos p2) {
    Pico_Pos pos = {
        X(SDL_min(p1.x,p2.x), 1),
        Y(SDL_min(p1.y,p2.y), 1),
    };
    SDL_Rect clip;
    SDL_RenderGetClipRect(REN, &clip);
    SDL_Texture* aux = _draw_aux (
        SDL_abs(p1.x-p2.x) + 1,
        SDL_abs(p1.y-p2.y) + 1
    );
    SDL_RenderDrawLine(REN, p1.x-pos.x,p1.y-pos.y, p2.x-pos.x,p2.y-pos.y);
    SDL_SetRenderTarget(REN, S.panel->tex);
    SDL_RenderSetClipRect(REN, &clip);
    Pico_Anchor anc = S.anchor.pos;
    S.anchor.pos = (Pico_Anchor){PICO_LEFT, PICO_TOP};
    _pico_output_draw_tex(pos, aux, PICO_DIM_KEEP);
    S.anchor.pos = anc;
    SDL_DestroyTexture(aux);
}

void pico_output_draw_pixel (Pico_Pos pos) {
    SDL_RenderDrawPoint(REN, X(pos.x,1), Y(pos.y,1));
    _pico_output_present(0, S.panel);
}

void pico_output_draw_pixels (const Pico_Pos* apos, int count) {
    Pico_Pos vec[count];
    for (int i=0; i<count; i++) {
        vec[i].x = X(apos[i].x, 1);
        vec[i].y = Y(apos[i].y, 1);
    }
    SDL_RenderDrawPoints(REN, vec, count);
    _pico_output_present(0, S.panel);
}

// TODO: Test me for flip and rotate
void pico_output_draw_rect (Pico_Rect rect) {
    Pico_Pos pos = {rect.x, rect.y};
    SDL_Rect clip;
    SDL_RenderGetClipRect(REN, &clip);
    SDL_Texture* aux = _draw_aux(rect.w, rect.h);
    rect.x = 0;
    rect.y = 0;
    switch (S.style) {
        case PICO_FILL:
            SDL_RenderFillRect(REN, &rect);
            break;
        case PICO_STROKE:
            SDL_RenderDrawRect(REN, &rect);
            break;
    }
    SDL_SetRenderTarget(REN, S.panel->tex);
    SDL_RenderSetClipRect(REN, &clip);
    _pico_output_draw_tex(pos, aux, PICO_DIM_KEEP);
    SDL_DestroyTexture(aux);
}

// TODO: Test me for flip and rotate
void pico_output_draw_tri (Pico_Rect rect) {
    Pico_Pos pos = {rect.x, rect.y};
    SDL_Rect clip;
    SDL_RenderGetClipRect(REN, &clip);
    SDL_Texture* aux = _draw_aux(rect.w, rect.h);
    switch (S.style) {
        case PICO_FILL:
            filledTrigonRGBA (REN,
                0, 0,
                0, rect.h - 1,
                rect.w - 1, rect.h - 1,
                S.color.draw.r, S.color.draw.g, S.color.draw.b, S.color.draw.a
            );
            break;
        case PICO_STROKE:
            trigonRGBA (REN,
                0, 0,
                0, rect.h - 1,
                rect.w - 1, rect.h - 1,
                S.color.draw.r, S.color.draw.g, S.color.draw.b, S.color.draw.a
            );
            break;
    }
    SDL_SetRenderTarget(REN, S.panel->tex);
    SDL_RenderSetClipRect(REN, &clip);
    _pico_output_draw_tex(pos, aux, PICO_DIM_KEEP);
    SDL_DestroyTexture(aux);
}

// TODO: Test me for flip and rotate
void pico_output_draw_oval (Pico_Rect rect) {
    Pico_Pos pos = {rect.x, rect.y};
    SDL_Rect clip;
    SDL_RenderGetClipRect(REN, &clip);
    SDL_Texture* aux = _draw_aux(rect.w, rect.h);
    switch (S.style) {
        case PICO_FILL:
            filledEllipseRGBA (REN,
                rect.w/2, rect.h/2, rect.w/2, rect.h/2,
                S.color.draw.r, S.color.draw.g, S.color.draw.b, S.color.draw.a
            );
            break;
        case PICO_STROKE:
            ellipseRGBA (REN,
                rect.w/2, rect.h/2, rect.w/2, rect.h/2,
                S.color.draw.r, S.color.draw.g, S.color.draw.b, S.color.draw.a
            );
            break;
    }
    SDL_SetRenderTarget(REN, S.panel->tex);
    SDL_RenderSetClipRect(REN, &clip);
    _pico_output_draw_tex(pos, aux, PICO_DIM_KEEP);
    SDL_DestroyTexture(aux);
}

void pico_output_draw_poly (const Pico_Pos* apos, int count) {
    Sint16 ax[count], ay[count];
    int minx = INT_MAX, maxx = INT_MIN, miny = INT_MAX, maxy = INT_MIN;
    for (int i = 0; i < count; i++) {
        ax[i] = apos[i].x;
        ay[i] = apos[i].y;
        minx = SDL_min(ax[i], minx);
        maxx = SDL_max(ax[i], maxx);
        miny = SDL_min(ay[i], miny);
        maxy = SDL_max(ay[i], maxy);
    }
    for (int i = 0; i < count; i++) {
        ax[i] -= minx;
        ay[i] -= miny;
    }

    Pico_Pos pos = { X(minx,1), Y(miny,1) };
    SDL_Rect clip;
    SDL_RenderGetClipRect(REN, &clip);
    SDL_Texture* aux = _draw_aux(maxx-minx+1, maxy-miny+1);
    switch (S.style) {
        case PICO_FILL:
            filledPolygonRGBA(REN,
                ax, ay, count,
                S.color.draw.r, S.color.draw.g, S.color.draw.b, S.color.draw.a
            );
            break;
        case PICO_STROKE:
            polygonRGBA(REN,
                ax, ay, count,
                S.color.draw.r, S.color.draw.g, S.color.draw.b, S.color.draw.a
            );
            break;
    }
    SDL_SetRenderTarget(REN, S.panel->tex);
    SDL_RenderSetClipRect(REN, &clip);
    Pico_Anchor anc = S.anchor.pos;
    S.anchor.pos = (Pico_Anchor){PICO_LEFT, PICO_TOP};
    _pico_output_draw_tex(pos, aux, PICO_DIM_KEEP);
    S.anchor.pos = anc;
    SDL_DestroyTexture(aux);
}

void pico_output_draw_text (Pico_Pos pos, const char* text) {
    pico_output_draw_text_ext(pos, text, PICO_DIM_KEEP);
}

void pico_output_draw_text_ext (Pico_Pos pos, const char* text, Pico_Dim dim) {
    if (text[0] == '\0') return;

    pico_assert(S.font.ttf != NULL);
    SDL_Surface* sfc = TTF_RenderText_Blended(S.font.ttf, text, S.color.draw);
    pico_assert(sfc != NULL);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(REN, sfc);
    pico_assert(tex != NULL);
    _pico_output_draw_tex(pos, tex, dim);
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(sfc);
}

static void pico_output_draw_fmt_va (Pico_Pos pos, const char* fmt, Pico_Dim dim, va_list args) {
    static char text[1024];
    vsprintf(text, fmt, args);
    pico_output_draw_text_ext(pos, text, dim);
}

void pico_output_draw_fmt (Pico_Pos pos, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    pico_output_draw_fmt_va(pos, fmt, PICO_DIM_KEEP, args);
    va_end(args);
}

void pico_output_draw_fmt_ext (Pico_Pos pos, Pico_Dim dim, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    pico_output_draw_fmt_va(pos, fmt, dim, args);
    va_end(args);
}

static void _show_grid (Pico_Panel* panel) {
    if (!panel->grid) return;

    SDL_SetRenderDrawColor(REN, 0x77, 0x77, 0x77, 0x77);

    if ((panel->dim.phy.x % panel->crop.w == 0) && (panel->crop.w < panel->dim.phy.x)) {
        for (int i=0; i<=panel->dim.phy.x; i+=(panel->dim.phy.x/panel->crop.w)) {
            SDL_RenderDrawLine(REN, panel->pos.x+i, panel->pos.y, panel->pos.x+i, panel->dim.phy.y);
        }
    }

    if ((panel->dim.phy.y % panel->crop.h == 0) && (panel->crop.h < panel->dim.phy.y)) {
        for (int j=0; j<=panel->dim.phy.y; j+=(panel->dim.phy.y/panel->crop.h)) {
            SDL_RenderDrawLine(REN, panel->pos.x, panel->pos.y+j, panel->dim.phy.x, panel->pos.y+j);
        }
    }

    Pico_Color c = S.color.draw;
    SDL_SetRenderDrawColor(REN, c.r, c.g, c.b, c.a);
}

static void _pico_output_present (int force, Pico_Panel* panel) {
    if (S.expert && !force) return;

    SDL_Rect clip;
    SDL_RenderGetClipRect(REN, &clip);

    SDL_Texture* up = (panel == &_ctx) ? NULL : _ctx.tex;

    SDL_SetRenderTarget(REN, up);
    SDL_SetTextureAlphaMod(panel->tex, panel->alpha);

    if (panel->name == NULL) {
        SDL_SetRenderDrawColor(REN, 0x77, 0x77, 0x77, 0x77);
        SDL_RenderClear(REN);
        Pico_Color c = S.color.draw;
        SDL_SetRenderDrawColor(REN, c.r, c.g, c.b, c.a);
    }

    {
        // intersection for the source rectangle
        int sx = MAX(0, panel->crop.x);                     // 0
        int sy = MAX(0, panel->crop.y);                     // 0
        int sw = MIN(panel->dim.log.x-sx, panel->crop.w);   // 50
        int sh = MIN(panel->dim.log.y-sy, panel->crop.h);   // 50
        SDL_Rect src = { sx, sy, sw, sh };

        // offset is the diff bw clipped start and the intended crop start
        #define xx panel->dim.phy.x / panel->crop.w   // scale x/y
        #define yy panel->dim.phy.y / panel->crop.h   // do not use parens
        SDL_Rect dst = {
            panel->pos.x + (sx - panel->crop.x) * xx,
            panel->pos.y + (sy - panel->crop.y) * yy,
            src.w * xx,
            src.h * yy,
        };

        SDL_RenderCopy(REN, panel->tex, &src, &dst);
    }

    if (panel->name == NULL) {
        _show_grid(panel);
    }

    if (panel->name == NULL) {
        SDL_RenderPresent(REN);
    }

    if (panel->name != NULL) {
        _pico_output_present(force, &_ctx);
    }

    SDL_SetRenderTarget(REN, panel->tex);
    SDL_RenderSetClipRect(REN, &clip);
}

void pico_output_present (void) {
    _pico_output_present(1, &_ctx);
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
        (Pico_Rect){0,0,S.panel->crop.w,S.panel->crop.h}
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

static void _pico_output_write_aux (const char* text, int isln) {
    if (strlen(text) == 0) {
        if (isln) {
            S.cursor.cur.x = S.cursor.x;
            S.cursor.cur.y += S.font.h;
        }
        return;
    }

    pico_assert(S.font.ttf != NULL);
    SDL_Surface* sfc = TTF_RenderText_Blended (
        S.font.ttf, text,
        (Pico_Color) { S.color.draw.r, S.color.draw.g,
                       S.color.draw.b, S.color.draw.a }
    );
    pico_assert(sfc != NULL);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(REN, sfc);
    pico_assert(tex != NULL);

    int w, h;
    TTF_SizeText(S.font.ttf, text, &w,&h);
    Pico_Rect rct = { X(S.cursor.cur.x,0),Y(S.cursor.cur.y,0), w,h };
    SDL_RenderCopy(REN, tex, NULL, &rct);
    _pico_output_present(0, S.panel);

    S.cursor.cur.x += w;
    if (isln) {
        S.cursor.cur.x = S.cursor.x;
        S.cursor.cur.y += S.font.h;
    }

    SDL_DestroyTexture(tex);
    SDL_FreeSurface(sfc);
}

void pico_output_write (const char* text) {
    _pico_output_write_aux(text, 0);
}

void pico_output_writeln (const char* text) {
    _pico_output_write_aux(text, 1);
}


// STATE

// GET

Pico_Anchor pico_get_anchor_pos (void) {
    return S.anchor.pos;
}

Pico_Anchor pico_get_anchor_rotate (void) {
    return S.anchor.rotate;
}

Pico_Rect pico_get_clip (void) {
    return S.clip;
}

Pico_Color pico_get_color_clear (void) {
    return S.color.clear;
}

Pico_Color pico_get_color_draw (void) {
    return S.color.draw;
}

Pico_Pos pico_get_cursor (void) {
    return S.cursor.cur;
}

int pico_get_expert (void) {
    return S.expert;
}

Pico_Flip pico_get_flip (void) {
    return S.flip;
}

const char* pico_get_font (void) {
    return TTF_FontFaceFamilyName(S.font.ttf);
}

int pico_get_fullscreen (void) {
    return S.fullscreen;
}

int pico_get_grid (void) {
    return S.panel->grid;
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
    pos->x = pos->x * S.panel->crop.w / S.panel->dim.phy.x;
    pos->y = pos->y * S.panel->crop.h / S.panel->dim.phy.y;
    //pos->x += S.panel->scroll.x;
    //pos->y += S.panel->scroll.y;

    return masks & SDL_BUTTON(button);
}

Pico_Rect pico_get_crop (void) {
    return S.crop;
}

int pico_get_rotate (void) {
    return S.angle;
}

Pico_Pct pico_get_scale (void) {
    return S.scale;
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

Pico_Dim pico_get_dim_text (const char* text) {
    if (text[0] == '\0') {
        return (Pico_Dim){0, 0};
    }

    SDL_Surface* sfc = TTF_RenderText_Blended(S.font.ttf, text,
                                              (Pico_Color){0,0,0,255});
    pico_assert(sfc != NULL);
    Pico_Dim dim = {sfc->w, sfc->h};
    SDL_FreeSurface(sfc);
    return dim;
}

Pico_Dim pico_get_dim_phy (void) {
    return S.panel->dim.phy;
}

Pico_Dim pico_get_dim_log (void) {
    return S.panel->dim.log;
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

// SET

void pico_set_alpha (int alpha) {
    S.panel->alpha = alpha;
    _pico_output_present(0, S.panel);
}

void pico_set_anchor_pos (Pico_Anchor anchor) {
    S.anchor.pos = anchor;
}

void pico_set_anchor_rotate (Pico_Anchor anchor) {
    S.anchor.rotate = anchor;
}

void pico_set_clip (Pico_Rect clip) {
    S.clip = clip;
    if (_noclip()) {
        clip.w = S.panel->crop.w;
        clip.h = S.panel->crop.h;
    } else {
        clip.x = X(clip.x, clip.w);
        clip.y = Y(clip.y, clip.h);
    }
    SDL_RenderSetClipRect(REN, &clip);
}

void pico_set_color_clear (Pico_Color clr) {
    S.color.clear = clr;
}

void pico_set_color_draw  (Pico_Color clr) {
    S.color.draw = clr;
    SDL_SetRenderDrawColor(REN, clr.r, clr.g, clr.b, clr.a);
}

void pico_set_panel (char* name) {
    if (name == NULL) {
        S.panel = &_ctx;
    } else {
        Pico_Panel* panel = (Pico_Panel*)pico_hash_get(_pico_hash, name);
        if (panel == NULL) {
            panel = malloc(sizeof(Pico_Panel));
            *panel = (Pico_Panel) {
                0xFF,
                {0, 0, 0, 0},
                { {0,0}, {0,0} },
                1,
                name,
                {0, 0},
                NULL,
            };
            pico_hash_add(_pico_hash, name, panel);
        }
        pico_assert(panel != NULL);
        S.panel = panel;
    }
}

void pico_set_crop (Pico_Rect crop) {
    //S.crop = crop;
    S.panel->crop = crop;
}

void pico_set_cursor (Pico_Pos pos) {
    S.cursor.cur = pos;
    S.cursor.x   = pos.x;
}

void pico_set_dim_phy (Pico_Dim dim) {
    S.panel->dim.phy = dim;
    if (S.panel->name != NULL) {
        return;
    }
    assert(!S.fullscreen);
    SDL_SetWindowSize(WIN, dim.x, dim.y);
    SDL_RenderSetClipRect(REN, &S.panel->crop);
}

void pico_set_dim_log (Pico_Dim dim) {
    S.panel->dim.log = dim;
    S.panel->crop = (Pico_Rect) { 0,0, dim.x,dim.y };
    if (S.panel->tex != NULL) {
        SDL_DestroyTexture(S.panel->tex);
    }
    S.panel->tex = SDL_CreateTexture (
        REN, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
        dim.x, dim.y
    );
    pico_assert(S.panel->tex != NULL);
    SDL_SetTextureBlendMode(S.panel->tex, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(REN, S.panel->tex);
    pico_output_clear();

    // TODO: need to init w/ explicit SetClip to save w/h
    //       do not pass NULL, GetClip would also return w=0,h=0
    SDL_Rect clip = { 0, 0, dim.x, dim.y };
    SDL_RenderSetClipRect(REN, &clip);
}

void pico_set_expert (int on) {
    S.expert = on;
}

void pico_set_flip (Pico_Flip flip) {
    S.flip = flip;
}

void pico_set_fullscreen (int on) {
    static Pico_Dim _old;
    if ((on && S.fullscreen) || (!on && !S.fullscreen)) {
        return;
    }
    FS = 1;

    Pico_Dim new;
    if (on) {
        _old = S.panel->dim.phy;
        pico_assert(0 == SDL_SetWindowFullscreen(WIN, SDL_WINDOW_FULLSCREEN_DESKTOP));
        pico_input_delay(50);    // TODO: required for some reason
        SDL_GetWindowSize(WIN, &new.x, &new.y);
    } else {
        pico_assert(0 == SDL_SetWindowFullscreen(WIN, 0));
        new = _old;
    }

    S.fullscreen = 0;           // cannot set_dim_win with fullscreen on
    pico_set_dim_phy(new);
    S.fullscreen = on;
}

void pico_set_font (const char* file, int h) {
    if (h == 0) {
        h = MAX(8, S.panel->dim.log.y/10);
    }
    S.font.h = h;

    if (S.font.ttf != NULL) {
        TTF_CloseFont(S.font.ttf);
    }
    if (file == NULL) {
        SDL_RWops* rw = SDL_RWFromConstMem(pico_tiny_ttf, pico_tiny_ttf_len);
        S.font.ttf = TTF_OpenFontRW(rw, 1, S.font.h);
    } else {
        S.font.ttf = TTF_OpenFont(file, S.font.h);
    }
    pico_assert(S.font.ttf != NULL);
}

void pico_set_grid (int on) {
    S.panel->grid = on;
    _pico_output_present(0, &_ctx);
}

void pico_set_pos_phy (Pico_Pos pos) {
    if (S.panel->name == NULL) {
        assert(0 && "TODO: window position");
    }
    S.panel->pos = (Pico_Pos) {
        X(pos.x, S.panel->dim.phy.x),
        Y(pos.y, S.panel->dim.phy.y),
    };
}

void pico_set_rotate (int angle) {
    S.angle = angle;
}

void pico_set_scale (Pico_Pct scale) {
    // TODO: checks???
    S.scale = scale;
}

void pico_set_show (int on) {
    if (on) {
        SDL_ShowWindow(WIN);
        _pico_output_present(0, &_ctx);
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
