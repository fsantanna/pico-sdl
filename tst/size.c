#include "pico.h"

int main (void) {
    pico_init(1);
    pico_set_title("Size - Fullscreen");
    Pico_Size size = pico_get_size();
    Pico_Pos pt1 = pico_pct_to_pos(50, 50);
    Pico_Rect rct1 = {pt1.x, pt1.y, 32, 18};

    puts("no fullscreen - less pixels");
    pico_output_draw_rect(rct1);

    pico_input_event(NULL, SDL_KEYDOWN);

    puts("ok fullscreen - less pixels");
    pico_set_size(PICO_SIZE_FULLSCREEN, PICO_SIZE_KEEP);
    pico_output_draw_rect(rct1);

    pico_input_event(NULL, SDL_KEYDOWN);

    puts("no fullscreen - more pixels");
    pico_set_size(size.phy, (Pico_Dim){128,72});
    Pico_Pos pt2  = pico_pct_to_pos(50, 50);
    Pico_Rect rct2 = {pt2.x, pt2.y, 32, 18};
    pico_output_draw_rect(rct2);

    pico_input_event(NULL, SDL_KEYDOWN);

    puts("no fullscreen - less pixels");
    pico_set_size((Pico_Dim){640,360}, size.log);
    pico_output_draw_rect(rct1);

    pico_input_event(NULL, SDL_KEYDOWN);

    pico_init(0);
    return 0;
}
