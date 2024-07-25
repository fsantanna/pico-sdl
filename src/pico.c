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

#define X(v) ((v)+LOG.x/2-S.pan.x)
#define Y(v) (LOG.y/2-(v)-S.pan.y)
#define _X(v) ((v)-LOG.x/2+S.pan.x)
#define _Y(v) (LOG.y/2-(v)+S.pan.y)

#define LOG ({SDL_Point log; SDL_RenderGetLogicalSize(REN, &log.x, &log.y); log;})
#define PHY ({SDL_Point phy; SDL_GetWindowSize(WIN, &phy.x, &phy.y); phy;})
#define PHY_LOG(phy) ({float x,y; SDL_RenderWindowToLogical(REN, phy.x,phy.y,&x,&y); (SDL_Point){x,y};})

static pico_hash* _pico_hash;

static struct {
    SDL_Point anchor;
    int       autom;
    SDL_Color color_clear;
    SDL_Color color_draw;
    SDL_Point cursor;
    int       grid;
    SDL_Rect  image_crop;
    SDL_Point pan;
    SDL_Point size_image;
} S = {
    { Center, Middle },
    1,
    {0x00,0x00,0x00,0xFF},
    {0xFF,0xFF,0xFF,0xFF},
    {0,0},
    1,
    {0,0,0,0},
    {0,0},
    {0,0}
};

static void show_grid (SDL_Point log) {
    if (!S.grid) return;

    SDL_SetRenderDrawColor(REN, 0x77,0x77,0x77,0x77);

    SDL_Point phy = PHY;
    for (int i=0; i<=phy.x; i+=(phy.x/log.x)) {
        SDL_RenderDrawLine(REN, i, 0, i, phy.y);
    }
    for (int j=0; j<=phy.y; j+=(phy.y/log.y)) {
        SDL_RenderDrawLine(REN, 0, j, phy.x, j);
    }

    SDL_SetRenderDrawColor (REN,
        S.color_draw.r,
        S.color_draw.g,
        S.color_draw.b,
        S.color_draw.a
    );
}

static void WIN_Present (int force) {
    if (!S.autom && !force) return;
    SDL_Point log;
    SDL_RenderGetLogicalSize(REN, &log.x, &log.y);
    SDL_SetRenderTarget(REN, NULL);
    SDL_RenderClear(REN);
    SDL_RenderCopy(REN, TEX, NULL, NULL);
    show_grid(log);
    SDL_RenderPresent(REN);
    assert(0 == SDL_SetRenderTarget(REN, TEX));
    assert(0 == SDL_RenderSetLogicalSize(REN, log.x, log.y));
}

static int hanchor (int x, int w) {
    switch (S.anchor.x) {
        case Left:
            return x;
        case Center:
            return x - w/2;
        case Right:
            return x - w + 1;
    }
    assert(0 && "bug found");
}

static int vanchor (int y, int h) {
    switch (S.anchor.y) {
        case Top:
            return y;
        case Middle:
            return y - h/2;
        case Bottom:
            return y - h + 1;
    }
    assert(0 && "bug found");
}

void pico_init (int on) {
    if (on) {
        _pico_hash = pico_hash_create(PICO_HASH);
        pico_assert(SDL_Init(SDL_INIT_VIDEO) == 0);
        WIN = SDL_CreateWindow (
            PICO_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            PICO_WIN, PICO_WIN, SDL_WINDOW_SHOWN
        );
        pico_assert(WIN != NULL);

        // https://stackoverflow.com/questions/19935727/sdl2-how-to-render-with-one-buffer-instead-of-two
        SDL_CreateRenderer(WIN, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_TARGETTEXTURE);
        pico_assert(REN != NULL);
        SDL_SetRenderDrawBlendMode(REN,SDL_BLENDMODE_BLEND);

        TTF_Init();
        Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 4096);

        pico_state_set_size_window (
            (SDL_Point) { PICO_LOG, PICO_LOG },
            (SDL_Point) { PICO_WIN, PICO_WIN }
        );
        //pico_state_set_font("tiny.ttf", S.size.x/50);
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
        case SDL_QUIT:
            if (S.autom) {
                exit(0);
            }

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
                    pico_state_set_size_window(log, PHY);
                    break;
                }
                case SDLK_EQUALS: {
                    SDL_Point log = LOG;
                    log.x *= 1.1;
                    log.y *= 1.1;
                    pico_state_set_size_window(log, PHY);
                    break;
                }
                case SDLK_LEFT: {
                    pico_state_set_pan((SDL_Point){S.pan.x-5, S.pan.y});
                    break;
                }
                case SDLK_RIGHT: {
                    pico_state_set_pan((SDL_Point){S.pan.x+5, S.pan.y});
                    break;
                }
                case SDLK_UP: {
                    pico_state_set_pan((SDL_Point){S.pan.x, S.pan.y-5});
                    break;
                }
                case SDLK_DOWN: {
                    pico_state_set_pan((SDL_Point){S.pan.x, S.pan.y+5});
                    break;
                }
            }
        }

#if 0
        case SDL_WINDOWEVENT: {
            if (e->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                pico_state_set_size((SDL_Point){e->window.data1,e->window.data2});
                return 0;
                break;
            }
        }
#endif
        default:
            // others are not handled automatically
            break;
    }

    if (xp == e->type) {
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
        case SDL_MOUSEMOTION: {
            // for some reason, e->button uses physical, not logical screen
            SDL_Point phy = { e->button.x, e->button.y };
            SDL_Point log = PHY_LOG(phy);
            e->button.x = _X(log.x);
            e->button.y = _Y(log.y);
            break;
        }
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
        SDL_WaitEvent(evt);
        if (pico_event_from_sdl(evt, type)) {
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

void pico_output_clear (void) {
    SDL_SetRenderDrawColor (REN,
        S.color_clear.r,
        S.color_clear.g,
        S.color_clear.b,
        S.color_clear.a
    );
    SDL_RenderClear(REN);
    WIN_Present(0);
    SDL_SetRenderDrawColor (REN,
        S.color_draw.r,
        S.color_draw.g,
        S.color_draw.b,
        S.color_draw.a
    );
}

void _pico_output_draw_image_tex (SDL_Point pos, SDL_Texture* tex) {
    int defsize = (S.size_image.x==0 && S.size_image.y==0);
    int defcrop = (S.image_crop.x==0 && S.image_crop.y==0 &&
                   S.image_crop.w==0 && S.image_crop.h==0);

    SDL_Rect* crp;
    if (defcrop) {
        crp = NULL;
    } else {
        crp = (SDL_Rect*) &S.image_crop;
    }

    SDL_Rect rct;
    if (defsize) {
        if (defcrop) {
            SDL_QueryTexture(tex, NULL, NULL, &rct.w, &rct.h);
        } else {
            rct.w = crp->w;
            rct.h = crp->h;
        }
    } else {
        rct.w = S.size_image.x;
        rct.h = S.size_image.y;
    }

    // SCALE
    rct.w = rct.w; // * GRAPHICS_SET_SCALE_W;
    rct.h = rct.h; // * GRAPHICS_SET_SCALE_H;

    // ANCHOR
    rct.x = hanchor( X(pos.x), rct.w );
    rct.y = vanchor( Y(pos.y), rct.h );

    SDL_RenderCopy(REN, tex, crp, &rct);

    WIN_Present(0);

}

void _pico_output_draw_image_cache (SDL_Point pos, char* path, int cache) {
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

void pico_output_draw_image (SDL_Point pos, char* path) {
    _pico_output_draw_image_cache(pos, path, 1);
}

void pico_output_draw_line (SDL_Point p1, SDL_Point p2) {
    SDL_RenderDrawLine(REN, X(p1.x),Y(p1.y), X(p2.x),Y(p2.y));
    WIN_Present(0);
}

void pico_output_draw_pixel (SDL_Point pos) {
    SDL_RenderDrawPoint(REN, X(pos.x), Y(pos.y) );
    WIN_Present(0);
}

void pico_output_draw_rect (SDL_Rect rect) {
    SDL_Rect out = {
        hanchor(X(rect.x),rect.w),
        vanchor(Y(rect.y),rect.h),
        rect.w, rect.h
    };
    SDL_RenderFillRect(REN, &out);
    WIN_Present(0);
}

void pico_output_draw_oval (SDL_Rect rect) {
    SDL_Rect out = {
        hanchor(X(rect.x),rect.w),
        vanchor(Y(rect.y),rect.h),
        rect.w, rect.h
    };
    filledEllipseRGBA (
        REN,
        out.x+out.w/2, out.y+out.h/2, out.w/2, out.h/2,
        S.color_draw.r, S.color_draw.g, S.color_draw.b, S.color_draw.a
    );
    WIN_Present(0);
}

void pico_output_draw_text  (SDL_Point pos, char* text) {
    uint8_t r, g, b;
    SDL_GetRenderDrawColor(REN, &r,&g,&b, NULL);
    assert(FNT != NULL);
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
    rct.x = hanchor( X(pos.x), rct.w );
    rct.y = vanchor( Y(pos.y), rct.h );

    SDL_RenderCopy(REN, tex, NULL, &rct);
    WIN_Present(0);

    SDL_DestroyTexture(tex);
    SDL_FreeSurface(sfc);
}

void pico_output_present (void) {
    WIN_Present(1);
}

void _pico_output_sound_cache (char* path, int cache) {
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

void pico_output_sound (char* path) {
    _pico_output_sound_cache(path, 1);
}

void pico_output_write_aux (char* text, int isln) {
    if (strlen(text) == 0) {
        if (isln) {
            CUR_CURSOR.x = S.cursor.x;
            CUR_CURSOR.y -= FNT_H;
        }
        return;
    }

    assert(FNT != NULL);
    SDL_Surface* sfc = TTF_RenderText_Blended (
        FNT, text,
        (SDL_Color) { S.color_draw.r, S.color_draw.g,
                      S.color_draw.b, S.color_draw.a }
    );
    pico_assert(sfc != NULL);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(REN, sfc);
    pico_assert(tex != NULL);

    int w, h;
    TTF_SizeText(FNT, text, &w,&h);
    SDL_Rect rct = { X(CUR_CURSOR.x),Y(CUR_CURSOR.y), w,h };
    SDL_RenderCopy(REN, tex, NULL, &rct);
    WIN_Present(0);

    CUR_CURSOR.x += w;
    if (isln) {
        CUR_CURSOR.x = S.cursor.x;
        CUR_CURSOR.y -= FNT_H;
    }

    SDL_DestroyTexture(tex);
    SDL_FreeSurface(sfc);
}

void pico_output_write (char* text) {
    pico_output_write_aux(text, 0);
}

void pico_output_writeln (char* text) {
    pico_output_write_aux(text, 1);
}


// STATE

// GET

void pico_state_get_size (SDL_Point* size) {
    assert(0 && "TODO");
    //size->x = WIN_W;
    //size->y = WIN_H;
}

void pico_state_get_size_image (char* file, SDL_Point* size) {
    SDL_Texture* tex = IMG_LoadTexture(REN, file);
    pico_assert(tex != NULL);
    SDL_QueryTexture(tex, NULL, NULL, &size->x, &size->y);
}

// SET

void pico_state_set_anchor (Pico_HAnchor h, Pico_VAnchor v) {
    S.anchor = (SDL_Point) {h, v};
}

void pico_state_set_auto (int on) {
    S.autom = on;
}

void pico_state_set_color_clear (SDL_Color color) {
    S.color_clear = color;
}

void pico_state_set_color_draw  (SDL_Color color) {
    S.color_draw = color;
    SDL_SetRenderDrawColor (REN,
        S.color_draw.r,
        S.color_draw.g,
        S.color_draw.b,
        S.color_draw.a
    );
}

void pico_state_set_cursor (SDL_Point pos) {
    CUR_CURSOR = S.cursor = pos;
}

void pico_state_set_grid (int on) {
    S.grid = on;
}

void pico_state_set_pan (SDL_Point pos) {
    S.pan = pos;
}

void pico_state_set_font (char* file, int h) {
    FNT_H = h;
    if (FNT != NULL) {
        TTF_CloseFont(FNT);
    }
    FNT = TTF_OpenFont(file, FNT_H);
    pico_assert(FNT != NULL);
}

void pico_state_set_size_window (SDL_Point log, SDL_Point phy) {
    SDL_SetWindowSize(WIN, phy.x, phy.y);
    TEX = SDL_CreateTexture (
            REN, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
            phy.x, phy.y
    );
    pico_assert(TEX != NULL);
    assert(0 == SDL_SetRenderTarget(REN, TEX));
    assert(0 == SDL_RenderSetLogicalSize(REN, log.x, log.y));
    pico_output_clear();
}

void pico_state_set_image_crop (SDL_Rect crop) {
    S.image_crop = crop;
}

void pico_state_set_size_image (SDL_Point size) {
    S.size_image = size;
}

void pico_state_set_title (char* title) {
    SDL_SetWindowTitle(WIN, title);
}

///////////////////////////////////////////////////////////////////////////////

int pico_isPointVsRect (SDL_Point pt, SDL_Rect r) {
    int rw = r.w / 2;
    int rh = r.h / 2;
    return !(pt.x<r.x-rw || pt.x>r.x+rw || pt.y<r.y-rh || pt.y>r.y+rh);
}
