#include "../pico.h"

int main (void) {
    pico_init(1);
    pico_state_set_title("Moving Around");

    int x=0, y=0;
    while (1) {
        pico_state_set_color_draw((SDL_Color){0xFF,0xFF,0xFF,0xFF});
        pico_output_draw_pixel((SDL_Point){x,y});

        pico_input_delay(200);

        pico_state_set_color_draw((SDL_Color){0x00,0x00,0x00,0xFF});
        pico_output_draw_pixel((SDL_Point){x,y});

        y = y+1;
    }

    pico_init(0);
}
