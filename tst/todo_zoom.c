#include "pico.h"

int main (void) {
    pico_init(1);

    Pico_Dim window, world;
    pico_get_view(NULL, &window, NULL, &world, NULL, NULL);
    assert(window.w==500 && window.h==500);
    assert(world.w==100 && world.h==100);

    // DRAW_RECT
    puts("shows lower-left X, center rect, center/up-right line");
    puts("increases zoom");
    for (int i=1; i<=20; i++) {
        world.w -= 1;
        world.h -= 1;
        pico_set_view_raw(-1, NULL, NULL, &world, NULL, NULL);
        pico_output_clear();
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF});
        pico_output_draw_rect_raw((Pico_Rect){world.w/2, world.h/2, 10, 10});
        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00});
        pico_output_draw_text_pct(&(Pico_Rect_Pct){0.25,0.75,0,0,PICO_ANCHOR_C,NULL}, "X");
        pico_output_draw_line_pct(&(Pico_Pos_Pct){0.5,0.5,PICO_ANCHOR_C,NULL}, &(Pico_Pos_Pct){1.0,0,PICO_ANCHOR_C,NULL});
        pico_input_delay(250);
    }
    puts("decreases zoom");
    for (int i=1; i<=20; i++) {
        world.w += 1;
        world.h += 1;
        pico_set_view_raw(-1, NULL, NULL, &world, NULL, NULL);
        pico_output_clear();
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF});
        pico_output_draw_rect_raw((Pico_Rect){world.w/2, world.h/2, 10, 10});
        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00});
        pico_output_draw_text_pct(&(Pico_Rect_Pct){0.25,0.75,0,0,PICO_ANCHOR_C,NULL}, "X");
        pico_output_draw_line_pct(&(Pico_Pos_Pct){0.5,0.5,PICO_ANCHOR_C,NULL}, &(Pico_Pos_Pct){1.0,0,PICO_ANCHOR_C,NULL});
        pico_input_delay(250);
    }
    pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF});

    // PAN

    puts("scrolls right/down");
    for (int i=0; i<20; i++) {
        pico_set_view_raw(-1, NULL, NULL, NULL, &(SDL_Rect){10-i, 10-i, world.w, world.h}, NULL);
        pico_output_clear();
        pico_output_draw_text_pct(&(Pico_Rect_Pct){0.5,0.5,0,0,PICO_ANCHOR_C,NULL}, "Uma frase bem grande...");
        pico_input_delay(250);
    }

    pico_init(0);
    return 0;
}
