#include "pico.h"

static SDL_Window*  WIN;
static SDL_Texture* TEX;
static TTF_Font*    FNT;

static int     FNT_H;
static Pico_2i LOG,PHY;

#define REN  (SDL_GetRenderer(WIN))
#define X(x) ((x)+LOG._1/2)
#define Y(y) (LOG._2/2-(y))
#define _X(x) ((x)-LOG._1/2)
#define _Y(y) (LOG._2/2-(y))
#define PHY_LOG_X(x) (x * LOG._1/PHY._1)
#define PHY_LOG_Y(y) (y * LOG._2/PHY._2)

static Pico_2i SET_ANCHOR      = { Center, Middle };
static int     SET_AUTO        = 1;
static Pico_4i SET_COLOR_CLEAR = {0x00,0x00,0x00,0xFF};
static Pico_4i SET_COLOR_DRAW  = {0xFF,0xFF,0xFF,0xFF};
static Pico_2i SET_CURSOR      = {0,0};
static Pico_2i CUR_CURSOR      = {0,0};

static void WIN_Present (int force) {
    if (!SET_AUTO && !force) return;
    //WINDOW_Show_Grid();
    SDL_SetRenderTarget(REN, NULL);
    SDL_RenderClear(REN);
    SDL_RenderCopy(REN, TEX, NULL, NULL);
    SDL_RenderPresent(REN);
    SDL_SetRenderTarget(REN, TEX);
}

static int hanchor (int x, int w) {
    switch (SET_ANCHOR._1) {
        case Left:
            return x;
        case Center:
            return x - w/2;
        case Right:
            return x - w + 1;
    }
    assert(0);
}

static int vanchor (int y, int h) {
    switch (SET_ANCHOR._2) {
        case Top:
            return y;
        case Middle:
            return y - h/2;
        case Bottom:
            return y - h + 1;
    }
    assert(0);
}

void pico_init (void) {
    pico_assert(SDL_Init(SDL_INIT_VIDEO) == 0);
    WIN = SDL_CreateWindow (
        _TITLE_, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        _WIN_, _WIN_, SDL_WINDOW_SHOWN
    );
    pico_assert(WIN != NULL);

    // https://stackoverflow.com/questions/19935727/sdl2-how-to-render-with-one-buffer-instead-of-two
    SDL_CreateRenderer(WIN, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_TARGETTEXTURE);
    pico_assert(REN != NULL);
    SDL_SetRenderDrawBlendMode(REN,SDL_BLENDMODE_BLEND);

    TTF_Init();

    pico_output((Pico_IO){ PICO_SET_SIZE,.Set_Size={{_WIN_,_WIN_},{_WIN_/10,_WIN_/10}}});
    pico_output((Pico_IO){ PICO_SET_FONT,.Set_Font={"tiny.ttf",_WIN_/50} });
    //pico_output((Pico_IO){ PICO_CLEAR });

    //SDL_Delay(1000);
    //SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
}

void pico_quit (void) {
    TTF_Quit();
    SDL_DestroyRenderer(REN);
    SDL_DestroyWindow(WIN);
    SDL_Quit();
}

static int event (SDL_Event* e, int xp) {
    switch (e->type) {
        case SDL_QUIT:
            exit(0);

        case SDL_WINDOWEVENT: {
            if (e->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                int w = e->window.data1; // - e->window.data1 % LOG._1;
                int h = e->window.data2; // - e->window.data2 % LOG._2;
//printf(">>> (%d,%d) -> (%d,%d)\n", e->window.data1, e->window.data2, w, h);
                pico_output((Pico_IO){ PICO_SET_SIZE,.Set_Size={{w,h},{LOG._1,LOG._2}}});
                return 0;
                break;
            }
        }
    }

    if (xp!=SDL_ANY && xp!=e->type) {
        return 0;
    }

    switch (e->type) {
        case SDL_MOUSEBUTTONDOWN:
            // for some reason, e->button uses physical, not logical screen
            e->button.x = _X(PHY_LOG_X(e->button.x));
            e->button.y = _Y(PHY_LOG_Y(e->button.y));
            break;
        default:
            break;
    }
    return 1;
}

int pico_input (Pico_IO inp) {
    switch (inp.sub) {
        case PICO_DELAY:
            while (1) {
                int old = SDL_GetTicks();
                SDL_Event e;
                int has = SDL_WaitEventTimeout(&e, inp.Delay);
                if (!has) {
                    return 0;
                }
                event(&e, SDL_ANY);
                int dt = SDL_GetTicks() - old;
                inp.Delay -= dt;
            }

        case PICO_EVENT:
            while (1) {
                SDL_WaitEvent(inp.Event.ret);
                if (event(inp.Event.ret, inp.Event.type)) {
                    return 1;
                }
            }

        case PICO_EVENT_TIMEOUT:
            while (1) {
                int has = SDL_WaitEventTimeout(inp.Event_Timeout.ret, inp.Event_Timeout.timeout);
                if (!has) {
                    return 0;
                }
                if (event(inp.Event_Timeout.ret, inp.Event.type)) {
                    return 1;
                }
            }

        default:
            break;
    }
    assert(0);
}

void pico_output (Pico_IO out) {
    switch (out.sub) {
        case PICO_CLEAR:
            SDL_SetRenderDrawColor (REN,
                SET_COLOR_CLEAR._1,
                SET_COLOR_CLEAR._2,
                SET_COLOR_CLEAR._3,
                SET_COLOR_CLEAR._4
            );
            SDL_RenderClear(REN);
            WIN_Present(0);
            SDL_SetRenderDrawColor (REN,
                SET_COLOR_DRAW._1,
                SET_COLOR_DRAW._2,
                SET_COLOR_DRAW._3,
                SET_COLOR_DRAW._4
            );
            break;

        case PICO_GET_SIZE:
            *out.Get_Size.phy = PHY;
            *out.Get_Size.log = LOG;
            break;

        case PICO_DRAW_PIXEL: {
            SDL_RenderDrawPoint(REN, X(out.Draw_Pixel._1), Y(out.Draw_Pixel._2) );
            WIN_Present(0);
            break;
        }
        case PICO_DRAW_TEXT: {
            u8 r, g, b;
            SDL_GetRenderDrawColor(REN, &r,&g,&b, NULL);
            SDL_Surface* sfc = TTF_RenderText_Blended(FNT, out.Draw_Text.txt, (SDL_Color){r,g,b,0xFF});
            pico_assert(sfc != NULL);
            SDL_Texture* tex = SDL_CreateTextureFromSurface(REN, sfc);
            pico_assert(tex != NULL);

            SDL_Rect rct;

            // SCALE
            rct.w = sfc->w; // * GRAPHICS_SET_SCALE_W;
            rct.h = sfc->h; // * GRAPHICS_SET_SCALE_H;

            // ANCHOR
            rct.x = hanchor( X(out.Draw_Text.pos._1), rct.w );
            rct.y = vanchor( Y(out.Draw_Text.pos._2), rct.h );

            SDL_RenderCopy(REN, tex, NULL, &rct);
            WIN_Present(0);

            SDL_DestroyTexture(tex);
            SDL_FreeSurface(sfc);
        }

        case PICO_PRESENT:
            WIN_Present(1);
            break;

        case PICO_SET_AUTO:
            SET_AUTO = out.Set_Auto;
            break;
        case PICO_SET_COLOR_CLEAR:
            SET_COLOR_CLEAR = out.Set_Color_Clear;
            break;
        case PICO_SET_COLOR_DRAW:
            SET_COLOR_DRAW = out.Set_Color_Draw;
            SDL_SetRenderDrawColor (REN,
                SET_COLOR_DRAW._1,
                SET_COLOR_DRAW._2,
                SET_COLOR_DRAW._3,
                SET_COLOR_DRAW._4
            );
            break;
        case PICO_SET_CURSOR:
            CUR_CURSOR = SET_CURSOR = out.Set_Cursor;
            break;
        case PICO_SET_FONT:
            FNT_H = out.Set_Font.height;
            if (FNT != NULL) {
                TTF_CloseFont(FNT);
            }
            FNT = TTF_OpenFont(out.Set_Font.file, FNT_H);
            pico_assert(FNT != NULL);
            break;
        case PICO_SET_SIZE: {
            Pico_2i phy = out.Set_Size.phy;
            Pico_2i log = out.Set_Size.log;

            // check resize that was not accepted (due to remainder with logical size)
            {
                int w,h;
                SDL_GetWindowSize(WIN, &w,&h);
                if (phy._1==w && phy._2==h && log._1==LOG._1 && log._2==LOG._2) {
                    break;
                }
            }

            assert(log._1!=0 && log._2!=0 && "invalid dimensions");
            //assert(phy._1%log._1 == 0 && "invalid dimensions");
            //assert(phy._2%log._2 == 0 && "invalid dimensions");

            TEX = SDL_CreateTexture (
                    REN, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
                    log._1, log._2
            );
            pico_assert(TEX != NULL);
            SDL_SetRenderTarget(REN, TEX);

            SDL_SetWindowSize(WIN, phy._1, phy._2);
            SDL_RenderSetLogicalSize(REN, log._1, log._2);
            PHY = phy;
            LOG = log;
#if 0
            // TODO: w/o delay, set_size + clear doesn't work
            SDL_Delay(500);
            if (!(win_w/log_w>1 && win_h/log_h>1)) {
                WINDOW_SET_GRID = 0;
            }
#endif
            pico_output((Pico_IO){PICO_CLEAR});
            break;
        }
        case PICO_SET_TITLE:
            SDL_SetWindowTitle(WIN, out.Set_Title);
            break;

        case PICO_WRITE:
        case PICO_WRITELN: {
            if (strlen(out.Write) == 0) {
                if (out.sub == PICO_WRITELN) {
                    CUR_CURSOR._1 = SET_CURSOR._1;
                    CUR_CURSOR._2 -= FNT_H;
                }
                break;
            }

            SDL_Surface* sfc = TTF_RenderText_Blended (
                FNT, out.Write,
                (SDL_Color) { SET_COLOR_DRAW._1, SET_COLOR_DRAW._2,
                              SET_COLOR_DRAW._3, SET_COLOR_DRAW._4 }
            );
            pico_assert(sfc != NULL);
            SDL_Texture* tex = SDL_CreateTextureFromSurface(REN, sfc);
            pico_assert(tex != NULL);

            int w, h;
            TTF_SizeText(FNT, out.Write, &w,&h);
            SDL_Rect rct = { X(CUR_CURSOR._1),Y(CUR_CURSOR._2), w,h };
            SDL_RenderCopy(REN, tex, NULL, &rct);
            WIN_Present(0);

            CUR_CURSOR._1 += w;
            if (out.sub == PICO_WRITELN) {
                CUR_CURSOR._1 = SET_CURSOR._1;
                CUR_CURSOR._2 -= FNT_H;
            }

            SDL_DestroyTexture(tex);
            SDL_FreeSurface(sfc);
        }

        default:
            break;
    }
}
