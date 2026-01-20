#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_title("Blend");
    pico_set_view_raw(
        -1, -1,
        &(Pico_Dim){640, 360},
        NULL,
        &(Pico_Dim){64, 36},
        NULL,
        NULL
    );

    {
        puts("pixel dimming");
        for (int a=255; a>0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_pixel_raw((Pico_Pos){32,18});
            pico_input_delay(10);
            if (a == 120) {
                _pico_check("blend_raw-01");
            }
        }
    }
    {
        puts("text dimming");
        int w = pico_get_text(10, "SOME TEXT");
        Pico_Rect r = {32-w/2, 18-5, 0, 10};
        for (int a=255; a>0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_text_raw("SOME TEXT", r);
            pico_input_delay(10);
            if (a == 120) {
                _pico_check("blend_raw-02");
            }
        }
    }
    {
        puts("rect dimming");
        Pico_Rect r = {32-30, 18-6, 60, 12};
        for (int a=255; a>0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_rect_raw(r);
            pico_input_delay(10);
            if (a == 120) {
                _pico_check("blend_raw-03");
            }
        }
    }
    {
        puts("oval dimming");
        Pico_Rect r = {32-30, 18-6, 60, 12};
        for (int a=255; a>0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_oval_raw(r);
            pico_input_delay(10);
            if (a == 120) {
                _pico_check("blend_raw-04");
            }
        }
    }
    {
        // 60, 12
        puts("tri dimming");
        Pico_Pos p1 = {  2, 12 };
        Pico_Pos p2 = {  2, 24 };
        Pico_Pos p3 = { 62, 24 };
        for (int a=255; a>0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_tri_raw(p1, p2, p3);
            pico_input_delay(10);
            if (a == 120) {
                _pico_check("blend_raw-05");
            }
        }
    }
    {
        puts("line dimming");
        Pico_Pos p1 = { 12, 12 };
        Pico_Pos p2 = { 52, 24 };
        for (int a=255; a>0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_line_raw(p1, p2);
            pico_input_delay(10);
            if (a == 120) {
                _pico_check("blend_raw-06");
            }
        }
    }
    {
        puts("poly dimming");
        Pico_Pos poly[] = {
            {  5,  5 },
            { 59, 10 },
            { 20, 31 },
        };

        for (int a=255; a>0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_poly_raw(3, poly);
            pico_input_delay(10);
            if (a == 120) {
                _pico_check("blend_raw-07");
            }
        }
    }

    pico_init(0);
    return 0;
}
