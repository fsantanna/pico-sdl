#include "pico.h"
#include "tst.c"

int main (void) {
    pico_init(1);
    pico_set_title("Pct-To-Pos");

    {
        puts("white centered rect");
        Pico_RectX r = { 0.5, 0.5, 0.5, 0.5, PICO_ANCHOR_C, NULL };
        pico_output_clear();
        pico_output_draw_rectX(&r);
        _pico_check("pct_rect50");
    }
    {
        pico_output_clear();

        // TODO: 0.28
        puts("white rect at 30%");
        Pico_RectX r1 = { 0.3, 0.28, 0.5, 0.5, PICO_ANCHOR_C, NULL };
        pico_output_draw_rectX(&r1);

        puts("red centered under white");
        Pico_RectX r2 = { 0.5, 0.5, 0.5, 0.5, PICO_ANCHOR_C, &r1 };
        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00});
        pico_output_draw_rectX(&r2);

        _pico_check("pct_rect30_inner50");
    }
    {
        pico_output_clear();

        puts("rect at 50% anchored by bottom-right");
        Pico_RectX r1 = { 0.5, 0.5, 0.5, 0.5, PICO_ANCHOR_SE, NULL };
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF});
        pico_output_draw_rectX(&r1);

        puts("red anchored by top-left under 0% of white");
        Pico_RectX r2 = { 0, 0, 0.5, 0.5, PICO_ANCHOR_NW, &r1 };
        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00});
        pico_output_draw_rectX(&r2);

	    _pico_check("pct_rect50_inner0");
    }
    {
        // TODO: 0.16
        puts("rect at -10/-10 top-left (4x7 rect on top)");
        pico_output_clear();
        Pico_RectX r = { -0.1, -0.1, 0.16, 0.3, PICO_ANCHOR_NW, NULL };
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF});
        pico_output_draw_rectX(&r);
        _pico_check("pct_rect-10");
    }
    {
        // TODO: 0.16
        puts("rect at 110/110 bottom-right (symmetric to previous)");
        pico_output_clear();
        Pico_RectX r = { 1.1, 1.1, 0.16, 0.3, PICO_ANCHOR_SE, NULL };
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF});
        pico_output_draw_rectX(&r);
        _pico_check("pct_rect110");
    }
    {
        pico_output_clear();

        puts("rect at 50% anchored by bottom-right");
        Pico_RectX r1 = { 0.5, 0.5, 0.5, 0.5, PICO_ANCHOR_C, NULL };
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF});
        pico_output_draw_rectX(&r1);

        // TODO: 0.08
        puts("red anchored by top-left under -10% of white");
        Pico_RectX r2 = { -0.1, -0.08, 0.5, 0.5, PICO_ANCHOR_NW, &r1 };
        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00});
        pico_output_draw_rectX(&r2);

        _pico_check("pct_rect50_inner-10");
    }
    {
        pico_output_clear();

        puts("rect at 50%");
        Pico_RectX r1 = { 0.5, 0.5, 0.5, 0.5, PICO_ANCHOR_C, NULL };
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF});
        pico_output_draw_rectX(&r1);

        // TODO: 1.08
        puts("red anchored by top-left under 110% of white (symmetric to previous)");
        Pico_RectX r2 = { 1.1, 1.08, 0.5, 0.5, PICO_ANCHOR_SE, &r1 };
        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00});
        pico_output_draw_rectX(&r2);

        _pico_check("pct_rect50_inner110");
    }

    pico_init(0);
    return 0;
}
