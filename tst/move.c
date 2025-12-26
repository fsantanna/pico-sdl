#include "pico.h"

int main (void) {
    pico_init(1);
    pico_set_title("Moving Around");
    pico_set_dim_window((Pico_Dim){640,360});
    pico_set_dim_world((Pico_Dim){64,18});

    Pico_Pos pos = pico_pos((Pico_Pct){50, 50});
    while (1) {
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF});
        pico_output_draw_pixel(pos);

        pico_input_delay(200);

        //pico_set_color_draw((Pico_Color){0x00,0x00,0x00});
        //pico_output_draw_pixel(pos);

        pos.x += rand()%3 - 1;
        pos.y += rand()%3 - 1;
    }

    pico_init(0);
}
