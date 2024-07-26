#include "../src/pico.h"

int main (void) {
    pico_init(1);
    pico_set_grid(0);

    SDL_Point log = { 301, 301 };
    SDL_Point phy = { 301, 301 };
    pico_set_size_window(log, phy);

    pico_output_draw_rect((SDL_Rect){0,0,100,100});
    pico_output_present();
    pico_input_event(NULL, SDL_KEYDOWN);

    debug();
    pico_set_size_fullscreen(1);
    debug();

    pico_output_draw_rect((SDL_Rect){0,0,100,100});
    pico_output_present();
    pico_input_event(NULL, SDL_KEYDOWN);

    debug();
    pico_set_size_fullscreen(0);
    debug();

    pico_output_draw_rect((SDL_Rect){0,0,100,100});
    pico_output_present();
    pico_input_event(NULL, SDL_KEYDOWN);

    pico_init(0);
    return 0;
}
