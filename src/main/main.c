#include "../pico.h"

int main (void) {
    pico_init();

    // TITLE
    pico_output((Pico_Output) { PICO_SET, .Set={PICO_TITLE,.Title="Hello World!"} });

    // CLEAR
    pico_output((Pico_Output) { PICO_SET, .Set={PICO_COLOR_BG,.Color_BG={0xFF,0xFF,0xFF,0x00}} });
    pico_output((Pico_Output) { PICO_CLEAR });
    pico_input((Pico_Input){ PICO_DELAY, .Delay=2000 });

    // DRAW_PIXEL
    pico_output((Pico_Output) { PICO_SET, .Set={PICO_COLOR_BG,.Color_BG={0x00,0x00,0x00,0x00}} });
    pico_output((Pico_Output) { PICO_SET, .Set={PICO_COLOR_FG,.Color_FG={0xFF,0xFF,0xFF,0x00}} });
    pico_output((Pico_Output) { PICO_CLEAR });
    pico_output((Pico_Output) { PICO_DRAW, .Draw={PICO_PIXEL,.Pixel={0,0}} });
    pico_input((Pico_Input){ PICO_DELAY, .Delay=2000 });

    // EVENT
    SDL_Event e;
    int ok = pico_input((Pico_Input){ PICO_EVENT, .Event={SDL_KEYUP,0,&e} });
    assert(ok);

    return 0;
}
