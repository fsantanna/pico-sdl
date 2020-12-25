#include "../pico.h"

int main (void) {
    init();

    // CLEAR
    output((Output) { SET, .Set={COLOR_BG,.Color_BG={0xFF,0xFF,0xFF,0x00}} });
    output((Output) { CLEAR });
    input((Input){ DELAY, .Delay=2000 });

    // DRAW_PIXEL
    output((Output) { SET, .Set={COLOR_BG,.Color_BG={0x00,0x00,0x00,0x00}} });
    output((Output) { SET, .Set={COLOR_FG,.Color_FG={0xFF,0xFF,0xFF,0x00}} });
    output((Output) { CLEAR });
    output((Output) { DRAW, .Draw={PIXEL,.Pixel={0,0}} });
    input((Input){ DELAY, .Delay=5000 });

    return 0;
}
