#include "pico.h"

int main (void) {
    pico_init(1);
    pico_set_title("Off-To-Pos");

    {
        puts("white at (1, 1) anchored by top-left");
        pico_output_clear();

        Pico_Rect rct1 = {1, 1, 32, 18};
        pico_set_anchor((Pico_Anchor){PICO_LEFT, PICO_TOP});
        pico_output_draw_rect(rct1);

        puts("red at (white.w - 1, middle) anchored by center-right under white");
        Pico_Pos  pt2;
        pt2.x = pico_off_to_pos(rct1, rct1.w - 1, 0).x;
        pt2.y = pico_pct_to_pos_ext(rct1, 0, 50).y;
        Pico_Rect rct2 = {pt2.x, pt2.y, 16, 9};
        pico_set_anchor((Pico_Anchor){PICO_RIGHT, PICO_MIDDLE});
        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00,0xFF});
        pico_output_draw_rect(rct2);

        pico_input_event(NULL, PICO_KEYDOWN);
    }

    {
        puts("white at (w-1,h-1) anchored by bottom-right");
        pico_output_clear();

        Pico_Dim sz = pico_get_size().log;
        Pico_Pos pt1 = {sz.x - 1, sz.y - 1};
        Pico_Rect rct1 = {pt1.x, pt1.y, 32, 18};
        pico_set_anchor((Pico_Anchor){PICO_RIGHT, PICO_BOTTOM});
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF,0xFF});
        pico_output_draw_rect(rct1);

        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00,0xFF});
        puts("red at (-1, 1) anchored by top-left under white");
        Pico_Pos pt2 = pico_off_to_pos(rct1, -1, 1);
        Pico_Rect rct2 = (Pico_Rect) {pt2.x, pt2.y, 5, 9};
        pico_set_anchor((Pico_Anchor){PICO_LEFT, PICO_TOP});
        pico_output_draw_rect(rct2);

        pico_set_anchor((Pico_Anchor){PICO_RIGHT, PICO_BOTTOM});
        puts("red2 at (white.w - 1, top + 1) anchored by top-right under white");
        Pico_Pos pt3 = pico_off_to_pos(rct1, rct1.w - 1, 1);
        Pico_Rect rct3 = (Pico_Rect) {pt3.x, pt3.y, 5, 9};
        pico_set_anchor((Pico_Anchor){PICO_RIGHT, PICO_TOP});
        pico_output_draw_rect(rct3);

        pico_input_event(NULL, PICO_KEYDOWN);
    }

    pico_init(0);
    return 0;
}
