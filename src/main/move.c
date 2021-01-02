#include "../pico.h"

int main (void) {
    pico_init();

    pico_output((Pico_Output) { PICO_SET, .Set={PICO_TITLE,.Title="pico-SDL: Moving Around"} });

    int x=0, y=0;

    while (1) {
        pico_output((Pico_Output) { PICO_SET, .Set={PICO_COLOR,.Color={PICO_COLOR_DRAW,.Draw={0xFF,0xFF,0xFF,0xFF}}} });
        pico_output((Pico_Output) { PICO_DRAW, .Draw={PICO_PIXEL,.Pixel={x,y}} });

        pico_input((Pico_Input){ PICO_DELAY, .Delay=2000 });

        pico_output((Pico_Output) { PICO_SET, .Set={PICO_COLOR,.Color={PICO_COLOR_DRAW,.Draw={0x00,0x00,0x00,0xFF}}} });
        pico_output((Pico_Output) { PICO_DRAW, .Draw={PICO_PIXEL,.Pixel={x,y}} });

        y = y+1;
    }
}
