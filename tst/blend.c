#include "pico.h"
#include "tst.c"

int main (void) {
    pico_init(1);
    pico_set_title("Blend");

    {
        puts("image dimming");
        Pico_RectX r = { 0.5, 0.5, 0.5, 0, PICO_ANCHOR_C, NULL };
        for (int a = 255; a > 0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_output_draw_imageX(&r, "open.png");
            pico_input_delay(50);
            if (a == 120) {
                //_pico_check("image_dimmed");
            }
        }
    }
    {
        puts("pixel dimming");
        Pico_PosX p = { 0.5, 0.5, PICO_ANCHOR_NW, NULL };
        for (int a = 255; a > 0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_pixelX(&p);
            pico_input_delay(50);
            if (a == 120) {
                _pico_check("pixel_dimmed");
            }
        }
    }
#if 0
    {
        puts("text dimming");
        for (int a = 255; a > 0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_text(pos, "SOME TEXT");
            pico_input_delay(50);
            if (a == 120) {
                _pico_check("text_dimmed");
            }
        }
    }
#endif
    {
        puts("rect dimming");
        Pico_RectX r = { 0.5, 0.5, 0.9, 0.33, PICO_ANCHOR_C, NULL };
        for (int a = 255; a > 0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_rectX(&r);
            pico_input_delay(50);
            if (a == 120) {
                //_pico_check("rect_dimmed");
            }
        }
    }
    {
        puts("oval dimming");
        Pico_RectX r = { 0.5, 0.5, 0.9, 0.33, PICO_ANCHOR_C, NULL };
        for (int a = 255; a > 0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_ovalX(&r);
            pico_input_delay(50);
            if (a == 120) {
                //_pico_check("oval_dimmed");
            }
        }
    }
    {
        puts("tri dimming");
        Pico_PosX p1 = { 0.50, 0.33, PICO_ANCHOR_C, NULL };
        Pico_PosX p2 = { 0.33, 0.66, PICO_ANCHOR_C, NULL };
        Pico_PosX p3 = { 0.66, 0.66, PICO_ANCHOR_C, NULL };
        for (int a = 255; a > 0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_triX(&p1, &p2, &p3);
            pico_input_delay(50);
            if (a == 120) {
                //_pico_check("tri_dimmed");
            }
        }
    }
    {
        puts("line dimming");
        Pico_PosX p1 = { 0.33, 0.33, PICO_ANCHOR_C, NULL };
        Pico_PosX p2 = { 0.66, 0.66, PICO_ANCHOR_C, NULL };
        for (int a = 255; a > 0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_lineX(&p1, &p2);
            pico_input_delay(50);
            if (a == 120) {
                //_pico_check("line_dimmed");
            }
        }
    }
    {
        puts("poly dimming");
        Pico_PosX poly[] = {
            { 0.50, 0.33, PICO_ANCHOR_C, NULL },
            { 0.33, 0.66, PICO_ANCHOR_C, NULL },
            { 0.66, 0.66, PICO_ANCHOR_C, NULL },
        };
        for (int a = 255; a > 0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_polyX(poly, 3);
            pico_input_delay(50);
            if (a == 120) {
                //_pico_check("poly_dimmed");
            }
        }
    }

    pico_init(0);
    return 0;
}
