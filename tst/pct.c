#include "pico.h"
#include "tst.c"

int main (void) {
    pico_init(1);
    pico_set_title("Pct-To-Pos");

    {
        puts("centered rect");
        Pico_Pos  pt  = pico_pos((Pico_Pct){50, 50});
        Pico_Rect rct = {pt.x, pt.y, 32, 18};
        pico_output_clear();
        pico_output_draw_rect(rct);
	_pico_check("pct_rect50");
    }

    {
        puts("rect at 30%");
        pico_output_clear();

        Pico_Pos  pt1  = pico_pos((Pico_Pct){30, 30});
        Pico_Rect rct1 = {pt1.x, pt1.y, 32, 18};
        pico_output_draw_rect(rct1);

        puts("red centered under white");
        Pico_Pos  pt2  = pico_pos_ext(rct1, (Pico_Pct){50, 50});
        Pico_Rect rct2 = {pt2.x, pt2.y, 16, 9};
        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00,0xFF});
        pico_output_draw_rect(rct2);

	_pico_check("pct_rect30_inner50");
    }

    {
        puts("rect at 50% anchored by bottom-right");
        pico_output_clear();

        Pico_Pos  pt1 = pico_pos((Pico_Pct){50, 50});
        Pico_Rect rct1 = {pt1.x, pt1.y, 32, 18};
        pico_set_anchor_draw((Pico_Anchor){PICO_RIGHT, PICO_BOTTOM});
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF,0xFF});
        pico_output_draw_rect(rct1);

        puts("red anchored by top-left under 0% of white");
        Pico_Pos pt2 = pico_pos_ext(rct1, (Pico_Pct){0, 0});
        Pico_Rect rct2 = (Pico_Rect) {pt2.x, pt2.y, 16, 9};
        pico_set_anchor_draw((Pico_Anchor){PICO_LEFT, PICO_TOP});
        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00,0xFF});
        pico_output_draw_rect(rct2);

	_pico_check("pct_rect50_inner0");
    }

    {
        puts("rect at -10/-10 top-left (4x7 rect on top)");
        pico_output_clear();

        Pico_Pos  pt = pico_pos((Pico_Pct){-10, -10});
        Pico_Rect rct = {pt.x, pt.y, 10, 10};
        pico_set_anchor_draw((Pico_Anchor){PICO_LEFT, PICO_TOP});
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF,0xFF});
        pico_output_draw_rect(rct);

	_pico_check("pct_rect-10");
    }

    {
        puts("rect at 110/110 bottom-right (symmetric to previous)");
        pico_output_clear();

        Pico_Pos  pt = pico_pos((Pico_Pct){110, 110});
        Pico_Rect rct = {pt.x, pt.y, 10, 10};
        pico_set_anchor_draw((Pico_Anchor){PICO_RIGHT, PICO_BOTTOM});
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF,0xFF});
        pico_output_draw_rect(rct);

	_pico_check("pct_rect110");
    }

    {
        puts("rect at 50% anchored by bottom-right");
        pico_output_clear();

        Pico_Pos  pt1 = pico_pos((Pico_Pct){50, 50});
        Pico_Rect rct1 = {pt1.x, pt1.y, 32, 18};
        pico_set_anchor_draw((Pico_Anchor){PICO_CENTER, PICO_MIDDLE});
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF,0xFF});
        pico_output_draw_rect(rct1);

        puts("red anchored by top-left under -10% of white");
        Pico_Pos  pt2 = pico_pos_ext(rct1, (Pico_Pct){-10, -10});
        Pico_Rect rct2 = (Pico_Rect) {pt2.x, pt2.y, 16, 9};
        pico_set_anchor_draw((Pico_Anchor){PICO_LEFT, PICO_TOP});
        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00,0xFF});
        pico_output_draw_rect(rct2);

	_pico_check("pct_rect50_inner-10");
    }

    {
        puts("rect at 50%");
        pico_output_clear();

        Pico_Pos  pt1 = pico_pos((Pico_Pct){50, 50});
        Pico_Rect rct1 = {pt1.x, pt1.y, 32, 18};
        pico_set_anchor_draw((Pico_Anchor){PICO_CENTER, PICO_MIDDLE});
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF,0xFF});
        pico_output_draw_rect(rct1);

        puts("red anchored by top-left under 110% of white (symmetric to previous)");
        Pico_Pos  pt2 = pico_pos_ext(rct1, (Pico_Pct){110, 110});
        Pico_Rect rct2 = (Pico_Rect) {pt2.x, pt2.y, 16, 9};
        pico_set_anchor_draw((Pico_Anchor){PICO_RIGHT, PICO_BOTTOM});
        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00,0xFF});
        pico_output_draw_rect(rct2);

	_pico_check("pct_rect50_inner110");
    }

    pico_init(0);
    return 0;
}
