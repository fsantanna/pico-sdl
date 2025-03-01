#include "../src/pico.h"

int main (void) {
    pico_init(1);
    pico_set_title("Pct-To-Pos");

    {
        puts("centered rect");
        Pico_Pos  pt  = pico_pct_to_pos(50, 50);
        Pico_Rect rct = {pt.x, pt.y, 32, 18};
        pico_output_clear();
        pico_output_draw_rect(rct);
        pico_input_event(NULL, SDL_KEYDOWN);
    }

    {
        puts("rect at 30%");
        pico_output_clear();

        Pico_Pos  pt1  = pico_pct_to_pos(30, 30);
        Pico_Rect rct1 = {pt1.x, pt1.y, 32, 18};
        pico_output_draw_rect(rct1);

        puts("red centered under white");
        Pico_Pos  pt2  = pico_pct_to_pos_ext(rct1, 50, 50);
        Pico_Rect rct2 = {pt2.x, pt2.y, 16, 9};
        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00,0xFF});
        pico_output_draw_rect(rct2);

        pico_input_event(NULL, SDL_KEYDOWN);
    }

    pico_init(0);
    return 0;
}
