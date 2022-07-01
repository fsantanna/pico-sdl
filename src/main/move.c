#include "../pico.h"

int main (void) {
    pico_open();

    pico_output((Pico_Output) {
        .tag = PICO_OUTPUT_SET,
        .Set = {
            .tag = PICO_OUTPUT_SET_TITLE,
            .Title = "pico-SDL: Moving Around"
        }
    });

    int x=0, y=0;

    while (1) {
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
        pico_output((Pico_Output) {
            .tag = PICO_OUTPUT_DRAW,
            .Draw = {
                .tag = PICO_OUTPUT_DRAW_PIXEL,
                .Pixel = {x,y}
            }
        });

        pico_input(NULL, (Pico_Input){ .tag=PICO_INPUT_DELAY, .Delay=2000 });

        pico_output((Pico_Output) {
            .tag = PICO_OUTPUT_SET,
            .Set = {
                .tag = PICO_OUTPUT_SET_COLOR,
                .Color = {
                    .tag = PICO_OUTPUT_SET_COLOR_DRAW,
                    .Draw = {0x00,0x00,0x00,0xFF}
                }
            }
        });
        pico_output((Pico_Output) {
            .tag = PICO_OUTPUT_DRAW,
            .Draw = {
                .tag = PICO_OUTPUT_DRAW_PIXEL,
                .Pixel = {x,y}
            }
        });

        y = y+1;
    }
}
