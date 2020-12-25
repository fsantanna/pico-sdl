#include "../pico.h"

int main (void) {
    init();

    // TITLE
    output((Output) { SET, .Set={TITLE,.Title="Hello World!"} });

    // CLEAR
    output((Output) { SET, .Set={COLOR_BG,.Color_BG={0xFF,0xFF,0xFF,0x00}} });
    output((Output) { CLEAR });
    input((Input){ DELAY, .Delay=2000 });

    // DRAW_PIXEL
    output((Output) { SET, .Set={COLOR_BG,.Color_BG={0x00,0x00,0x00,0x00}} });
    output((Output) { SET, .Set={COLOR_FG,.Color_FG={0xFF,0xFF,0xFF,0x00}} });
    output((Output) { CLEAR });
    output((Output) { DRAW, .Draw={PIXEL,.Pixel={0,0}} });
    input((Input){ DELAY, .Delay=2000 });

    // EVENT
    SDL_Event e;
    int ok = input((Input){ EVENT, .Event={SDL_KEYUP,0,&e} });
    assert(ok);

    return 0;
}
