#include "../pico.h"

int main (void) {
    pico_open();

    pico_output((Pico_IO) { PICO_SET_TITLE,.Set_Title="pico-SDL: Moving Around" });

    int x=0, y=0;

    while (1) {
        pico_output((Pico_IO) { PICO_SET_COLOR_DRAW, .Set_Color_Draw={0xFF,0xFF,0xFF,0xFF} });
        pico_output((Pico_IO) { PICO_DRAW_PIXEL,.Draw_Pixel={x,y} });

        pico_input(NULL, (Pico_IO){ PICO_DELAY, .Delay=2000 });

        pico_output((Pico_IO) { PICO_SET_COLOR_DRAW, .Set_Color_Draw={0x00,0x00,0x00,0xFF} });
        pico_output((Pico_IO) { PICO_DRAW_PIXEL, .Draw_Pixel={x,y} });

        y = y+1;
    }
}
