#include "pico.h"
#include "tst.c"

int main (void) {
    pico_init(1);
    pico_set_title("Clip");

    puts("rect at 30%");
    pico_output_clear();
    Pico_Rect r1 = { 30-25, 30-25, 50, 50 };
    pico_output_draw_rect_raw(r1);
    pico_input_event(NULL, PICO_KEYDOWN);

    pico_set_view_raw(-1, NULL, NULL, NULL, NULL, &r1);
    pico_set_color_clear((Pico_Color){0xCC, 0xCC, 0xCC});
    pico_output_clear();
    pico_input_event(NULL, PICO_KEYDOWN);

    puts("red centered under white");
    Pico_Rect r2 = { r1.x+r1.w/2-12, r1.y+r1.h/2-12, 25, 25 };
    pico_set_color_draw((Pico_Color){0xFF,0x00,0x00});
    pico_output_draw_rect_raw(r2);
    pico_input_event(NULL, PICO_KEYDOWN);

    puts("blue centered under white");
    Pico_Pos p3 = { r1.x+r1.w/2, r1.y+r1.h/2 };
    pico_set_color_draw((Pico_Color){0x00,0x00,0xFF});
    pico_output_draw_pixel_raw(p3);
    pico_input_event(NULL, PICO_KEYDOWN);

    puts("yellow clipped under white bottom right");
    Pico_Rect r4 = { r1.x+r1.w-12, r1.y+r1.h-12, 25, 25 };
    pico_set_color_draw((Pico_Color){0xFF,0xFF,0x00});
    pico_output_draw_rect_raw(r4);
    pico_input_event(NULL, PICO_KEYDOWN);
    //_pico_check("pct_rect30_inner50");

    pico_init(0);
    return 0;
}
