#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_title("Blend");
    pico_set_view_raw(
        -1,
        &(Pico_Dim){640, 360},
        NULL,
        &(Pico_Dim){64, 36},
        NULL,
        NULL
    );

    {
        puts("image dimming");
        Pico_Rect_Pct r = { 0.5, 0.5, 0.5, 0, PICO_ANCHOR_C, NULL };
        for (int a=255; a>0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_output_draw_image_pct(&r, "open.png");
            pico_input_delay(10);
            if (a == 120) {
                _pico_check("blend_pct-01");
            }
        }
    }
    {
        puts("pixel dimming");
        Pico_Pos_Pct p = { 0.5, 0.5, PICO_ANCHOR_NW, NULL };
        for (int a=255; a>0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_pixel_pct(&p);
            pico_input_delay(10);
            if (a == 120) {
                _pico_check("blend_pct-02");
            }
        }
    }
    {
        puts("text dimming");
        Pico_Rect_Pct r = { 0.5, 0.5, 0, 0.28, PICO_ANCHOR_C, NULL };
        for (int a=255; a>0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_text_pct(&r, "SOME TEXT");
            pico_input_delay(10);
            if (a == 120) {
                _pico_check("blend_pct-03");
            }
        }
    }
    {
        puts("rect dimming");
        Pico_Rect_Pct r = { 0.5, 0.5, 0.95, 0.34, PICO_ANCHOR_C, NULL };
        for (int a=255; a>0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_rect_pct(&r);
            pico_input_delay(10);
            if (a == 120) {
                _pico_check("blend_pct-04");
            }
        }
    }
    {
        puts("oval dimming");
        Pico_Rect_Pct r = { 0.5, 0.5, 0.95, 0.34, PICO_ANCHOR_C, NULL };
        for (int a=255; a>0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_oval_pct(&r);
            pico_input_delay(10);
            if (a == 120) {
                _pico_check("blend_pct-05");
            }
        }
    }
    {
        puts("tri dimming");
        Pico_Pos_Pct p1 = { 0.50, 0.33, PICO_ANCHOR_C, NULL };
        Pico_Pos_Pct p2 = { 0.33, 0.66, PICO_ANCHOR_C, NULL };
        Pico_Pos_Pct p3 = { 0.66, 0.66, PICO_ANCHOR_C, NULL };
        for (int a=255; a>0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_tri_pct(&p1, &p2, &p3);
            pico_input_delay(10);
            if (a == 120) {
                _pico_check("blend_pct-06");
            }
        }
    }
    {
        puts("line dimming");
        Pico_Pos_Pct p1 = { 0.33, 0.33, PICO_ANCHOR_C, NULL };
        Pico_Pos_Pct p2 = { 0.66, 0.66, PICO_ANCHOR_C, NULL };
        for (int a=255; a>0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_line_pct(&p1, &p2);
            pico_input_delay(10);
            if (a == 120) {
                _pico_check("blend_pct-07");
            }
        }
    }
    {
        puts("poly dimming");
        Pico_Pos_Pct poly[] = {
            { 0.50, 0.33, PICO_ANCHOR_C, NULL },
            { 0.33, 0.66, PICO_ANCHOR_C, NULL },
            { 0.66, 0.66, PICO_ANCHOR_C, NULL },
        };
        for (int a=255; a>0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_poly_pct(poly, 3);
            pico_input_delay(10);
            if (a == 120) {
                _pico_check("blend_pct-08");
            }
        }
    }

    pico_init(0);
    return 0;
}
