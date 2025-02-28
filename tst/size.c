#include "../src/pico.h"

int main (void) {
    pico_init(1);
    pico_set_title("Size - Fullscreen");
    Pico_Size size = pico_get_size();

    SDL_Point pt  = pico_pct_to_pos(0.5, 0.5);
    SDL_Rect  rct = {pt.x, pt.y, 32, 18};
    pico_output_draw_rect(rct);
    pico_output_present();
    pico_input_event(NULL, SDL_KEYDOWN);

    pico_set_size(PICO_SIZE_FULLSCREEN, PICO_SIZE_KEEP);

    pico_output_draw_rect(rct);
    pico_output_present();
    pico_input_event(NULL, SDL_KEYDOWN);

    pico_set_size(size.phy, (SDL_Point){128,72});

    pt  = pico_pct_to_pos(0.5, 0.5);
    rct = (SDL_Rect) {pt.x, pt.y, 32, 18};
    pico_output_draw_rect(rct);
    pico_output_draw_rect(rct);
    pico_output_present();
    pico_input_event(NULL, SDL_KEYDOWN);

    pico_set_size((SDL_Point){640,360}, size.log);

    pico_output_draw_rect(rct);
    pico_output_present();
    pico_input_event(NULL, SDL_KEYDOWN);

    pico_init(0);
    return 0;
}
