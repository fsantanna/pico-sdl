#include "pico.h"

int main (void) {
    pico_init(1);
    pico_set_title("Moving Around");
    pico_set_view_raw(-1, &(Pico_Dim){640, 360}, NULL, &(Pico_Dim){64, 18}, NULL, NULL);

    Pico_Pos pos = {32, 9};  // Center of world (50%, 50%)
    while (1) {
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF});
        pico_output_draw_pixel_raw(pos);

        pico_input_delay(200);

        //pico_set_color_draw((Pico_Color){0x00,0x00,0x00});
        //pico_output_draw_pixel_raw(pos);

        pos.x += rand()%3 - 1;
        pos.y += rand()%3 - 1;
    }

    pico_init(0);
}
