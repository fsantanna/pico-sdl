#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_view("Rect", -1, -1, NULL, NULL, NULL, NULL, NULL);

    {
        puts("centered rect");
        pico_output_clear();
        pico_output_draw_rect(&(Pico_Rel_Rect){ '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, NULL });
        _pico_check("rect-01");
    }

    {
        puts("rect at 30%");
        pico_output_clear();
        Pico_Rel_Rect r1 = { '%', {0.3, 0.3, 0.5, 0.5}, PICO_ANCHOR_C, NULL };
        pico_output_draw_rect(&r1);

        puts("red centered under white");
        Pico_Rel_Rect r2 = { '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, &r1 };
        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00});
        pico_output_draw_rect(&r2);

        _pico_check("rect-02");
    }

    {
        puts("rect at 50% anchored by bottom-right");
        pico_output_clear();

        Pico_Rel_Rect r1 = { '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_SE, NULL };
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF});
        pico_output_draw_rect(&r1);

        puts("red anchored by top-left under 0% of white");
        Pico_Rel_Rect r2 = { '%', {0, 0, 0.5, 0.5}, PICO_ANCHOR_NW, &r1 };
        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00});
        pico_output_draw_rect(&r2);

        _pico_check("rect-03");
    }

    {
        puts("rect at 50% anchored by bottom-right");
        pico_output_clear();

        Pico_Rel_Rect r1 = { '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, NULL };
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF});
        pico_output_draw_rect(&r1);

        puts("red anchored by top-left under -10% of white");
        Pico_Rel_Rect r2 = { '%', {-0.1, -0.1, 0.5, 0.5}, PICO_ANCHOR_NW, &r1 };
        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00});
        pico_output_draw_rect(&r2);

        _pico_check("rect-04");
    }

    {
        puts("rect at 50%");
        pico_output_clear();

        Pico_Rel_Rect r1 = { '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, NULL };
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF});
        pico_output_draw_rect(&r1);

        puts("red anchored by top-left under 110% of white (symmetric to previous)");
        Pico_Rel_Rect r2 = { '%', {1.1, 1.1, 0.5, 0.5}, PICO_ANCHOR_SE, &r1 };
        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00});
        pico_output_draw_rect(&r2);

        _pico_check("rect-05");
    }

    pico_init(0);
    return 0;
}
