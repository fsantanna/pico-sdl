#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_window_title("Clip");

    puts("rect at 30%");
    pico_output_clear();
    Pico_Rel_Rect r1 = { '%', {0.3,0.3, 0.5,0.5}, PICO_ANCHOR_C };
    pico_output_draw_rect(r1);
    _pico_check("clip-01");     // same as raw

    pico_set_scene_clip(r1);
    pico_set_effect_color((Pico_Color){0xCC, 0xCC, 0xCC, 0xFF});
    pico_output_clear();
    _pico_check("clip-02");     // same as raw

    puts("red centered under white");
    Pico_Rel_Rect r2 = pico_in_rect(r1, (Pico_Rel_Rect){ '%', {0.5,0.5, 0.5,0.5}, PICO_ANCHOR_C });
    pico_set_pencil_color((Pico_Color){0xFF,0x00,0x00, 0xFF});
    pico_output_draw_rect(r2);
    _pico_check("clip-03");     // same as raw

    puts("blue centered under white");
    Pico_Rel_Pos pt3 = pico_in_pos(r1, (Pico_Rel_Pos){ '%', {0.5,0.5}, PICO_ANCHOR_C });
    pico_set_pencil_color((Pico_Color){0x00,0x00,0xFF, 0xFF});
    pico_output_draw_pixel(pt3);
    _pico_check("clip-04");     // same as raw

    puts("yellow clipped under white bottom right");
    Pico_Rel_Rect r4 = pico_in_rect(r1, (Pico_Rel_Rect){ '%', {1,1, 0.5,0.5}, PICO_ANCHOR_C });
    pico_set_pencil_color((Pico_Color){0xFF,0xFF,0x00, 0xFF});
    pico_output_draw_rect(r4);
    _pico_check("clip-05");     // same as raw

    pico_init(0);
    return 0;
}
