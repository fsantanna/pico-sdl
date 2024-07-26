#include "../src/pico.h"

int main (void) {
    pico_init(1);
    pico_set_title("Size - Fullscreen");

    SDL_Point pt  = pico_pct_to_point(0.5, 0.5);
    SDL_Rect  rct = {pt.x, pt.y, 32, 18};
    pico_output_draw_rect(rct);
    pico_output_present();
    pico_input_event(NULL, SDL_KEYDOWN);

    pico_set_fullscreen(1);

    pico_output_draw_rect(rct);
    pico_output_present();
    pico_input_event(NULL, SDL_KEYDOWN);

    pico_set_fullscreen(0);

    pico_output_draw_rect(rct);
    pico_output_present();
    pico_input_event(NULL, SDL_KEYDOWN);

    pico_init(0);
    return 0;
}
