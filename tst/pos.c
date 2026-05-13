#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_window_title("Pct-To-Pos");

    {
        puts("white centered rect");
        Pico_Rel_Rect r = { '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C };
        pico_output_clear();
        pico_output_draw_rect(&r);
        _pico_check("pos-01");
    }
    {
        pico_output_clear();

        puts("white rect at 30%");
        Pico_Rel_Rect r1 = { '%', {0.3, 0.3, 0.5, 0.5}, PICO_ANCHOR_C };
        pico_output_draw_rect(&r1);

        puts("red centered under white");
        Pico_Rel_Rect r2 = pico_in_rect(r1, (Pico_Rel_Rect){ '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C });
        pico_set_pencil_color((Pico_Color){0xFF,0x00,0x00, 0xFF});
        pico_output_draw_rect(&r2);

        _pico_check("pos-02");
    }
    {
        pico_output_clear();

        puts("rect at 50% anchored by bottom-right");
        Pico_Rel_Rect r1 = { '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_SE };
        pico_set_pencil_color((Pico_Color){0xFF,0xFF,0xFF, 0xFF});
        pico_output_draw_rect(&r1);

        puts("red anchored by top-left under 0% of white");
        Pico_Rel_Rect r2 = pico_in_rect(r1, (Pico_Rel_Rect){ '%', {0, 0, 0.5, 0.5}, PICO_ANCHOR_NW });
        pico_set_pencil_color((Pico_Color){0xFF,0x00,0x00, 0xFF});
        pico_output_draw_rect(&r2);

        _pico_check("pos-03");
    }
    {
        puts("rect at -10/-10 top-left (4x7 rect on top)");
        pico_output_clear();
        Pico_Rel_Rect r = { '%', {-0.1, -0.1, 0.15, 0.3}, PICO_ANCHOR_NW };
        pico_set_pencil_color((Pico_Color){0xFF,0xFF,0xFF, 0xFF});
        pico_output_draw_rect(&r);
        _pico_check("pos-04");
    }
    {
        puts("rect at 110/110 bottom-right (symmetric to previous)");
        pico_output_clear();
        Pico_Rel_Rect r = { '%', {1.1, 1.1, 0.15, 0.3}, PICO_ANCHOR_SE };
        pico_set_pencil_color((Pico_Color){0xFF,0xFF,0xFF, 0xFF});
        pico_output_draw_rect(&r);
        _pico_check("pos-05");
    }
    {
        pico_output_clear();

        puts("rect at 50% anchored by center");
        Pico_Rel_Rect r1 = { '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C };
        pico_set_pencil_color((Pico_Color){0xFF,0xFF,0xFF, 0xFF});
        pico_output_draw_rect(&r1);

        puts("red anchored by top-left under -10% of white");
        Pico_Rel_Rect r2 = pico_in_rect(r1, (Pico_Rel_Rect){ '%', {-0.1, -0.1, 0.5, 0.5}, PICO_ANCHOR_NW });
        pico_set_pencil_color((Pico_Color){0xFF,0x00,0x00, 0xFF});
        pico_output_draw_rect(&r2);

        _pico_check("pos-06");
    }
    {
        pico_output_clear();

        puts("rect at 50%");
        Pico_Rel_Rect r1 = { '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C };
        pico_set_pencil_color((Pico_Color){0xFF,0xFF,0xFF, 0xFF});
        pico_output_draw_rect(&r1);

        puts("red anchored by top-left under 110% of white (symmetric to previous)");
        Pico_Rel_Rect r2 = pico_in_rect(r1, (Pico_Rel_Rect){ '%', {1.1, 1.1, 0.5, 0.5}, PICO_ANCHOR_SE });
        pico_set_pencil_color((Pico_Color){0xFF,0x00,0x00, 0xFF});
        pico_output_draw_rect(&r2);

        _pico_check("pos-07");
    }

    pico_init(0);
    return 0;
}
