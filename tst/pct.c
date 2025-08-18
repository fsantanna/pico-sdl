#include "pico.h"

int main (void) {
    pico_init(1);
    pico_set_title("Pct-To-Pos");

    {
        puts("centered rect");
        Pico_Pos  pt  = pico_pos(50, 50);
        Pico_Rect rct = {pt.x, pt.y, 32, 18};
        pico_output_clear();
        pico_output_draw_rect(rct);
        pico_input_event(NULL, PICO_KEYDOWN);
    }

    {
        puts("rect at 30%");
        pico_output_clear();

        Pico_Pos  pt1  = pico_pos(30, 30);
        Pico_Rect rct1 = {pt1.x, pt1.y, 32, 18};
        pico_output_draw_rect(rct1);

        puts("red centered under white");
        Pico_Pos  pt2  = pico_pos_ext(rct1, 50, 50);
        Pico_Rect rct2 = {pt2.x, pt2.y, 16, 9};
        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00,0xFF});
        pico_output_draw_rect(rct2);

        pico_input_event(NULL, PICO_KEYDOWN);
    }

    {
        puts("rect at 50% anchored by bottom-right");
        pico_output_clear();

        Pico_Pos pt1 = pico_pos(50, 50);
        Pico_Rect rct1 = {pt1.x, pt1.y, 32, 18};
        pico_set_anchor((Pico_Anchor){PICO_RIGHT, PICO_BOTTOM});
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF,0xFF});
        pico_output_draw_rect(rct1);

        puts("red anchored by top-left under 0% of white");
        Pico_Pos pt2 = pico_pos_ext(rct1, 0, 0);
        Pico_Rect rct2 = (Pico_Rect) {pt2.x, pt2.y, 16, 9};
        pico_set_anchor((Pico_Anchor){PICO_LEFT, PICO_TOP});
        pico_set_color_draw((Pico_Color){255,0,0,255});
        pico_output_draw_rect(rct2);

        pico_input_event(NULL, PICO_KEYDOWN);
    }

    {
        puts("rect at -10/-10 (top-left)");
        pico_output_clear();

        Pico_Pos pt = pico_pos(-10, -10);
        Pico_Rect rct = {pt.x, pt.y, 10, 10};
        pico_set_anchor((Pico_Anchor){PICO_LEFT, PICO_TOP});
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF,0xFF});
        pico_output_draw_rect(rct);

        pico_input_event(NULL, PICO_KEYDOWN);
    }

    {
        puts("rect at 110/110 (bottom-right; symmetric to previous)");
        pico_output_clear();

        Pico_Pos pt = pico_pos(110, 110);
        Pico_Rect rct = {pt.x, pt.y, 10, 10};
        pico_set_anchor((Pico_Anchor){PICO_RIGHT, PICO_BOTTOM});
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF,0xFF});
        pico_output_draw_rect(rct);

        pico_input_event(NULL, PICO_KEYDOWN);
    }

    {
        puts("rect at 50% anchored by bottom-right");
        pico_output_clear();

        Pico_Pos pt1 = pico_pos(50, 50);
        Pico_Rect rct1 = {pt1.x, pt1.y, 32, 18};
        pico_set_anchor((Pico_Anchor){PICO_CENTER, PICO_MIDDLE});
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF,0xFF});
        pico_output_draw_rect(rct1);

        puts("red anchored by top-left under -10% of white");
        Pico_Pos pt2 = pico_pos_ext(rct1, -10, -10);
        Pico_Rect rct2 = (Pico_Rect) {pt2.x, pt2.y, 16, 9};
        pico_set_anchor((Pico_Anchor){PICO_LEFT, PICO_TOP});
        pico_set_color_draw((Pico_Color){255,0,0,255});
        pico_output_draw_rect(rct2);

        pico_input_event(NULL, PICO_KEYDOWN);
    }

    {
        puts("rect at 50%");
        pico_output_clear();

        Pico_Pos pt1 = pico_pos(50, 50);
        Pico_Rect rct1 = {pt1.x, pt1.y, 32, 18};
        pico_set_anchor((Pico_Anchor){PICO_CENTER, PICO_MIDDLE});
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF,0xFF});
        pico_output_draw_rect(rct1);

        puts("red anchored by top-left under 110% of white (symmetric to previous)");
        Pico_Pos pt2 = pico_pos_ext(rct1, 110, 110);
        Pico_Rect rct2 = (Pico_Rect) {pt2.x, pt2.y, 16, 9};
        pico_set_anchor((Pico_Anchor){PICO_RIGHT, PICO_BOTTOM});
        pico_set_color_draw((Pico_Color){255,0,0,255});
        pico_output_draw_rect(rct2);

        pico_input_event(NULL, PICO_KEYDOWN);
    }

    pico_init(0);
    return 0;
}
