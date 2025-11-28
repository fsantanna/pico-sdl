#include "pico.h"

int main (void) {
    pico_init(1);
    pico_set_title("Size - Fullscreen");
    Pico_Pos pt1 = pico_pos((Pico_Pct){50, 50});
    Pico_Rect rct1 = {pt1.x, pt1.y, 32, 18};

    puts("no fullscreen - less pixels");
    pico_set_fullscreen(0);
    pico_output_clear();
    pico_output_draw_rect(rct1);

    pico_input_event(NULL, PICO_KEYDOWN);

    puts("ok fullscreen - less pixels");
    pico_set_fullscreen(1);
    pico_output_clear();
    pico_output_draw_rect(rct1);

    pico_input_event(NULL, PICO_KEYDOWN);

    puts("no fullscreen - less pixels");
    pico_set_fullscreen(0);
    pico_output_clear();
    pico_output_draw_rect(rct1);

    pico_input_event(NULL, PICO_KEYDOWN);

    puts("no fullscreen - more pixels");
    pico_set_fullscreen(0);
    pico_set_dim_world((Pico_Dim){128,72});
    Pico_Pos pt2  = pico_pos((Pico_Pct){50, 50});
    Pico_Rect rct2 = {pt2.x, pt2.y, 32, 18};
    pico_output_clear();
    pico_output_draw_rect(rct2);

    pico_input_event(NULL, PICO_KEYDOWN);

    puts("no fullscreen - less pixels");
    pico_set_fullscreen(0);
    pico_set_dim_window((Pico_Dim){640,360});
    pico_output_clear();
    pico_output_draw_rect(rct1);

    pico_input_event(NULL, PICO_KEYDOWN);

    pico_init(0);
    return 0;
}
