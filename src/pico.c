#include "pico.h"

SDL_Window* WIN = NULL;
#define REN (SDL_GetRenderer(WIN))
int LOG_W, LOG_H;

#define X(x) ((x)+LOG_W/2)
#define Y(y) (LOG_H/2-(y))

Pico_4i SET_COLOR_BG = {0x00,0x00,0x00,0x00};
Pico_4i SET_COLOR_FG = {0xFF,0xFF,0xFF,0x00};;

void init () {
    pico_assert(SDL_Init(SDL_INIT_VIDEO) == 0);
    WIN = SDL_CreateWindow (
        _TITLE_, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        _WIN_, _WIN_, SDL_WINDOW_SHOWN
    );
    pico_assert(WIN != NULL);
    SDL_CreateRenderer(WIN, -1, 0);
    pico_assert(REN != NULL);

    output((Output){ SET, .Set={SIZE,.Size={_WIN_,_WIN_,_WIN_/10,_WIN_/10}}});
    output((Output){ CLEAR });
}

int input (Input inp) {
    switch (inp.sub) {
        case DELAY:
            SDL_Delay(inp.Delay);
            return 0;
        case EVENT:
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

void output (Output out) {
    switch (out.sub) {
        case SET:
            switch (out.Set.sub) {
                case COLOR_BG:
                    SET_COLOR_BG = out.Set.Color_BG;
                    break;
                case COLOR_FG:
                    SET_COLOR_FG = out.Set.Color_FG;
                    break;

                case SIZE: {
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
                    output((Output){CLEAR});
                    break;
                }

                case TITLE:
                    SDL_SetWindowTitle(WIN, out.Set.Title);
                    break;
            }
            break;
        case CLEAR:
            SDL_SetRenderDrawColor (REN,
                SET_COLOR_BG.v1,
                SET_COLOR_BG.v2,
                SET_COLOR_BG.v3,
                SET_COLOR_BG.v4
            );
            SDL_RenderClear(REN);
            SDL_RenderPresent(REN);
            SDL_SetRenderDrawColor (REN,
                SET_COLOR_FG.v1,
                SET_COLOR_FG.v2,
                SET_COLOR_FG.v3,
                SET_COLOR_FG.v4
            );
            break;
        case UPDATE:
            SDL_RenderPresent(REN);
            break;
        case DRAW:
            switch (out.Draw.sub) {
                case PIXEL: {
                    Pico_4i rct = { X(out.Draw.Pixel.v1), Y(out.Draw.Pixel.v2), 1, 1 };
                    SDL_RenderFillRect(REN, (SDL_Rect*)&rct);
                    SDL_RenderPresent(REN);
                    break;
                }
            }
            break;
    }
}
