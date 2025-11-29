#include "pico.h"
#include "tst.c"

int main (void) {
    pico_init(1);
    pico_set_title("Rect");

    {
        puts("centered rect");
        Pico_Rect r = pico_rect (
            (Pico_Pct) {50, 50},
            (Pico_Pct) {50, 50}
        );
        pico_output_clear();
        pico_output_draw_rect(r);
	_pico_check("pct_rect50");
    }

    {
        puts("rect at 30%");
        pico_output_clear();
        Pico_Rect r1 = pico_rect (
            (Pico_Pct) {30, 30},
            (Pico_Pct) {50, 50}
        );
        pico_output_draw_rect(r1);

        puts("red centered under white");
        Pico_Rect r2 = pico_rect_ext (
            (Pico_Pct){50, 50},
            (Pico_Pct){50, 50},
            r1, pico_get_anchor_pos()
        );
        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00,0xFF});
        pico_output_draw_rect(r2);

	_pico_check("pct_rect30_inner50");
    }

    {
        puts("rect at 50% anchored by bottom-right");
        pico_output_clear();

        Pico_Rect r1 = pico_rect (
            (Pico_Pct) {50, 50},
            (Pico_Pct) {50, 50}
        );
        pico_set_anchor_pos((Pico_Anchor){PICO_RIGHT, PICO_BOTTOM});
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF,0xFF});
        pico_output_draw_rect(r1);

        puts("red anchored by top-left under 0% of white");
        Pico_Rect r2 = pico_rect_ext (
            (Pico_Pct){0, 0},
            (Pico_Pct){50, 50},
            r1, pico_get_anchor_pos()
        );
        pico_set_anchor_pos((Pico_Anchor){PICO_LEFT, PICO_TOP});
        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00,0xFF});
        pico_output_draw_rect(r2);

	_pico_check("pct_rect50_inner0");
    }

    {
        puts("rect at 50% anchored by bottom-right");
        pico_output_clear();

        Pico_Rect r1 = pico_rect (
            (Pico_Pct) {50, 50},
            (Pico_Pct) {50, 50}
        );
        pico_set_anchor_pos((Pico_Anchor){PICO_CENTER, PICO_MIDDLE});
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF,0xFF});
        pico_output_draw_rect(r1);

        puts("red anchored by top-left under -10% of white");
        Pico_Rect r2 = pico_rect_ext (
            (Pico_Pct){-10, -10},
            (Pico_Pct){50, 50},
            r1, pico_get_anchor_pos()
        );
        pico_set_anchor_pos((Pico_Anchor){PICO_LEFT, PICO_TOP});
        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00,0xFF});
        pico_output_draw_rect(r2);

	_pico_check("pct_rect50_inner-10");
    }

    {
        puts("rect at 50%");
        pico_output_clear();

        Pico_Rect r1 = pico_rect (
            (Pico_Pct) {50, 50},
            (Pico_Pct) {50, 50}
        );
        pico_set_anchor_pos((Pico_Anchor){PICO_CENTER, PICO_MIDDLE});
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF,0xFF});
        pico_output_draw_rect(r1);

        puts("red anchored by top-left under 110% of white (symmetric to previous)");
        Pico_Rect r2 = pico_rect_ext (
            (Pico_Pct){110, 110},
            (Pico_Pct){50, 50},
            r1, pico_get_anchor_pos()
        );
        pico_set_anchor_pos((Pico_Anchor){PICO_RIGHT, PICO_BOTTOM});
        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00,0xFF});
        pico_output_draw_rect(r2);

	_pico_check("pct_rect50_inner110");
    }

    pico_init(0);
    return 0;
}
