#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include "hash.h"
#include "pico.h"

#define MAX(x,y) ((x) > (y) ? (x) : (y))

SDL_Window*         WIN;
static SDL_Texture* TEX;
static TTF_Font*    FNT = NULL;
static int          FNT_H;
static SDL_Point    CUR_CURSOR = {0,0};

#define REN  (SDL_GetRenderer(WIN))

#define X(v,w) (hanchor(v,w)-S.pan.x)
#define Y(v,h) (vanchor(v,h)-S.pan.y)

#define LOG ({SDL_Point log; SDL_RenderGetLogicalSize(REN, &log.x, &log.y); log;})
#define PHY ({SDL_Point phy; SDL_GetWindowSize(WIN, &phy.x, &phy.y); phy;})

static pico_hash* _pico_hash;

static struct {
    SDL_Point anchor;
    struct {
        SDL_Color clear;
        SDL_Color draw;
    } color;
    SDL_Point cursor;
    int grid;
    struct {
        SDL_Rect  crop;
        SDL_Point size;
    } image;
    SDL_Point pan;
    Pico_Style style;
} S = {
    { PICO_CENTER, PICO_MIDDLE },
    { {0x00,0x00,0x00,0xFF}, {0xFF,0xFF,0xFF,0xFF} },
    {0,0},
    1,
    { {0,0,0,0}, {0,0} },
    {0,0},
    PICO_FILL
};

static void show_grid (void) {
    if (!S.grid) return;

    SDL_SetRenderDrawColor(REN, 0x77,0x77,0x77,0x77);

    SDL_Point log = LOG;
    SDL_Point phy = PHY;

    SDL_RenderSetLogicalSize(REN, phy.x, phy.y);
    for (int i=0; i<=phy.x; i+=(phy.x/log.x)) {
        SDL_RenderDrawLine(REN, i, 0, i, phy.y);
    }
    for (int j=0; j<=phy.y; j+=(phy.y/log.y)) {
        SDL_RenderDrawLine(REN, 0, j, phy.x, j);
    }
    SDL_RenderSetLogicalSize(REN, log.x, log.y);

    SDL_SetRenderDrawColor (REN,
        S.color.draw.r,
        S.color.draw.g,
        S.color.draw.b,
        S.color.draw.a
    );
}

static int hanchor (int x, int w) {
    switch (S.anchor.x) {
        case PICO_LEFT:
            return x;
        case PICO_CENTER:
            return x - w/2;
        case PICO_RIGHT:
            return x - w; // + 1;
    }
    assert(0 && "bug found");
}

static int vanchor (int y, int h) {
    switch (S.anchor.y) {
        case PICO_TOP:
            return y;
        case PICO_MIDDLE:
            return y - h/2;
        case PICO_BOTTOM:
            return y - h; // + 1;
    }
    assert(0 && "bug found");
}

void pico_init (int on) {
    if (on) {
        _pico_hash = pico_hash_create(PICO_HASH);
        pico_assert(0 == SDL_Init(SDL_INIT_VIDEO));
        WIN = SDL_CreateWindow (
            PICO_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            PICO_PHY_X, PICO_PHY_Y, SDL_WINDOW_SHOWN
        );
        pico_assert(WIN != NULL);

        //SDL_CreateRenderer(WIN, -1, SDL_RENDERER_ACCELERATED);
        SDL_CreateRenderer(WIN, -1, SDL_RENDERER_SOFTWARE);
        pico_assert(REN != NULL);

        TTF_Init();
        Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 4096);

        pico_set_size_internal((SDL_Point) { PICO_LOG_X, PICO_LOG_Y });
        pico_set_size_external((SDL_Point) { PICO_PHY_X, PICO_PHY_Y });

        //pico_set_font("tiny.ttf", S.size.x/50);
        //pico_output_clear();

        //SDL_Delay(1000);
        //SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
    } else {
        if (FNT != NULL) {
            TTF_CloseFont(FNT);
        }
        Mix_CloseAudio();
        TTF_Quit();
        SDL_DestroyRenderer(REN);
        SDL_DestroyWindow(WIN);
        SDL_Quit();
        pico_hash_destroy(_pico_hash);
    }
}

// Pre-handles input from environment:
//  - SDL_QUIT: quit
//  - CTRL_-/=: pixel
//  - CTRL_L/R/U/D: pan
//  - receives:
//      - e:  actual input
//      - xp: input I was expecting
//  - returns
//      - 1: if e matches xp
//      - 0: otherwise
int pico_event_from_sdl (SDL_Event* e, int xp) {
    switch (e->type) {
        case SDL_KEYDOWN: {
            const unsigned char* state = SDL_GetKeyboardState(NULL);
            if (!state[SDL_SCANCODE_LCTRL] && !state[SDL_SCANCODE_RCTRL]) {
                break;
            }
            switch (e->key.keysym.sym) {
                case SDLK_MINUS: {
                    SDL_Point log = LOG;
                    log.x *= 0.9;
                    log.y *= 0.9;
                    pico_set_size_internal(log);
                    break;
                }
                case SDLK_EQUALS: {
                    SDL_Point log = LOG;
                    log.x *= 1.1;
                    log.y *= 1.1;
                    pico_set_size_internal(log);
                    break;
                }
                case SDLK_LEFT: {
                    pico_set_pan((SDL_Point){S.pan.x-5, S.pan.y});
                    break;
                }
                case SDLK_RIGHT: {
                    pico_set_pan((SDL_Point){S.pan.x+5, S.pan.y});
                    break;
                }
                case SDLK_UP: {
                    pico_set_pan((SDL_Point){S.pan.x, S.pan.y-5});
                    break;
                }
                case SDLK_DOWN: {
                    pico_set_pan((SDL_Point){S.pan.x, S.pan.y+5});
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
            e->button.x = e->button.x + S.pan.x;
            e->button.y = e->button.y + S.pan.y;
            break;
        default:
            break;
    }
    return 1;
}

// INPUT

void pico_input_delay (int ms) {
    while (1) {
        int old = SDL_GetTicks();
        SDL_Event e;
        int has = SDL_WaitEventTimeout(&e, ms);
        if (has) {
            pico_event_from_sdl(&e, SDL_ANY);
        }
        int dt = SDL_GetTicks() - old;
        ms -= dt;
        if (ms <= 0) {
            return;
        }
    }
}

void pico_input_event (SDL_Event* evt, int type) {
    while (1) {
        SDL_Event x;
        SDL_WaitEvent(&x);
        if (pico_event_from_sdl(&x, type)) {
            if (evt != NULL) {
                *evt = x;
            }
            return;
        }
    }
}

int pico_input_event_ask (SDL_Event* evt, int type) {
    int has = SDL_PollEvent(evt);
    if (!has) return 0;
    return pico_event_from_sdl(evt, type);
}

int pico_input_event_timeout (SDL_Event* evt, int type, int timeout) {
    int has = SDL_WaitEventTimeout(evt, timeout);
    if (!has) {
        return 0;
    }
    if (pico_event_from_sdl(evt, type)) {
        return 1;
    }
    return 0;
}

// OUTPUT

static void WIN_Clear (void) {
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

void pico_output_clear (void) {
    WIN_Clear();
}

void _pico_output_draw_image_tex (SDL_Point pos, SDL_Texture* tex) {
    SDL_Rect rct;
    SDL_QueryTexture(tex, NULL, NULL, &rct.w, &rct.h);

    SDL_Rect crp = S.image.crop;
    if (S.image.crop.w == 0) {
        crp.w = rct.w;
    }
    if (S.image.crop.h == 0) {
        crp.h = rct.h;
    }

    if (S.image.size.x==0 && S.image.size.y==0) {
        // normal image size
        rct.w = crp.w;  // (or copy from crop)
        rct.h = crp.h;  // (or copy from crop)
    } else if (S.image.size.x == 0) {
        // adjust w based on h
        rct.w = rct.w * (S.image.size.y / (float)rct.h);
        rct.h = S.image.size.y;
    } else if (S.image.size.y == 0) {
        // adjust h based on w
        rct.h = rct.h * (S.image.size.x / (float)rct.w);
        rct.w = S.image.size.x;
    } else {
        rct.w = S.image.size.x;
        rct.h = S.image.size.y;
    }

    // SCALE
    rct.w = rct.w; // * GRAPHICS_SET_SCALE_W;
    rct.h = rct.h; // * GRAPHICS_SET_SCALE_H;

    // ANCHOR / PAN
    rct.x = X(pos.x, rct.w);
    rct.y = Y(pos.y, rct.h);

    SDL_RenderCopy(REN, tex, &crp, &rct);
}

void _pico_output_draw_image_cache (SDL_Point pos, const char* path, int cache) {
    SDL_Texture* tex = NULL;
    if (cache) {
        tex = pico_hash_get(_pico_hash, path);
        if (tex == NULL) {
            tex = IMG_LoadTexture(REN, path);
            pico_hash_add(_pico_hash, path, tex);
        }
    } else {
        tex = IMG_LoadTexture(REN, path);
    }
    pico_assert(tex != NULL);

    _pico_output_draw_image_tex(pos, tex);

    if (!cache) {
        SDL_DestroyTexture(tex);
    }
}

void pico_output_draw_image (SDL_Point pos, const char* path) {
    _pico_output_draw_image_cache(pos, path, 1);
}

void pico_output_draw_line (SDL_Point p1, SDL_Point p2) {
    SDL_RenderDrawLine(REN, X(p1.x,1),Y(p1.y,1), X(p2.x,1),Y(p2.y,1));
}

void pico_output_draw_pixel (SDL_Point pos) {
    SDL_RenderDrawPoint(REN, X(pos.x,1), Y(pos.y,1) );
}

void pico_output_draw_pixels (const SDL_Point* apos, int count) {
    SDL_Point *apos_fixed = malloc(count * sizeof(SDL_Point));
    for (int i = 0; i < count; i++) {
        apos_fixed[i].x = X(apos[i].x,1);
        apos_fixed[i].y = Y(apos[i].y,1);
    }

    SDL_RenderDrawPoints(REN, apos_fixed, count);
    free(apos_fixed);
}

void pico_output_draw_rect (SDL_Rect rect) {
    SDL_Rect out = {
        X(rect.x, rect.w),
        Y(rect.y, rect.h),
        rect.w, rect.h
    };
    switch (S.style) {
        case PICO_FILL:
            SDL_RenderFillRect(REN, &out);
            break;
        case PICO_STROKE:
            SDL_RenderDrawRect(REN, &out);
            break;
    }
}

void pico_output_draw_oval (SDL_Rect rect) {
    SDL_Rect out = {
        X(rect.x, rect.w),
        Y(rect.y, rect.h),
        rect.w, rect.h
    };
    switch (S.style) {
        case PICO_FILL:
            filledEllipseRGBA (
                REN,
                out.x+out.w/2, out.y+out.h/2, out.w/2, out.h/2,
                S.color.draw.r, S.color.draw.g, S.color.draw.b, S.color.draw.a
            );
            break;
        case PICO_STROKE:
            ellipseRGBA (
                REN,
                out.x+out.w/2, out.y+out.h/2, out.w/2, out.h/2,
                S.color.draw.r, S.color.draw.g, S.color.draw.b, S.color.draw.a
            );
            break;
    }
}

void pico_output_draw_text (SDL_Point pos, const char* text) {
    uint8_t r, g, b;
    SDL_GetRenderDrawColor(REN, &r,&g,&b, NULL);
    pico_assert(FNT != NULL);
    SDL_Surface* sfc = TTF_RenderText_Blended(FNT, text,
                                              (SDL_Color){r,g,b,0xFF});
    pico_assert(sfc != NULL);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(REN, sfc);
    pico_assert(tex != NULL);

    SDL_Rect rct;

    // SCALE
    rct.w = sfc->w; // * GRAPHICS_SET_SCALE_W;
    rct.h = sfc->h; // * GRAPHICS_SET_SCALE_H;

    // ANCHOR
    rct.x = X(pos.x, rct.w);
    rct.y = Y(pos.y, rct.h);

    SDL_RenderCopy(REN, tex, NULL, &rct);

    SDL_DestroyTexture(tex);
    SDL_FreeSurface(sfc);
}

void pico_output_present (void) {
    show_grid();
    SDL_RenderPresent(REN);
    WIN_Clear();
}

void _pico_output_sound_cache (const char* path, int cache) {
    Mix_Chunk* mix = NULL;

    if (cache) {
        mix = pico_hash_get(_pico_hash, path);
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

void pico_output_sound (const char* path) {
    _pico_output_sound_cache(path, 1);
}

void pico_output_write_aux (const char* text, int isln) {
    if (strlen(text) == 0) {
        if (isln) {
            CUR_CURSOR.x = S.cursor.x;
            CUR_CURSOR.y += FNT_H;
        }
        return;
    }

    pico_assert(FNT != NULL);
    SDL_Surface* sfc = TTF_RenderText_Blended (
        FNT, text,
        (SDL_Color) { S.color.draw.r, S.color.draw.g,
                      S.color.draw.b, S.color.draw.a }
    );
    pico_assert(sfc != NULL);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(REN, sfc);
    pico_assert(tex != NULL);

    int w, h;
    TTF_SizeText(FNT, text, &w,&h);
    SDL_Rect rct = { X(CUR_CURSOR.x,0),Y(CUR_CURSOR.y,0), w,h };
    SDL_RenderCopy(REN, tex, NULL, &rct);

    CUR_CURSOR.x += w;
    if (isln) {
        CUR_CURSOR.x = S.cursor.x;
        CUR_CURSOR.y += FNT_H;
    }

    SDL_DestroyTexture(tex);
    SDL_FreeSurface(sfc);
}

void pico_output_write (const char* text) {
    pico_output_write_aux(text, 0);
}

void pico_output_writeln (const char* text) {
    pico_output_write_aux(text, 1);
}


// STATE

// GET

SDL_Point pico_get_image_size (const char* file) {
    SDL_Texture* tex = IMG_LoadTexture(REN, file);
    pico_assert(tex != NULL);
    SDL_Point size;
    SDL_QueryTexture(tex, NULL, NULL, &size.x, &size.y);
    return size;
}

SDL_Point pico_get_size (void) {
    SDL_Point phy = pico_get_size_external();
    SDL_Point log = pico_get_size_internal();
    assert(phy.x==log.x && phy.y==log.y);
    return phy;
}

SDL_Point pico_get_size_external (void) {
    //return SDL_GetWindowFlags(WIN) & SDL_WINDOW_FULLSCREEN_DESKTOP;
    return PHY;
}

SDL_Point pico_get_size_internal (void) {
    return LOG;
}

Uint32 pico_get_ticks (void) {
    return SDL_GetTicks();
}

// SET

void pico_set_anchor (Pico_HAnchor h, Pico_VAnchor v) {
    S.anchor = (SDL_Point) {h, v};
}

void pico_set_color_clear (SDL_Color color) {
    S.color.clear = color;
}

void pico_set_color_draw  (SDL_Color color) {
    S.color.draw = color;
    SDL_SetRenderDrawColor (REN,
        S.color.draw.r,
        S.color.draw.g,
        S.color.draw.b,
        S.color.draw.a
    );
}

void pico_set_cursor (SDL_Point pos) {
    CUR_CURSOR = S.cursor = pos;
}

void pico_set_grid (int on) {
    S.grid = on;
}

void pico_set_image_crop (SDL_Rect crop) {
    S.image.crop = crop;
}

void pico_set_image_size (SDL_Point size) {
    S.image.size = size;
}

void pico_set_pan (SDL_Point pos) {
    S.pan = pos;
}

void pico_set_font (const char* file, int h) {
    FNT_H = h;
    if (FNT != NULL) {
        TTF_CloseFont(FNT);
    }
    FNT = TTF_OpenFont(file, FNT_H);
    pico_assert(FNT != NULL);
}

void pico_set_size (SDL_Point size) {
    SDL_SetWindowSize(WIN, size.x, size.y);
    SDL_RenderSetLogicalSize(REN, size.x, size.y);
    pico_set_grid(0);
    WIN_Clear();
    pico_output_present();
}

void pico_set_size_external (SDL_Point phy) {
    SDL_Point log = LOG;
    if (phy.x==0 && phy.y==0) {
        pico_assert(0 == SDL_SetWindowFullscreen(WIN, SDL_WINDOW_FULLSCREEN_DESKTOP));
        phy = PHY;
    } else {
        pico_assert(0 == SDL_SetWindowFullscreen(WIN, 0));
        SDL_SetWindowSize(WIN, phy.x, phy.y);
    }
    if (phy.x==log.x || phy.y==log.y) {
        pico_set_grid(0);
    }
    WIN_Clear();
    pico_output_present();
}

void pico_set_size_internal (SDL_Point log) {
    SDL_Point phy = PHY;
    SDL_RenderSetLogicalSize(REN, log.x, log.y);
    if (phy.x==log.x || phy.y==log.y) {
        pico_set_grid(0);
    }
    WIN_Clear();
    pico_output_present();
}

void pico_set_show (int on) {
    if (on) {
        SDL_ShowWindow(WIN);
        pico_output_present();
    } else {
        SDL_HideWindow(WIN);
    }
}

void pico_set_style (Pico_Style style) {
    S.style = style;
}

void pico_set_title (const char* title) {
    SDL_SetWindowTitle(WIN, title);
}

///////////////////////////////////////////////////////////////////////////////

int pico_is_point_in_rect (SDL_Point pt, SDL_Rect r) {
    int rw = r.w / 2;
    int rh = r.h / 2;
    return !(pt.x<r.x-rw || pt.x>r.x+rw || pt.y<r.y-rh || pt.y>r.y+rh);
}

SDL_Point pico_pct_to_pos (float x, float y) {
    SDL_Point log = LOG;
    return pico_pct_to_pos_x((SDL_Rect){log.x/2,log.y/2,log.x,log.y}, x, y);
}

SDL_Point pico_pct_to_pos_x (SDL_Rect r, float x, float y) {
    return (SDL_Point) { r.x-r.w/2 + r.w*x, r.y-r.h/2 + r.h*y };
}
