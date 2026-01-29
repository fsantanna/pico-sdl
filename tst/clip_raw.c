#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_view("Clip", -1, -1, NULL, NULL, NULL, NULL, NULL);

    puts("rect at 30%");
    pico_output_clear();
    Pico_Rel_Rect r1 = { '!', {30-25, 30-25, 50, 50}, PICO_ANCHOR_NW, NULL };
    pico_output_draw_rect(&r1);
    _pico_check("clip-01"); // same as pct

    puts("gray background");
    pico_set_view(NULL, -1, -1, NULL, NULL, NULL, NULL, &r1);
    pico_set_color_clear((Pico_Color){0xCC, 0xCC, 0xCC});
    pico_output_clear();
    _pico_check("clip-02"); // same as pct

    puts("red centered under gray");
    Pico_Rel_Rect r2 = { '!', {r1.x+r1.w/2-12, r1.y+r1.h/2-12, 25, 25}, PICO_ANCHOR_NW, NULL };
    pico_set_color_draw((Pico_Color){0xFF,0x00,0x00});
    pico_output_draw_rect(&r2);
    _pico_check("clip-03"); // same as pct

    puts("blue centered under gray");
    Pico_Rel_Pos p3 = { '!', {r1.x+r1.w/2, r1.y+r1.h/2}, PICO_ANCHOR_NW, NULL };
    pico_set_color_draw((Pico_Color){0x00,0x00,0xFF});
    pico_output_draw_pixel(&p3);
    _pico_check("clip-04"); // same as pct

    puts("yellow clipped under gray bottom right");
    Pico_Rel_Rect r4 = { '!', {r1.x+r1.w-12, r1.y+r1.h-12, 25, 25}, PICO_ANCHOR_NW, NULL };
    pico_set_color_draw((Pico_Color){0xFF,0xFF,0x00});
    pico_output_draw_rect(&r4);
    _pico_check("clip-05"); // same as pct

    pico_init(0);
    return 0;
}
