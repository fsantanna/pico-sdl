#include "pico.h"

SDL_Window* WIN = NULL;
#define REN (SDL_GetRenderer(WIN))
int LOG_W, LOG_H;

#define X(x) ((x)+LOG_W/2)
#define Y(y) (LOG_H/2-(y))

SDL_Color SET_COLOR_BG = {0x00,0x00,0x00,0x00};
SDL_Color SET_COLOR_FG = {0xFF,0xFF,0xFF,0x00};;

void init () {
    pico_assert(SDL_Init(SDL_INIT_VIDEO) == 0);
    WIN = SDL_CreateWindow (
        TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        WIN_DIM, WIN_DIM, SDL_WINDOW_SHOWN
    );
    pico_assert(WIN != NULL);
    SDL_CreateRenderer(WIN, -1, 0);
    pico_assert(REN != NULL);

    output((Output){ SET, .Set={SIZE,.Size={WIN_DIM,WIN_DIM,WIN_DIM/10,WIN_DIM/10}}});
    output((Output){ CLEAR });
}

void input (Input inp) {
    switch (inp.sub) {
        case DELAY:
            SDL_Delay(inp.Delay);
            break;
    }
}

void output (Output out) {
    switch (out.sub) {
        case SET:
            switch (out.Set.sub) {
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

                case COLOR_BG:
                    SET_COLOR_BG = out.Set.Color_BG;
                    break;
                case COLOR_FG:
                    SET_COLOR_FG = out.Set.Color_FG;
                    break;
            }
            break;
        case CLEAR:
            SDL_SetRenderDrawColor (REN,
                SET_COLOR_BG.r,
                SET_COLOR_BG.g,
                SET_COLOR_BG.b,
                SET_COLOR_BG.a
            );
            SDL_RenderClear(REN);
            SDL_RenderPresent(REN);
            SDL_SetRenderDrawColor (REN,
                SET_COLOR_FG.r,
                SET_COLOR_FG.g,
                SET_COLOR_FG.b,
                SET_COLOR_FG.a
            );
            break;
        case UPDATE:
            SDL_RenderPresent(REN);
            break;
        case DRAW:
            switch (out.Draw.sub) {
                case PIXEL: {
                    SDL_Rect rct = { X(out.Draw.Pixel.x), Y(out.Draw.Pixel.y), 1, 1 };
                    SDL_RenderFillRect(REN, &rct);
                    SDL_RenderPresent(REN);
                    break;
                }
            }
            break;
    }
}
