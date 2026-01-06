#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_title("Blend");

    {
        puts("pixel dimming");
        for (int a = 255; a > 0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_pixel_raw((Pico_Pos){50,50});
            pico_input_delay(50);
            if (a == 120) {
                _pico_check("blend_raw-01");
            }
        }
    }
    {
        puts("text dimming");
        int w = pico_get_text_width(10, "SOME TEXT");
        Pico_Rect r = {50-w/2, 50-5, 0, 10};
        for (int a = 255; a > 0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_text_raw(r, "SOME TEXT");
            pico_input_delay(50);
            if (a == 120) {
                _pico_check("blend_raw-02");
            }
        }
    }
    {
        puts("rect dimming");
        Pico_Rect r = {50-30, 50-6, 60, 12};
        for (int a = 255; a > 0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_rect_raw(r);
            pico_input_delay(50);
            if (a == 120) {
                _pico_check("blend_raw-03");
            }
        }
    }
    {
        puts("oval dimming");
        Pico_Rect r = {50-30, 50-6, 60, 12};
        for (int a = 255; a > 0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_oval_raw(r);
            pico_input_delay(50);
            if (a == 120) {
                _pico_check("blend_raw-04");
            }
        }
    }
    {
        puts("tri dimming");
        Pico_Pos p1 = { 50, 33 };
        Pico_Pos p2 = { 33, 66 };
        Pico_Pos p3 = { 66, 66 };
        for (int a = 255; a > 0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_tri_raw(p1, p2, p3);
            pico_input_delay(50);
            if (a == 120) {
                _pico_check("blend_raw-05");
            }
        }
    }
    {
        puts("line dimming");
        Pico_Pos p1 = {50-20, 50-6};
        Pico_Pos p2 = {50+20, 50+6};
        for (int a = 255; a > 0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_line_raw(p1, p2);
            pico_input_delay(50);
            if (a == 120) {
                _pico_check("blend_raw-06");
            }
        }
    }
    {
        puts("poly dimming");
        Pico_Pos poly[] = {
            { 50, 33 },
            { 33, 66 },
            { 66, 66 },
        };
        for (int a = 255; a > 0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_poly_raw(poly, 3);
            pico_input_delay(50);
            if (a == 120) {
                _pico_check("blend_raw-07");
            }
        }
    }

    pico_init(0);
    return 0;
}
