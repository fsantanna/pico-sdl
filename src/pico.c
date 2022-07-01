#include "pico.h"

#define MAX(x,y) ((x) > (y) ? (x) : (y))

SDL_Window*         WIN;
static SDL_Texture* TEX;
static TTF_Font*    FNT;
static int          FNT_H;

#define REN  (SDL_GetRenderer(WIN))

#define X(x) ((x)+LOG_W/2-SET_PAN._1)
#define Y(y) (LOG_H/2-(y)-SET_PAN._2)
#define _X(x) ((x)-LOG_W/2+SET_PAN._1)
#define _Y(y) (LOG_H/2-(y)+SET_PAN._2)

#define LOG_W (SET_SIZE._1/SET_PIXEL._1)
#define LOG_H (SET_SIZE._2/SET_PIXEL._2)
#define PHY_LOG_X(x) (x * LOG_W/SET_SIZE._1)
#define PHY_LOG_Y(y) (y * LOG_H/SET_SIZE._2)

static Pico_2i SET_ANCHOR      = { Center, Middle };
static int     SET_AUTO        = 1;
static Pico_4i SET_COLOR_CLEAR = {0x00,0x00,0x00,0xFF};
static Pico_4i SET_COLOR_DRAW  = {0xFF,0xFF,0xFF,0xFF};
static Pico_2i SET_CURSOR      = {0,0};
static int     SET_GRID        = 1;
static Pico_4i SET_IMAGE_CROP  = {0,0,0,0};
static Pico_2i SET_IMAGE_SIZE  = {0,0};
static Pico_2i SET_PAN         = {0,0};
static Pico_2i SET_SIZE        = {_WIN_,_WIN_};
static Pico_2i SET_PIXEL       = {10,10};

static Pico_2i CUR_CURSOR      = {0,0};

static void show_grid (void) {
    if (!SET_GRID) return;

    SDL_SetRenderDrawColor(REN, 0x77,0x77,0x77,0x77);

    for (int i=0; i<=SET_SIZE._1; i+=(SET_SIZE._1/LOG_W)) {
        SDL_RenderDrawLine(REN, i, 0, i, SET_SIZE._2);
    }
    for (int j=0; j<=SET_SIZE._2; j+=(SET_SIZE._2/LOG_H)) {
        SDL_RenderDrawLine(REN, 0, j, SET_SIZE._1, j);
    }

    SDL_SetRenderDrawColor (REN,
        SET_COLOR_DRAW._1,
        SET_COLOR_DRAW._2,
        SET_COLOR_DRAW._3,
        SET_COLOR_DRAW._4
    );
}

static void WIN_Present (int force) {
    if (!SET_AUTO && !force) return;
    SDL_SetRenderTarget(REN, NULL);
    SDL_RenderClear(REN);
    SDL_RenderCopy(REN, TEX, NULL, NULL);
    show_grid();
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
    assert(0 && "bug found");
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
    assert(0 && "bug found");
}

void pico_open (void) {
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

    pico_output((Pico_Output) {
        .tag = PICO_OUTPUT_SET,
        .Set = {
            .tag = PICO_OUTPUT_SET_SIZE,
            .Size = { SET_SIZE._1,SET_SIZE._2 }
        }
    });
    pico_output((Pico_Output) {
        .tag = PICO_OUTPUT_SET,
        .Set = {
            .tag = PICO_OUTPUT_SET_PIXEL,
            .Pixel = { SET_PIXEL._1, SET_PIXEL._2 }
        }
    });
    pico_output((Pico_Output) {
        .tag = PICO_OUTPUT_SET,
        .Set = {
            .tag = PICO_OUTPUT_SET_FONT,
            .Font = { "tiny.ttf", SET_SIZE._1/50 }
        }
    });
    //pico_output((Pico_Output){ PICO_OUTPUT_CLEAR });

    //SDL_Delay(1000);
    //SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
}

void pico_close (void) {
    TTF_Quit();
    SDL_DestroyRenderer(REN);
    SDL_DestroyWindow(WIN);
    SDL_Quit();
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
static int event (SDL_Event* e, int xp) {
    switch (e->type) {
        case SDL_QUIT:
            exit(0);

        case SDL_KEYDOWN: {
            const unsigned char* state = SDL_GetKeyboardState(NULL);
            if (!state[SDL_SCANCODE_LCTRL] && !state[SDL_SCANCODE_RCTRL]) {
                break;
            }
            switch (e->key.keysym.sym) {
                case SDLK_MINUS: {
                    int x = SET_PIXEL._1;
                    int y = SET_PIXEL._2;
                    pico_output((Pico_Output) {
                        .tag = PICO_OUTPUT_SET,
                        .Set = {
                            .tag = PICO_OUTPUT_SET_PIXEL,
                            .Pixel = { x-1, y-1 }
                        }
                    });
                    break;
                }
                case SDLK_EQUALS: {
                    int x = SET_PIXEL._1;
                    int y = SET_PIXEL._2;
                    pico_output((Pico_Output) {
                        .tag = PICO_OUTPUT_SET,
                        .Set = {
                            .tag = PICO_OUTPUT_SET_PIXEL,
                            .Pixel = { x+1, y+1 }
                        }
                    });
                    break;
                }
                case SDLK_LEFT: {
                    int x = SET_PAN._1;
                    int y = SET_PAN._2;
                    pico_output((Pico_Output) {
                        .tag = PICO_OUTPUT_SET,
                        .Set = {
                            .tag = PICO_OUTPUT_SET_PAN,
                            .Pan = { x-5, y }
                        }
                    });
                    break;
                }
                case SDLK_RIGHT: {
                    int x = SET_PAN._1;
                    int y = SET_PAN._2;
                    pico_output((Pico_Output) {
                        .tag = PICO_OUTPUT_SET,
                        .Set = {
                            .tag = PICO_OUTPUT_SET_PAN,
                            .Pan = { x+5, y }
                        }
                    });
                    break;
                }
                case SDLK_UP: {
                    int x = SET_PAN._1;
                    int y = SET_PAN._2;
                    pico_output((Pico_Output) {
                        .tag = PICO_OUTPUT_SET,
                        .Set = {
                            .tag = PICO_OUTPUT_SET_PAN,
                            .Pan = { x, y-5 }
                        }
                    });
                    break;
                }
                case SDLK_DOWN: {
                    int x = SET_PAN._1;
                    int y = SET_PAN._2;
                    pico_output((Pico_Output) {
                        .tag = PICO_OUTPUT_SET,
                        .Set = {
                            .tag = PICO_OUTPUT_SET_PAN,
                            .Pan = { x, y+5 }
                        }
                    });
                    break;
                }
            }
        }

#if 0
        case SDL_WINDOWEVENT: {
            if (e->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                pico_output((Pico_Output){ PICO_OUTPUT_SET_SIZE,.Set.Size={e->window.data1,e->window.data2}});
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
        if (e->type==SDL_KEYDOWN       || e->type==SDL_MOUSEBUTTONDOWN ||
            e->type==SDL_MOUSEBUTTONUP || e->type==SDL_MOUSEMOTION) {
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
            // for some reason, e->button uses physical, not logical screen
            e->button.x = _X(PHY_LOG_X(e->button.x));
            e->button.y = _Y(PHY_LOG_Y(e->button.y));
            break;
        default:
            break;
    }
    return 1;
}

// Wait until "inp" occurs.
// Saves occurred result to "out".
// 1 = event expected
// 0 = timeout
int pico_input (SDL_Event* out, Pico_Input inp) {
    switch (inp.tag) {
        case PICO_INPUT_DELAY:
            while (1) {
                int old = SDL_GetTicks();
                SDL_Event e;
                int has = SDL_WaitEventTimeout(&e, inp.Delay);
                if (has) {
                    event(&e, SDL_ANY);
                }
                int dt = SDL_GetTicks() - old;
                inp.Delay -= dt;
                if (inp.Delay <= 0) {
                    return 1;
                }
            }
            break;

        case PICO_INPUT_EVENT:
            switch (inp.Event.tag) {
                case PICO_INPUT_EVENT_POLL: {
                    int has = SDL_PollEvent(out);
                    if (!has) return 0;
                    return event(out, inp.Event.type);
                }
                case PICO_INPUT_EVENT_FOREVER:
                    while (1) {
                        SDL_WaitEvent(out);
                        if (event(out, inp.Event.type)) {
                            return 1;
                        }
                    }
                    break;
                case PICO_INPUT_EVENT_TIMEOUT:
                    while (1) {
                        int has = SDL_WaitEventTimeout(out, inp.Event.Timeout.timeout);
                        if (!has) {
                            return 0;
                        }
                        if (event(out, inp.Event.Timeout.type)) {
                            return 1;
                        }
                    }
                    break;
                default:
                    assert(0 && "bug found");
            }
            break;

        default:
            assert(0 && "bug found");
    }
    assert(0 && "bug found");
}

void pico_output (Pico_Output out) {
    switch (out.tag) {
        case PICO_OUTPUT_PRESENT:
            WIN_Present(1);
            break;
        case PICO_OUTPUT_CLEAR:
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

        case PICO_OUTPUT_DRAW:
            switch (out.Draw.tag) {
                case PICO_OUTPUT_DRAW_PIXEL: {
                    SDL_RenderDrawPoint(REN, X(out.Draw.Pixel._1), Y(out.Draw.Pixel._2) );
                    WIN_Present(0);
                    break;
                }
                case PICO_OUTPUT_DRAW_LINE: {
                    int x1 = X(out.Draw.Line.p1._1);
                    int y1 = Y(out.Draw.Line.p1._2);
                    int x2 = X(out.Draw.Line.p2._1);
                    int y2 = Y(out.Draw.Line.p2._2);
                    SDL_RenderDrawLine(REN, x1,y1, x2,y2);
                    WIN_Present(0);
                    break;
                }
                case PICO_OUTPUT_DRAW_RECT: {
                    SDL_Rect rct = { 0,0, out.Draw.Rect.size._1, out.Draw.Rect.size._2 };
                    rct.x = hanchor(X(out.Draw.Rect.pos._1),rct.w);
                    rct.y = vanchor(Y(out.Draw.Rect.pos._2),rct.h);
                    SDL_RenderFillRect(REN, &rct);
                    WIN_Present(0);
                    break;
                }
                case PICO_OUTPUT_DRAW_TEXT: {
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
                    rct.x = hanchor( X(out.Draw.Text.pos._1), rct.w );
                    rct.y = vanchor( Y(out.Draw.Text.pos._2), rct.h );

                    SDL_RenderCopy(REN, tex, NULL, &rct);
                    WIN_Present(0);

                    SDL_DestroyTexture(tex);
                    SDL_FreeSurface(sfc);
                    break;
                }
                case PICO_OUTPUT_DRAW_IMAGE: {
                    SDL_Texture* tex = IMG_LoadTexture(REN, out.Draw.Image.path);
                    pico_assert(tex != NULL);

                    int defsize = (SET_IMAGE_SIZE._1==0 && SET_IMAGE_SIZE._2==0);
                    int defcrop = (SET_IMAGE_CROP._1==0 && SET_IMAGE_CROP._2==0 && SET_IMAGE_CROP._3==0 && SET_IMAGE_CROP._4==0);

                    SDL_Rect* crp;
                    if (defcrop) {
                        crp = NULL;
                    } else {
                        crp = (SDL_Rect*) &SET_IMAGE_CROP;
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
                        rct.w = SET_IMAGE_SIZE._1;
                        rct.h = SET_IMAGE_SIZE._2;
                    }

                    // SCALE
                    rct.w = rct.w; // * GRAPHICS_SET_SCALE_W;
                    rct.h = rct.h; // * GRAPHICS_SET_SCALE_H;

                    // ANCHOR
                    rct.x = hanchor( X(out.Draw.Image.pos._1), rct.w );
                    rct.y = vanchor( Y(out.Draw.Image.pos._2), rct.h );

                    SDL_RenderCopy(REN, tex, crp, &rct);

                    WIN_Present(0);

                    SDL_DestroyTexture(tex);
                    break;
                }

                default:
                    assert(0 && "bug found");
            }
            break;

        case PICO_OUTPUT_GET:
            switch (out.Get.tag) {
                case PICO_OUTPUT_GET_SIZE: {
                    switch (out.Get.Size.tag) {
                        case PICO_OUTPUT_GET_SIZE_WINDOW:
                            SDL_GetWindowSize(WIN, &out.Get.Size.Window->_1, &out.Get.Size.Window->_2);
                            break;
                        case PICO_OUTPUT_GET_SIZE_IMAGE: {
                            SDL_Texture* tex = IMG_LoadTexture(REN, out.Get.Size.Image.path);
                            pico_assert(tex != NULL);
                            SDL_QueryTexture(tex, NULL, NULL,
                                &out.Get.Size.Image.size->_1, &out.Get.Size.Image.size->_2);
                            break;
                        }
                        default:
                            assert(0 && "bug found");
                    }
                    break;
                }
                default:
                    assert(0 && "bug found");
            }
            break;

        case PICO_OUTPUT_SET:
            switch (out.Set.tag) {
                case PICO_OUTPUT_SET_ANCHOR:
                    SET_ANCHOR = out.Set.Anchor;
                    break;
                case PICO_OUTPUT_SET_AUTO:
                    SET_AUTO = out.Set.Auto;
                    break;
                case PICO_OUTPUT_SET_COLOR:
                    switch (out.Set.Color.tag) {
                        case PICO_OUTPUT_SET_COLOR_CLEAR:
                            SET_COLOR_CLEAR = out.Set.Color.Clear;
                            break;
                        case PICO_OUTPUT_SET_COLOR_DRAW:
                            SET_COLOR_DRAW = out.Set.Color.Draw;
                            SDL_SetRenderDrawColor (REN,
                                SET_COLOR_DRAW._1,
                                SET_COLOR_DRAW._2,
                                SET_COLOR_DRAW._3,
                                SET_COLOR_DRAW._4
                            );
                            break;
                        default:
                            assert(0 && "bug found");
                    }
                    break;
                case PICO_OUTPUT_SET_CURSOR:
                    CUR_CURSOR = SET_CURSOR = out.Set.Cursor;
                    break;
                case PICO_OUTPUT_SET_FONT:
                    FNT_H = out.Set.Font.height;
                    if (FNT != NULL) {
                        TTF_CloseFont(FNT);
                    }
                    FNT = TTF_OpenFont(out.Set.Font.file, FNT_H);
                    pico_assert(FNT != NULL);
                    break;
                case PICO_OUTPUT_SET_GRID:
                    SET_GRID = out.Set.Grid;
                    break;
                case PICO_OUTPUT_SET_IMAGE:
                    switch (out.Set.Image.tag) {
                        case PICO_OUTPUT_SET_IMAGE_CROP:
                            SET_IMAGE_CROP = out.Set.Image.Crop;
                            break;
                        case PICO_OUTPUT_SET_IMAGE_SIZE:
                            SET_IMAGE_SIZE = out.Set.Image.Size;
                            break;
                        default:
                            assert(0 && "bug found");
                    }
                    break;
                case PICO_OUTPUT_SET_PAN:
                    SET_PAN = out.Set.Pan;
                    break;
                case PICO_OUTPUT_SET_SIZE:
                    SET_SIZE = out.Set.Size;
                    SDL_SetWindowSize(WIN, SET_SIZE._1, SET_SIZE._2);
                    pico_output((Pico_Output) {
                        .tag = PICO_OUTPUT_SET,
                        .Set = {
                            .tag = PICO_OUTPUT_SET_PIXEL,
                            .Pixel = { SET_PIXEL._1, SET_PIXEL._2 }
                        }
                    });
                    break;
                case PICO_OUTPUT_SET_TITLE:
                    SDL_SetWindowTitle(WIN, out.Set.Title);
                    break;
                case PICO_OUTPUT_SET_PIXEL: {
                    int w,h;
                    SDL_GetWindowSize(WIN, &w, &h);
                    SET_PIXEL = out.Set.Pixel;
                    w = MAX(1, w/SET_PIXEL._1);
                    h = MAX(1, h/SET_PIXEL._2);
                    TEX = SDL_CreateTexture (
                            REN, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
                            w, h
                    );
                    pico_assert(TEX != NULL);
                    SDL_SetRenderTarget(REN, TEX);
                    SDL_RenderSetLogicalSize(REN, w, h);
                    pico_output((Pico_Output) {
                        .tag = PICO_OUTPUT_CLEAR,
                    });
                    break;
                }
                default:
                    assert(0 && "bug found");
            }
            break;

        case PICO_OUTPUT_WRITE: {
            if (strlen(out.Write.Norm) == 0) {
                if (out.Write.tag == PICO_OUTPUT_WRITE_LINE) {
                    CUR_CURSOR._1 = SET_CURSOR._1;
                    CUR_CURSOR._2 -= FNT_H;
                }
                break;
            }

            SDL_Surface* sfc = TTF_RenderText_Blended (
                FNT, out.Write.Norm,
                (SDL_Color) { SET_COLOR_DRAW._1, SET_COLOR_DRAW._2,
                              SET_COLOR_DRAW._3, SET_COLOR_DRAW._4 }
            );
            pico_assert(sfc != NULL);
            SDL_Texture* tex = SDL_CreateTextureFromSurface(REN, sfc);
            pico_assert(tex != NULL);

            int w, h;
            TTF_SizeText(FNT, out.Write.Norm, &w,&h);
            SDL_Rect rct = { X(CUR_CURSOR._1),Y(CUR_CURSOR._2), w,h };
            SDL_RenderCopy(REN, tex, NULL, &rct);
            WIN_Present(0);

            CUR_CURSOR._1 += w;
            if (out.Write.tag == PICO_OUTPUT_WRITE_LINE) {
                CUR_CURSOR._1 = SET_CURSOR._1;
                CUR_CURSOR._2 -= FNT_H;
            }

            SDL_DestroyTexture(tex);
            SDL_FreeSurface(sfc);
            break;
        }

        default:
            assert(0 && "bug found");
            break;
    }
}

///////////////////////////////////////////////////////////////////////////////

int pico_isPointVsRect (Pico_2i pt, Pico_4i r) {
    int rw = r._3 / 2;
    int rh = r._4 / 2;
    return !(pt._1<r._1-rw || pt._1>r._1+rw || pt._2<r._2-rh || pt._2>r._2+rh);
}
