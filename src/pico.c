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

#include "dir.h"
#include "hash.h"
#include "pico.h"

#define SDL_ANY PICO_ANY
#define MAX(x,y) ((x) > (y) ? (x) : (y))

static SDL_Window*  WIN;
static SDL_Texture* TEX;

#define REN (SDL_GetRenderer(WIN))

#define X(v,w) (hanchor(v,w) - S.scroll.x)
#define Y(v,h) (vanchor(v,h) - S.scroll.y)

#define PHY ({Pico_Dim phy; SDL_GetWindowSize(WIN, &phy.x, &phy.y); phy;})

static pico_hash* _pico_hash;

static struct {
    struct {
        Pico_Anchor draw;
        Pico_Anchor rotate;
    } anchor;
    struct {
        Pico_Color clear;
        Pico_Color draw;
    } color;
    struct {
        int x;
        Pico_Pos cur;
    } cursor;
    int expert;
    struct {
        TTF_Font* ttf;
        int h;
    } font;
    int grid;
    Pico_Rect crop;
    Pico_Pos scroll;
    struct {
        Pico_Dim org;
        Pico_Dim cur;
    } size;
    PICO_STYLE style;
    Pico_Flip flip;
    int angle;
    Pico_Pct zoom;
    Pico_Pct scale;
} S = {
    { {PICO_CENTER, PICO_MIDDLE}, {PICO_CENTER, PICO_MIDDLE} },
    { {0x00,0x00,0x00,0xFF}, {0xFF,0xFF,0xFF,0xFF} },
    {0, {0,0}},
    0,
    {NULL, 0},
    1,
    {0,0,0,0},
    {0, 0},
    { {0,0}, {0,0} },
    PICO_FILL,
    {0, 0},
    0,
    {100, 100},
    {100, 100}
};

static int hanchor (int x, int w) {
    return x - (S.anchor.draw.x*w)/100;
}

static int vanchor (int y, int h) {
    return y - (S.anchor.draw.y*h)/100;
}

// UTILS

Pico_Dim pico_dim (Pico_Pct pct) {
    return pico_dim_ext(S.size.org, pct);
}

Pico_Dim pico_dim_ext (Pico_Dim d, Pico_Pct pct) {
    assert(0 <= pct.x && 0 <= pct.y && "negative dimentions");
    return (Pico_Dim){ (pct.x*d.x)/100, (pct.y*d.y)/100};
}

int pico_pos_vs_rect (Pico_Pos pt, Pico_Rect r) {
    return pico_pos_vs_rect_ext(pt, S.anchor.draw, r, S.anchor.draw);
}

int pico_pos_vs_rect_ext (Pico_Pos pt, Pico_Anchor ap, Pico_Rect r, Pico_Anchor ar) {
    return pico_rect_vs_rect_ext((Pico_Rect){pt.x, pt.y, 1, 1}, ap, r, ar);
}

Pico_Pos pico_pos (Pico_Pct pct) {
    Pico_Anchor old = S.anchor.draw;
    S.anchor.draw = (Pico_Anchor) {PICO_CENTER, PICO_MIDDLE};
    Pico_Pos pt = pico_pos_ext (
        (Pico_Rect){ S.size.org.x/2, S.size.org.y/2, S.size.org.x, S.size.org.y},
        pct 
    );
    S.anchor.draw = old;
    return pt;
}

Pico_Pos pico_pos_ext (Pico_Rect r, Pico_Pct pct) {
    return (Pico_Pos) {
        hanchor(r.x,r.w) + (pct.x*r.w)/100,
        vanchor(r.y,r.h) + (pct.y*r.h)/100
    };
}

int pico_rect_vs_rect (Pico_Rect r1, Pico_Rect r2) {
    return pico_rect_vs_rect_ext(r1, S.anchor.draw, r2, S.anchor.draw);
}

int pico_rect_vs_rect_ext (Pico_Rect r1, Pico_Anchor a1, Pico_Rect r2, Pico_Anchor a2) {
    assert(S.angle == 0 && "rotation angle != 0");
    Pico_Anchor old = S.anchor.draw;
    S.anchor.draw = a1;
    r1.x = hanchor(r1.x, r1.w);
    r1.y = vanchor(r1.y, r1.h);
    S.anchor.draw = a2;
    r2.x = hanchor(r2.x, r2.w);
    r2.y = vanchor(r2.y, r2.h);
    S.anchor.draw = old;
    return SDL_HasIntersection(&r1, &r2);
}

// INIT

void pico_init (int on) {
    char* dir = pico_dir_exe_get();
    assert(dir!=NULL && "cannot determine execution path");
    assert(chdir(dir)==0 && "cannot determine execution path");
    if (on) {
        _pico_hash = pico_hash_create(PICO_HASH);
        pico_assert(0 == SDL_Init(SDL_INIT_VIDEO));
        WIN = SDL_CreateWindow (
            PICO_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            PICO_DIM_PHY.x, PICO_DIM_PHY.y, SDL_WINDOW_SHOWN
        );
        pico_assert(WIN != NULL);

        SDL_CreateRenderer(WIN, -1, SDL_RENDERER_ACCELERATED);
        //SDL_CreateRenderer(WIN, -1, SDL_RENDERER_SOFTWARE);
        pico_assert(REN != NULL);
        SDL_SetRenderDrawBlendMode(REN, SDL_BLENDMODE_BLEND);

        TTF_Init();
        Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 1024);

        pico_set_size(PICO_DIM_PHY, PICO_DIM_LOG);
        pico_set_font(NULL, 0);
        pico_output_clear();
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

        case SDL_KEYDOWN: {
            const unsigned char* state = SDL_GetKeyboardState(NULL);
            if (!state[SDL_SCANCODE_LCTRL] && !state[SDL_SCANCODE_RCTRL]) {
                break;
            }
            switch (e->key.keysym.sym) {
                case SDLK_0: {
                    pico_set_zoom((Pico_Pct){100, 100});
                    pico_set_scroll((Pico_Pos){0, 0});
                    break;
                }
                case SDLK_MINUS: {
                    pico_set_zoom ((Pico_Pct) {
                        MAX(1, S.zoom.x-10),
                        MAX(1, S.zoom.y-10)
                    });
                    break;
                }
                case SDLK_EQUALS: {
                    pico_set_zoom ((Pico_Pct) {
                        S.zoom.x + 10,
                        S.zoom.y + 10
                    });
                    break;
                }
                case SDLK_LEFT: {
                    pico_set_scroll ((Pico_Pos) {
                        S.scroll.x - MAX(1, S.size.cur.x/20),
                        S.scroll.y
                    });
                    break;
                }
                case SDLK_RIGHT: {
                    pico_set_scroll ((Pico_Pos) {
                        S.scroll.x + MAX(1, S.size.cur.x/20),
                        S.scroll.y
                    });
                    break;
                }
                case SDLK_UP: {
                    pico_set_scroll ((Pico_Pos) {
                        S.scroll.x,
                        S.scroll.y - MAX(1, S.size.cur.y/20)
                    });
                    break;
                }
                case SDLK_DOWN: {
                    pico_set_scroll ((Pico_Pos) {
                        S.scroll.x,
                        S.scroll.y + MAX(1, S.size.cur.y/20)
                    });
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

#if 0
        case SDL_WINDOWEVENT: {
            if (e->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                pico_set_size((SDL_Point){e->window.data1,e->window.data2});
                return 0;
                break;
            }
        }
#endif
        default:
            // others are not handled automatically
            break;
    }

    if (xp == (int)e->type) {
        // OK
    } else if (xp == SDL_ANY) {
        // MAYBE
        if (e->type==SDL_KEYDOWN || e->type==SDL_KEYUP || e->type==SDL_MOUSEBUTTONDOWN ||
            e->type==SDL_MOUSEBUTTONUP || e->type==SDL_MOUSEMOTION ||
            e->type==SDL_QUIT) {
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
        case SDL_MOUSEMOTION:
            e->button.x = (e->button.x + S.scroll.x);
            e->button.y = (e->button.y + S.scroll.y);
            break;
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

static void _pico_output_clear (void) {
    SDL_SetRenderDrawColor (REN,
        S.color.clear.r,
        S.color.clear.g,
        S.color.clear.b,
        S.color.clear.a
    );
    SDL_RenderClear(REN);
    SDL_SetRenderDrawColor (REN,
        S.color.draw.r,
        S.color.draw.g,
        S.color.draw.b,
        S.color.draw.a
    );
}

static void _pico_output_present (int force);

void pico_output_clear (void) {
    _pico_output_clear();
    _pico_output_present(0);
}

static void _pico_output_draw_tex (Pico_Pos pos, SDL_Texture* tex, Pico_Dim size);

void pico_output_draw_buffer (Pico_Pos pos, const Pico_Color buffer[], Pico_Dim size) {
    SDL_Surface* sfc = SDL_CreateRGBSurfaceWithFormatFrom((void*)buffer, size.x, size.y, 32,
                                                          4*size.x, SDL_PIXELFORMAT_RGBA32);
    SDL_Texture *aux = SDL_CreateTextureFromSurface(REN, sfc);

    _pico_output_draw_tex(pos, aux, size);
    SDL_FreeSurface(sfc);
    SDL_DestroyTexture(aux);
}

static void _pico_output_draw_tex (Pico_Pos pos, SDL_Texture* tex, Pico_Dim size) {
    Pico_Rect rct;
    SDL_QueryTexture(tex, NULL, NULL, &rct.w, &rct.h);

    Pico_Rect crp = S.crop;
    if (S.crop.w == 0) {
        crp.w = rct.w;
    }
    if (S.crop.h == 0) {
        crp.h = rct.h;
    }

    if (size.x==0 && size.y==0) {
        // normal image size
        rct.w = crp.w;  // (or copy from crop)
        rct.h = crp.h;  // (or copy from crop)
    } else if (size.x == 0) {
        // adjust w based on h
        rct.w = rct.w * (size.y / (float)rct.h);
        rct.h = size.y;
    } else if (size.y == 0) {
        // adjust h based on w
        rct.h = rct.h * (size.x / (float)rct.w);
        rct.w = size.x;
    } else {
        rct.w = size.x;
        rct.h = size.y;
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
    _pico_output_present(0);
}

void pico_output_draw_image (Pico_Pos pos, const char* path) {
    pico_output_draw_image_ext(pos, path, PICO_SIZE_KEEP);
}

void pico_output_draw_image_ext (Pico_Pos pos, const char* path, Pico_Dim size) {
    SDL_Texture* tex = (SDL_Texture*)pico_hash_get(_pico_hash, path);
    if (tex == NULL) {
        tex = IMG_LoadTexture(REN, path);
        pico_hash_add(_pico_hash, path, tex);
    }
    pico_assert(tex != NULL);

    _pico_output_draw_tex(pos, tex, size);
}

// TODO: Test me for flip and rotate
void pico_output_draw_line (Pico_Pos p1, Pico_Pos p2) {
    Pico_Pos pos = {
        hanchor(SDL_min(p1.x,p2.x),1),
        vanchor(SDL_min(p1.y,p2.y),1)
    };
    SDL_Texture* aux = SDL_CreateTexture (REN,
        SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
        SDL_abs(p1.x-p2.x) + 1, SDL_abs(p1.y-p2.y) + 1
    );

    SDL_SetTextureBlendMode(aux, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(REN, aux);
    Pico_Color clr = S.color.clear;
    S.color.clear = (Pico_Color){0, 0, 0, 0};
    _pico_output_clear();
    S.color.clear = clr;
    SDL_RenderDrawLine(REN, p1.x-pos.x,p1.y-pos.y, p2.x-pos.x,p2.y-pos.y);
    SDL_SetRenderTarget(REN, TEX);
    Pico_Anchor anc = S.anchor.draw;
    S.anchor.draw = (Pico_Anchor){PICO_LEFT, PICO_TOP};
    _pico_output_draw_tex(pos, aux, PICO_SIZE_KEEP);
    S.anchor.draw = anc;
    SDL_DestroyTexture(aux);
}

void pico_output_draw_pixel (Pico_Pos pos) {
    SDL_RenderDrawPoint(REN, X(pos.x,1), Y(pos.y,1) );
    _pico_output_present(0);
}

void pico_output_draw_pixels (const Pico_Pos* apos, int count) {
    Pico_Pos vec[count];
    for (int i=0; i<count; i++) {
        vec[i].x = X(apos[i].x,1);
        vec[i].y = Y(apos[i].y,1);
    }
    SDL_RenderDrawPoints(REN, vec, count);
    _pico_output_present(0);
}

// TODO: Test me for flip and rotate
void pico_output_draw_rect (Pico_Rect rect) {
    Pico_Pos pos = {rect.x, rect.y};
    SDL_Texture* aux = SDL_CreateTexture (REN,
        SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
        rect.w, rect.h
    );

    SDL_SetTextureBlendMode(aux, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(REN, aux);
    Pico_Color clr = S.color.clear;
    S.color.clear = (Pico_Color){0, 0, 0, 0};
    _pico_output_clear();
    S.color.clear = clr;
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
    SDL_SetRenderTarget(REN, TEX);
    _pico_output_draw_tex(pos, aux, PICO_SIZE_KEEP);
    SDL_DestroyTexture(aux);
}

// TODO: Test me for flip and rotate
void pico_output_draw_tri (Pico_Rect rect) {
    Pico_Pos pos = {rect.x, rect.y};
    SDL_Texture* aux = SDL_CreateTexture (REN,
        SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
        rect.w, rect.h
    );

    SDL_SetTextureBlendMode(aux, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(REN, aux);
    Pico_Color clr = S.color.clear;
    S.color.clear = (Pico_Color){0, 0, 0, 0};
    _pico_output_clear();
    S.color.clear = clr;
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
    SDL_SetRenderTarget(REN, TEX);
    _pico_output_draw_tex(pos, aux, PICO_SIZE_KEEP);
    SDL_DestroyTexture(aux);
}

// TODO: Test me for flip and rotate
void pico_output_draw_oval (Pico_Rect rect) {
    Pico_Pos pos = {rect.x, rect.y};
    SDL_Texture* aux = SDL_CreateTexture (
        REN, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
        rect.w, rect.h
    );

    SDL_SetTextureBlendMode(aux, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(REN, aux);
    Pico_Color clr = S.color.clear;
    S.color.clear = (Pico_Color){0, 0, 0, 0};
    _pico_output_clear();
    S.color.clear = clr;
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
    SDL_SetRenderTarget(REN, TEX);
    _pico_output_draw_tex(pos, aux, PICO_SIZE_KEEP);
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

    Pico_Pos pos = {
        hanchor(minx,1),
        vanchor(miny,1)
    };
    SDL_Texture* aux = SDL_CreateTexture (
        REN, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
        maxx - minx + 1, maxy - miny + 1
    );

    SDL_SetTextureBlendMode(aux, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(REN, aux);
    Pico_Color clr = S.color.clear;
    S.color.clear = (Pico_Color){0, 0, 0, 0};
    _pico_output_clear();
    S.color.clear = clr;
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
    SDL_SetRenderTarget(REN, TEX);
    Pico_Anchor anc = S.anchor.draw;
    S.anchor.draw = (Pico_Anchor){PICO_LEFT, PICO_TOP};
    _pico_output_draw_tex(pos, aux, PICO_SIZE_KEEP);
    S.anchor.draw = anc;
    SDL_DestroyTexture(aux);
}

void pico_output_draw_text (Pico_Pos pos, const char* text) {
    pico_output_draw_text_ext(pos, text, PICO_SIZE_KEEP);
}

void pico_output_draw_text_ext (Pico_Pos pos, const char* text, Pico_Dim size) {
    if (text[0] == '\0') return;

    pico_assert(S.font.ttf != NULL);
    SDL_Surface* sfc = TTF_RenderText_Blended(S.font.ttf, text, S.color.draw);
    pico_assert(sfc != NULL);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(REN, sfc);
    pico_assert(tex != NULL);

    _pico_output_draw_tex(pos, tex, size);
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(sfc);
}

static void pico_output_draw_fmt_va (Pico_Pos pos, const char* fmt, Pico_Dim size, va_list args) {
    static char text[1024];
    vsprintf(text, fmt, args);
    pico_output_draw_text_ext(pos, text, size);
}

void pico_output_draw_fmt (Pico_Pos pos, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    pico_output_draw_fmt_va(pos, fmt, PICO_SIZE_KEEP, args);
    va_end(args);
}

void pico_output_draw_fmt_ext (Pico_Pos pos, const char* fmt, Pico_Dim size, ...) {
    va_list args;
    va_start(args, fmt);
    pico_output_draw_fmt_va(pos, fmt, size, args);
    va_end(args);
}

static void show_grid (void) {
    if (!S.grid) return;

    SDL_SetRenderDrawColor(REN, 0x77,0x77,0x77,0x77);

    Pico_Dim phy = PHY;
    SDL_RenderSetLogicalSize(REN, phy.x, phy.y);
    for (int i=0; i<=phy.x; i+=(phy.x/S.size.cur.x)) {
        SDL_RenderDrawLine(REN, i, 0, i, phy.y);
    }
    for (int j=0; j<=phy.y; j+=(phy.y/S.size.cur.y)) {
        SDL_RenderDrawLine(REN, 0, j, phy.x, j);
    }
    SDL_RenderSetLogicalSize(REN, S.size.cur.x, S.size.cur.y);

    SDL_SetRenderDrawColor (REN,
        S.color.draw.r,
        S.color.draw.g,
        S.color.draw.b,
        S.color.draw.a
    );
}

static void _pico_output_present (int force) {
    if (S.expert && !force) return;
    SDL_SetRenderTarget(REN, NULL);
    SDL_SetRenderDrawColor(REN, 0x77,0x77,0x77,0x77);
    SDL_RenderClear(REN);
    SDL_RenderCopy(REN, TEX, NULL, NULL);
    show_grid();
    SDL_RenderPresent(REN);
    SDL_SetRenderDrawColor (REN,
        S.color.draw.r,
        S.color.draw.g,
        S.color.draw.b,
        S.color.draw.a
    );
    SDL_SetRenderTarget(REN, TEX);
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
        (Pico_Rect){0,0,S.size.cur.x,S.size.cur.y}
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
    _pico_output_present(0);

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

Pico_Anchor pico_get_anchor_draw (void) {
    return S.anchor.draw;
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

int pico_get_grid (void) {
    return S.grid;
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

Pico_Pos pico_get_scroll (void) {
    return S.scroll;
}

Pico_Size pico_get_size (void) {
    return (Pico_Size) { PHY, S.size.org };
}

Pico_Dim pico_get_size_image (const char* file) {
    SDL_Texture* tex = (SDL_Texture*)pico_hash_get(_pico_hash, file);
    if (tex == NULL) {
        tex = IMG_LoadTexture(REN, file);
        pico_hash_add(_pico_hash, file, tex);
    }
    pico_assert(tex != NULL);

    Pico_Dim size;
    SDL_QueryTexture(tex, NULL, NULL, &size.x, &size.y);
    return size;
}

Pico_Dim pico_get_size_text (const char* text) {
    if (!text || text[0] == '\0') {
        return (Pico_Dim){0, 0};
    }

    SDL_Surface* sfc = TTF_RenderText_Blended(S.font.ttf, text,
                                              (Pico_Color){0,0,0,255});
    pico_assert(sfc != NULL);
    Pico_Dim size = {sfc->w, sfc->h};
    SDL_FreeSurface(sfc);
    return size;
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

Pico_Pct pico_get_zoom (void) {
    return S.zoom;
}

// SET

void pico_set_anchor_draw (Pico_Anchor anchor) {
    S.anchor.draw = anchor;
}

void pico_set_anchor_rotate (Pico_Anchor anchor) {
    S.anchor.rotate = anchor;
}

void pico_set_color_clear (Pico_Color color) {
    S.color.clear = color;
}

void pico_set_color_draw  (Pico_Color color) {
    S.color.draw = color;
    SDL_SetRenderDrawColor (REN,
        S.color.draw.r,
        S.color.draw.g,
        S.color.draw.b,
        S.color.draw.a
    );
}

void pico_set_cursor (Pico_Pos pos) {
    S.cursor.cur = pos;
    S.cursor.x   = pos.x;
}

void pico_set_expert (int on) {
    S.expert = on;
}

void pico_set_flip (Pico_Flip flip) {
    S.flip = flip;
}

void pico_set_font (const char* file, int h) {
    if (file == NULL) {
        static char _file[255];
        strcpy(_file, __FILE__);
        _file[strlen(_file) - strlen("pico.c") - 1] = '\0';
        strcat(_file, "/../tiny.ttf");
        file = _file;
    }
    if (h == 0) {
        h = MAX(8, S.size.org.y/10);
    }
    S.font.h = h;
    if (S.font.ttf != NULL) {
        TTF_CloseFont(S.font.ttf);
    }
    S.font.ttf = TTF_OpenFont(file, S.font.h);
    pico_assert(S.font.ttf != NULL);
}

void pico_set_grid (int on) {
    S.grid = on;
    _pico_output_present(0);
}

void pico_set_crop (Pico_Rect crop) {
    S.crop = crop;
}

void pico_set_rotate (int angle) {
    S.angle = angle;
}

void pico_set_scale (Pico_Pct scale) {
    // TODO: checks???
    S.scale = scale;
}

void pico_set_scroll (Pico_Pos pos) {
    S.scroll = pos;
}

void _pico_set_size (Pico_Dim phy, Pico_Dim log) {
    // physical
    {
        if (phy.x==PICO_SIZE_KEEP.x && phy.y==PICO_SIZE_KEEP.y) {
            // keep
        } else if (phy.x==PICO_SIZE_FULLSCREEN.x && phy.y==PICO_SIZE_FULLSCREEN.y) {
            pico_assert(0 == SDL_SetWindowFullscreen(WIN, SDL_WINDOW_FULLSCREEN_DESKTOP));
            phy = PHY;
        } else {
            pico_assert(0 == SDL_SetWindowFullscreen(WIN, 0));
            SDL_SetWindowSize(WIN, phy.x, phy.y);
        }
    }

    // logical
    if (log.x==PICO_SIZE_KEEP.x && log.y==PICO_SIZE_KEEP.y) {
        // keep
    } else {
        S.size.cur = log;
        SDL_DestroyTexture(TEX);
        TEX = SDL_CreateTexture (
            REN, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
            S.size.cur.x, S.size.cur.y
        );
        SDL_RenderSetLogicalSize(REN, S.size.cur.x, S.size.cur.y);
        pico_assert(TEX != NULL);
    }

    if (PHY.x==S.size.cur.x || PHY.y==S.size.cur.y) {
        pico_set_grid(0);
    }

    _pico_output_present(0);
}

void pico_set_size (Pico_Dim phy, Pico_Dim log) {
    S.size.org = log;
    _pico_set_size(phy, log);
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

void pico_set_zoom (Pico_Pct zoom) {
    S.zoom = zoom;
    pico_set_scroll ((Pico_Pos) {
        S.scroll.x - (S.size.org.x - S.size.cur.x)/2,
        S.scroll.y - (S.size.org.y - S.size.cur.y)/2
    });
    _pico_set_size (
        PICO_SIZE_KEEP,
        (Pico_Dim){ S.size.org.x*100/zoom.x, S.size.org.y*100/zoom.y }
    );
    pico_set_scroll ((Pico_Pos) {
        S.scroll.x + (S.size.org.x - S.size.cur.x)/2,
        S.scroll.y + (S.size.org.y - S.size.cur.y)/2
    });
}
