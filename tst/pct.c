#include "../src/pico.h"

int main (void) {
    pico_init(1);
    pico_set_title("Pct-To-Pos");

    {
        SDL_Point pt  = pico_pct_to_pos(0.5, 0.5);
        SDL_Rect  rct = {pt.x, pt.y, 32, 18};
        pico_output_draw_rect(rct);
        pico_output_present();                      // centered rect
        pico_input_event(NULL, SDL_KEYDOWN);
    }

    {
        SDL_Point pt1  = pico_pct_to_pos(0.3, 0.3);
        SDL_Rect  rct1 = {pt1.x, pt1.y, 32, 18};
        pico_output_draw_rect(rct1);

        SDL_Point pt2  = pico_pct_to_pos_x(rct1, 0.5, 0.5);
        SDL_Rect  rct2 = {pt2.x, pt2.y, 16, 9};
        pico_set_color_draw((SDL_Color){0xFF,0x00,0x00,0xFF});
        pico_output_draw_rect(rct2);

        pico_output_present();                      // centered at 0.3,0.3
        pico_input_event(NULL, SDL_KEYDOWN);
    }

    pico_init(0);
    return 0;
}
