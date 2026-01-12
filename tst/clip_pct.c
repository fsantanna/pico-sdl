#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_title("Clip");

    puts("rect at 30%");
    pico_output_clear();
    Pico_Rect_Pct r1 = { 0.3,0.3, 0.5,0.5, PICO_ANCHOR_C, NULL };
    pico_output_draw_rect_pct(&r1);
    _pico_check("clip-01");     // same as raw

    pico_set_view_pct(-1, NULL, NULL, NULL, NULL, &r1);
    pico_set_color_clear((Pico_Color){0xCC, 0xCC, 0xCC});
    pico_output_clear();
    _pico_check("clip-02");     // same as raw

    puts("red centered under white");
    Pico_Rect_Pct r2 = { 0.5,0.5, 0.5,0.5, PICO_ANCHOR_C, &r1 };
    pico_set_color_draw((Pico_Color){0xFF,0x00,0x00});
    pico_output_draw_rect_pct(&r2);
    _pico_check("clip-03");     // same as raw

    puts("blue centered under white");
    Pico_Pos_Pct pt3 = { 0.5,0.5, PICO_ANCHOR_C, &r1 };
    pico_set_color_draw((Pico_Color){0x00,0x00,0xFF});
    pico_output_draw_pixel_pct(&pt3);
    _pico_check("clip-04");     // same as raw

    puts("yellow clipped under white bottom right");
    Pico_Rect_Pct r4 = { 1,1, 0.5,0.5, PICO_ANCHOR_C, &r1 };
    pico_set_color_draw((Pico_Color){0xFF,0xFF,0x00});
    pico_output_draw_rect_pct(&r4);
    _pico_check("clip-05");     // same as raw

    pico_init(0);
    return 0;
}
