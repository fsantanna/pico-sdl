#include "../src/pico.h"

int main (void) {
    pico_init(1);
    pico_set_title("Moving Around");

    SDL_Point pt = pico_pct_to_point(0.5, 0.5);
    while (1) {
        pico_output_draw_pixel(pt);

        pico_output_present();
        pico_input_delay(200);

        pt.y--;
    }

    pico_init(0);
}
