#include "../pico.h"

int main (void) {
    pico_open();

    SDL_Event e1;
    int ok = pico_input(&e1, (Pico_Input) {
        .tag = PICO_INPUT_EVENT,
        .Event = {
            .tag = PICO_INPUT_EVENT_FOREVER,
            .type = SDL_KEYUP
        }
    });
    assert(ok);

    // TITLE
    pico_output((Pico_Output) {
        .tag = PICO_OUTPUT_SET,
        .Set = {
            .tag = PICO_OUTPUT_SET_TITLE,
            .Title = "Hello World!"
        }
    });

    // CLEAR
    pico_output((Pico_Output) {
        .tag = PICO_OUTPUT_SET,
        .Set = {
            .tag = PICO_OUTPUT_SET_COLOR,
            .Color = {
                .tag = PICO_OUTPUT_SET_COLOR_CLEAR,
                .Clear = {0xFF,0xFF,0xFF,0xFF}
            }
        }
    });
    pico_output((Pico_Output) { .tag=PICO_OUTPUT_CLEAR });

    pico_input(NULL, (Pico_Input){ .tag=PICO_INPUT_DELAY, .Delay=2000 });

    // DRAW_IMAGE
    pico_output((Pico_Output) {
        .tag = PICO_OUTPUT_SET,
        .Set = {
            .tag = PICO_OUTPUT_SET_ANCHOR,
            .Anchor = {Center,Middle}
        }
    });
    pico_output((Pico_Output) {
        .tag = PICO_OUTPUT_DRAW,
        .Draw = {
            .tag = PICO_OUTPUT_DRAW_IMAGE,
            .Image = {{0,0},"open.png"}
        }
    });

    pico_input(NULL, (Pico_Input){ .tag=PICO_INPUT_DELAY, .Delay=2000 });

    // DRAW_PIXEL
    pico_output((Pico_Output) {
        .tag = PICO_OUTPUT_SET,
        .Set = {
            .tag = PICO_OUTPUT_SET_COLOR,
            .Color = {
                .tag = PICO_OUTPUT_SET_COLOR_CLEAR,
                .Clear = {0x00,0x00,0x00,0xFF}
            }
        }
    });
    pico_output((Pico_Output) {
        .tag = PICO_OUTPUT_SET,
        .Set = {
            .tag = PICO_OUTPUT_SET_COLOR,
            .Color = {
                .tag = PICO_OUTPUT_SET_COLOR_DRAW,
                .Draw = {0xFF,0xFF,0xFF,0xFF}
            }
        }
    });
    pico_output((Pico_Output) { .tag=PICO_OUTPUT_CLEAR });
    pico_output((Pico_Output) {
        .tag = PICO_OUTPUT_DRAW,
        .Draw = {
            .tag = PICO_OUTPUT_DRAW_PIXEL,
            .Pixel={0,0}
        }
    });

    pico_input(NULL, (Pico_Input){ .tag=PICO_INPUT_DELAY, .Delay=2000 });

    // DRAW_TEXT
    pico_output((Pico_Output) {
        .tag = PICO_OUTPUT_DRAW,
        .Draw = {
            .tag = PICO_OUTPUT_DRAW_TEXT,
            .Text = {{0,0},"Hello!"}
        }
    });

    pico_input(NULL, (Pico_Input){ .tag=PICO_INPUT_DELAY, .Delay=2000 });
    pico_output((Pico_Output) { .tag=PICO_OUTPUT_CLEAR });

    // WRITE
    pico_output((Pico_Output) {
        .tag = PICO_OUTPUT_SET,
        .Set = {
            .tag = PICO_OUTPUT_SET_CURSOR,
            .Cursor={-25,25}
        }
    });
    pico_output((Pico_Output) {
        .tag = PICO_OUTPUT_WRITE,
        .Write = {
            .tag = PICO_OUTPUT_WRITE_NORM,
            .Norm = "1 "
        }
    });
    pico_output((Pico_Output) {
        .tag = PICO_OUTPUT_WRITE,
        .Write = {
            .tag = PICO_OUTPUT_WRITE_NORM,
            .Norm = "2 "
        }
    });
    pico_output((Pico_Output) {
        .tag = PICO_OUTPUT_WRITE,
        .Write = {
            .tag = PICO_OUTPUT_WRITE_LINE,
            .Line = "3"
        }
    });
    pico_output((Pico_Output) {
        .tag = PICO_OUTPUT_WRITE,
        .Write = {
            .tag = PICO_OUTPUT_WRITE_LINE,
            .Line = ""
        }
    });
    pico_output((Pico_Output) {
        .tag = PICO_OUTPUT_WRITE,
        .Write = {
            .tag = PICO_OUTPUT_WRITE_LINE,
            .Line = "pico"
        }
    });

    pico_input(NULL, (Pico_Input){ .tag=PICO_INPUT_DELAY, .Delay=2000 });

    // MOUSE
    SDL_Event e2;
    pico_input(&e2, (Pico_Input){
        .tag = PICO_INPUT_EVENT,
        .Event = {
            .tag = PICO_INPUT_EVENT_FOREVER,
            .type = SDL_MOUSEBUTTONDOWN
        }
    });
    pico_output((Pico_Output) {
        .tag = PICO_OUTPUT_DRAW,
        .Draw = {
            .tag = PICO_OUTPUT_DRAW_PIXEL,
            .Pixel = {e2.button.x,e2.button.y}
        }
    });

    pico_input(NULL, (Pico_Input){ .tag=PICO_INPUT_DELAY, .Delay=2000 });

    // EVENT
    SDL_Event e3;
    pico_input(&e3, (Pico_Input){
        .tag = PICO_INPUT_EVENT,
        .Event = {
            .tag = PICO_INPUT_EVENT_TIMEOUT,
            .Timeout = {SDL_ANY,5000}
        }
    });

    // GET SIZE
    Pico_2i size;
    pico_output((Pico_Output) {
        .tag = PICO_OUTPUT_GET,
        .Get = {
            .tag = PICO_OUTPUT_GET_SIZE,
            .Size = {
                .tag = PICO_OUTPUT_GET_SIZE_WINDOW,
                .Window = &size
            }
        }
    });
    printf("SIZE=(%d,%d)\n", size._1,size._2);

    // GRID=0
    pico_output((Pico_Output){
        .tag = PICO_OUTPUT_SET,
        .Set = {
            .tag = PICO_OUTPUT_SET_GRID,
            .Grid = 0
        }
    });

    // AUTO=0
    pico_output((Pico_Output){ .tag=PICO_OUTPUT_CLEAR });
    pico_output((Pico_Output){
        .tag = PICO_OUTPUT_SET,
        .Set = {
            .tag = PICO_OUTPUT_SET_AUTO,
            .Auto = 0
        }
    });
    pico_output((Pico_Output) {
        .tag = PICO_OUTPUT_WRITE,
        .Write = {
            .tag = PICO_OUTPUT_WRITE_LINE,
            .Line = "no auto"
        }
    });
    pico_output((Pico_Output) { .tag=PICO_OUTPUT_PRESENT });

    pico_input(NULL, (Pico_Input){ .tag=PICO_INPUT_DELAY, .Delay=2000 });

    // AUTO=1
    pico_output((Pico_Output){
        .tag = PICO_OUTPUT_SET,
        .Set = {
            .tag = PICO_OUTPUT_SET_AUTO,
            .Auto = 1
        }
    });
    pico_output((Pico_Output){ .tag=PICO_OUTPUT_CLEAR });

    // PIXEL
    for (int i=1; i<=20; i++) {
        pico_output((Pico_Output) {
            .tag = PICO_OUTPUT_SET,
            .Set = {
                .tag = PICO_OUTPUT_SET_SIZE,
                .Size = {_WIN_,_WIN_},
            },
        });
        pico_output((Pico_Output){
            .tag = PICO_OUTPUT_SET,
            .Set = {
                .tag = PICO_OUTPUT_SET_PIXEL,
                .Pixel = {i,i}
            }
        });
        pico_output((Pico_Output){ .tag=PICO_OUTPUT_CLEAR });
        pico_output((Pico_Output) {
            .tag = PICO_OUTPUT_DRAW,
            .Draw = {
                .tag = PICO_OUTPUT_DRAW_TEXT,
                .Text = {{0,0},"X"}
            }
        });
        pico_output((Pico_Output) {
            .tag = PICO_OUTPUT_DRAW,
            .Draw = {
                .tag = PICO_OUTPUT_DRAW_RECT,
                .Rect = {{0,0},{25,25}}
            }
        });
        pico_output((Pico_Output) {
            .tag = PICO_OUTPUT_SET,
            .Set = {
                .tag = PICO_OUTPUT_SET_COLOR,
                .Color = {
                    .tag = PICO_OUTPUT_SET_COLOR_DRAW,
                    .Draw = {0xFF,0x00,0x00,0xFF}
                }
            }
        });
        pico_output((Pico_Output) {
            .tag = PICO_OUTPUT_DRAW,
            .Draw = {
                .tag = PICO_OUTPUT_DRAW_LINE,
                .Line= {{0,0},{25,25}}
            }
        });
        pico_output((Pico_Output) {
            .tag = PICO_OUTPUT_SET,
            .Set = {
                .tag = PICO_OUTPUT_SET_COLOR,
                .Color = {
                    .tag = PICO_OUTPUT_SET_COLOR_DRAW,
                    .Draw = {0xFF,0xFF,0xFF,0xFF}
                }
            }
        });
        pico_input(NULL, (Pico_Input){ .tag=PICO_INPUT_DELAY, .Delay=200 });
    }

    // PAN

    for (int i=0; i<10; i++) {
        pico_output((Pico_Output){
            .tag = PICO_OUTPUT_SET,
            .Set = {
                .tag = PICO_OUTPUT_SET_PAN,
                .Pan = {i,0}
            }
        });
        pico_output((Pico_Output){ .tag=PICO_OUTPUT_CLEAR });
        pico_output((Pico_Output) {
            .tag = PICO_OUTPUT_DRAW,
            .Draw = {
                .tag = PICO_OUTPUT_DRAW_TEXT,
                .Text = {{0,0},"Uma frase bem grande"}
            }
        });
        pico_input(NULL, (Pico_Input){ .tag=PICO_INPUT_DELAY, .Delay=500 });
    }

    for (int i=0; i<20; i++) {
        pico_output((Pico_Output){
            .tag = PICO_OUTPUT_SET,
            .Set = {
                .tag = PICO_OUTPUT_SET_PAN,
                .Pan = {10-i,-i}
            }
        });
        pico_output((Pico_Output){ .tag=PICO_OUTPUT_CLEAR });
        pico_output((Pico_Output) {
            .tag = PICO_OUTPUT_DRAW,
            .Draw = {
                .tag = PICO_OUTPUT_DRAW_TEXT,
                .Text = {{0,0},"Uma frase bem grande"}
            }
        });
        pico_input(NULL, (Pico_Input){ .tag=PICO_INPUT_DELAY, .Delay=500 });
    }

    pico_close();
    return 0;
}
