#include "pico.h"
#include "tst.c"

int main (void) {
    pico_init(1);
    pico_set_title("Pct-To-Pos");

#if 1
    {
        puts("white centered rect");
        Pico_RectX r = { 0.5, 0.5, 0.5, 0.5, PICO_ANCHOR_C, NULL };
        pico_output_clear();
        pico_output_draw_rectX(&r);
        _pico_check("pct_rect50");
    }
    {
        pico_output_clear();

        puts("white rect at 30%");
        Pico_RectX r1 = { 0.3, 0.3, 0.5, 0.5, PICO_ANCHOR_C, NULL };
        pico_output_draw_rectX(&r1);

        puts("red centered under white");
        Pico_RectX r2 = { 0.5, 0.5, 0.5, 0.5, PICO_ANCHOR_C, &r1 };
        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00});
        pico_output_draw_rectX(&r2);

        //_pico_check("pct_rect30_inner50");
    }
    {
        puts("rect at 50% anchored by bottom-right");
        pico_output_clear();

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
        puts("rect at -10/-10 top-left (4x7 rect on top)");
        pico_output_clear();
        Pico_RectX r = { -0.1, -0.1, 0.16, 0.3, PICO_ANCHOR_NW, NULL };
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF});
        pico_output_draw_rectX(&r);
        _pico_check("pct_rect-10");
    }
    {
        puts("rect at 110/110 bottom-right (symmetric to previous)");
        pico_output_clear();
        Pico_RectX r = { 1.1, 1.1, 0.16, 0.3, PICO_ANCHOR_SE, NULL };
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF});
        pico_output_draw_rectX(&r);
        _pico_check("pct_rect110");
    }
#else
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
        Pico_Pos  pt2  = pico_pos_ext((Pico_Pct){50, 50}, rct1, pico_get_anchor_pos());
        Pico_Rect rct2 = {pt2.x, pt2.y, 16, 9};
        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00});
        pico_output_draw_rect(rct2);

	_pico_check("pct_rect30_inner50");
    }
    {
        puts("rect at 50% anchored by bottom-right");
        pico_output_clear();

        Pico_Pos  pt1 = pico_pos((Pico_Pct){50, 50});
        Pico_Rect rct1 = {pt1.x, pt1.y, 32, 18};
        pico_set_anchor_pos((Pico_Anchor){PICO_RIGHT, PICO_BOTTOM});
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF});
        pico_output_draw_rect(rct1);

        puts("red anchored by top-left under 0% of white");
        Pico_Pos pt2 = pico_pos_ext((Pico_Pct){0, 0}, rct1, pico_get_anchor_pos());
        Pico_Rect rct2 = (Pico_Rect) {pt2.x, pt2.y, 16, 9};
        pico_set_anchor_pos((Pico_Anchor){PICO_LEFT, PICO_TOP});
        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00});
        pico_output_draw_rect(rct2);

	_pico_check("pct_rect50_inner0");
    }
    {
        puts("rect at -10/-10 top-left (4x7 rect on top)");
        pico_output_clear();

        Pico_Pos  pt = pico_pos((Pico_Pct){-10, -10});
        Pico_Rect rct = {pt.x, pt.y, 10, 10};
        pico_set_anchor_pos((Pico_Anchor){PICO_LEFT, PICO_TOP});
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF});
        pico_output_draw_rect(rct);

	_pico_check("pct_rect-10");
    }
    {
        puts("rect at 110/110 bottom-right (symmetric to previous)");
        pico_output_clear();

        Pico_Pos  pt = pico_pos((Pico_Pct){110, 110});
        Pico_Rect rct = {pt.x, pt.y, 10, 10};
        pico_set_anchor_pos((Pico_Anchor){PICO_RIGHT, PICO_BOTTOM});
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF});
        pico_output_draw_rect(rct);

	_pico_check("pct_rect110");
    }
#endif

    {
        puts("rect at 50% anchored by bottom-right");
        pico_output_clear();

        Pico_Pos  pt1 = pico_pos((Pico_Pct){50, 50});
        Pico_Rect rct1 = {pt1.x, pt1.y, 32, 18};
        pico_set_anchor_pos((Pico_Anchor){PICO_CENTER, PICO_MIDDLE});
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF});
        pico_output_draw_rect(rct1);

        puts("red anchored by top-left under -10% of white");
        Pico_Pos  pt2 = pico_pos_ext((Pico_Pct){-10, -10}, rct1, pico_get_anchor_pos());
        Pico_Rect rct2 = (Pico_Rect) {pt2.x, pt2.y, 16, 9};
        pico_set_anchor_pos((Pico_Anchor){PICO_LEFT, PICO_TOP});
        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00});
        pico_output_draw_rect(rct2);

	_pico_check("pct_rect50_inner-10");
    }

    {
        puts("rect at 50%");
        pico_output_clear();

        Pico_Pos  pt1 = pico_pos((Pico_Pct){50, 50});
        Pico_Rect rct1 = {pt1.x, pt1.y, 32, 18};
        pico_set_anchor_pos((Pico_Anchor){PICO_CENTER, PICO_MIDDLE});
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF});
        pico_output_draw_rect(rct1);

        puts("red anchored by top-left under 110% of white (symmetric to previous)");
        Pico_Pos  pt2 = pico_pos_ext((Pico_Pct){110, 110}, rct1, pico_get_anchor_pos());
        Pico_Rect rct2 = (Pico_Rect) {pt2.x, pt2.y, 16, 9};
        pico_set_anchor_pos((Pico_Anchor){PICO_RIGHT, PICO_BOTTOM});
        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00});
        pico_output_draw_rect(rct2);

	_pico_check("pct_rect50_inner110");
    }

    pico_init(0);
    return 0;
}
