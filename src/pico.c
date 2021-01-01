#include "pico.h"

static SDL_Window* WIN;
static TTF_Font*   FNT;

static int FNT_H;
static int LOG_W;
static int LOG_H;

#define REN  (SDL_GetRenderer(WIN))
#define X(x) ((x)+LOG_W/2)
#define Y(y) (LOG_H/2-(y))

Pico_4i SET_COLOR_CLEAR = {0x00,0x00,0x00,0x00};
Pico_4i SET_COLOR_DRAW  = {0xFF,0xFF,0xFF,0x00};;

static void WIN_Present (void) {
    //if (FRAMES_SET) return;
    //WINDOW_Show_Grid();
    SDL_RenderPresent(REN);
    //SDL_Delay(10);  // prevents flickering in Linux (probably related to double buffering)
}

void pico_init (void) {
    pico_assert(SDL_Init(SDL_INIT_VIDEO) == 0);
    WIN = SDL_CreateWindow (
        _TITLE_, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        _WIN_, _WIN_, SDL_WINDOW_SHOWN
    );
    pico_assert(WIN != NULL);
    SDL_CreateRenderer(WIN, -1, 0);
    pico_assert(REN != NULL);

    // SET_FONT
    //emit GRAPHICS_SET_FONT("tiny.ttf", WIN_DIM/50);
    TTF_Init();
    FNT_H = _WIN_ / 50;
    FNT = TTF_OpenFont("tiny.ttf", FNT_H);
    pico_assert(FNT != NULL);

    pico_output((Pico_Output){ PICO_SET, .Set={PICO_SIZE,.Size={_WIN_,_WIN_,_WIN_/10,_WIN_/10}}});
    pico_output((Pico_Output){ PICO_CLEAR });
}

void pico_quit (void) {
    TTF_Quit();
    SDL_DestroyRenderer(REN);
    SDL_DestroyWindow(WIN);
    SDL_Quit();
}

int pico_input (Pico_Input inp) {
    switch (inp.sub) {
        case PICO_DELAY:
            SDL_Delay(inp.Delay);
            return 0;
        case PICO_EVENT:
            while (1) {
                int has;
                if (inp.Event.timeout == 0) {
                    has = SDL_WaitEvent(inp.Event.ret);
                    assert(has);
                } else {
                    has = SDL_WaitEventTimeout(inp.Event.ret, inp.Event.timeout);
                }
                if (!has) {
                    return 0;
                }
                if (inp.Event.type == inp.Event.ret->type) {
                    return 1;
                }
            }
    }
    assert(0);
}

void pico_output (Pico_Output out) {
    switch (out.sub) {
        case PICO_SET:
            switch (out.Set.sub) {
                case PICO_COLOR:
                    switch (out.Set.Color.sub) {
                        case PICO_COLOR_CLEAR:
                            SET_COLOR_CLEAR = out.Set.Color.Clear;
                            break;
                        case PICO_COLOR_DRAW:
                            SET_COLOR_DRAW = out.Set.Color.Draw;
                            break;
                    }
                    break;

                case PICO_SIZE: {
                    int win_w = out.Set.Size.win_w;
                    int win_h = out.Set.Size.win_h;
                    int log_w = out.Set.Size.log_w;
                    int log_h = out.Set.Size.log_h;
                    assert(win_w%log_w == 0 && "invalid dimensions");
                    assert(win_h%log_h == 0 && "invalid dimensions");

                    SDL_SetWindowSize(WIN, win_w, win_h);
                    SDL_RenderSetLogicalSize(REN, log_w, log_h);
                    LOG_W = log_w;
                    LOG_H = log_h;
            #if 0
                    // TODO: w/o delay, set_size + clear doesn't work
                    SDL_Delay(500);
                    if (!(win_w/log_w>1 && win_h/log_h>1)) {
                        WINDOW_SET_GRID = 0;
                    }
            #endif
                    pico_output((Pico_Output){PICO_CLEAR});
                    break;
                }

                case PICO_TITLE:
                    SDL_SetWindowTitle(WIN, out.Set.Title);
                    break;
            }
            break;
        case PICO_CLEAR:
            SDL_SetRenderDrawColor (REN,
                SET_COLOR_CLEAR.v1,
                SET_COLOR_CLEAR.v2,
                SET_COLOR_CLEAR.v3,
                SET_COLOR_CLEAR.v4
            );
            SDL_RenderClear(REN);
            WIN_Present();
            SDL_SetRenderDrawColor (REN,
                SET_COLOR_DRAW.v1,
                SET_COLOR_DRAW.v2,
                SET_COLOR_DRAW.v3,
                SET_COLOR_DRAW.v4
            );
            break;
        case PICO_DRAW:
            switch (out.Draw.sub) {
                case PICO_PIXEL: {
                    Pico_4i rct = { X(out.Draw.Pixel.v1), Y(out.Draw.Pixel.v2), 1, 1 };
                    SDL_RenderFillRect(REN, (SDL_Rect*)&rct);
                    WIN_Present();
                    break;
                }
                case PICO_TEXT: {
                    u8 r, g, b;
                    SDL_GetRenderDrawColor(REN, &r,&g,&b, NULL);
                    SDL_Surface* sfc = TTF_RenderText_Blended(FNT, out.Draw.Text.txt, (SDL_Color){r,g,b,0xFF});
                    pico_assert(sfc != NULL);
                    SDL_Texture* tex = SDL_CreateTextureFromSurface(REN, sfc);
                    pico_assert(tex != NULL);

                    SDL_Rect rct;

                    // SCALE
                    rct.w = sfc->w; // * GRAPHICS_SET_SCALE_W;
                    rct.h = sfc->h; // * GRAPHICS_SET_SCALE_H;

                    // ANCHOR
                    rct.x = X(out.Draw.Text.pos.v1); //GRAPHICS_ANCHOR_X(X(ps_->_1),rct.w);
                    rct.y = Y(out.Draw.Text.pos.v2); //GRAPHICS_ANCHOR_Y(Y(ps_->_2),rct.h);

                    SDL_RenderCopy(REN, tex, NULL, &rct);
                    WIN_Present();

                    SDL_DestroyTexture(tex);
                    SDL_FreeSurface(sfc);
                }
            }
            break;
    }
}
