#include "pico.h"
#include "tst.c"

int main (void) {
    pico_init(1);
    pico_set_title("Blend");

    Pico_Pos pos = pico_pos((Pico_Pct){50, 50});
    Pico_Rect rct = {pos.x, pos.y, 60, 12};

    {
        puts("pixel dimming");
        for (int a = 255; a > 0; a-=5) {
            pico_output_clear();
            pico_set_color_draw((Pico_Color){255,0,0,a});
            pico_output_draw_pixel(pos);
            pico_input_delay(50);
            if (a == 120) {
                _pico_check("pixel_dimmed");
            }
        }
    }
    
    {
        puts("text dimming");
        for (int a = 255; a > 0; a-=5) {
            pico_output_clear();
            pico_set_color_draw((Pico_Color){255,0,0,a});
            pico_output_draw_text(pos, "SOME TEXT");
            pico_input_delay(50);
            if (a == 120) {
                _pico_check("text_dimmed");
            }
        }
    }

    {
        puts("rect dimming");
        for (int a = 255; a > 0; a-=5) {
            pico_output_clear();
            pico_set_color_draw((Pico_Color){255,0,0,a});
            pico_output_draw_rect(rct);
            pico_input_delay(50);
            if (a == 120) {
                _pico_check("rect_dimmed");
            }
        }
    }
    
    {
        puts("oval dimming");
        for (int a = 255; a > 0; a-=5) {
            pico_output_clear();
            pico_set_color_draw((Pico_Color){255,0,0,a});
            pico_output_draw_oval(rct);
            pico_input_delay(50);
            if (a == 120) {
                _pico_check("oval_dimmed");
            }
        }
    }

    {
        puts("tri dimming");
        for (int a = 255; a > 0; a-=5) {
            pico_output_clear();
            pico_set_color_draw((Pico_Color){255,0,0,a});
            pico_output_draw_tri(rct);
            pico_input_delay(50);
            if (a == 120) {
                _pico_check("tri_dimmed");
            }
        }
    }

    {
        puts("line dimming");
        Pico_Pos p1 = {pos.x-20, pos.y-6};
        Pico_Pos p2 = {pos.x+20, pos.y+6};
        for (int a = 255; a > 0; a-=5) {
            pico_output_clear();
            pico_set_color_draw((Pico_Color){255,0,0,a});
            pico_output_draw_line(p1, p2);
            pico_input_delay(50);
            if (a == 120) {
                _pico_check("line_dimmed");
            }
        }
    }

    {
        puts("poly dimming");
        Pico_Pos poly[] = {{5, 5}, {59, 10}, {20, 31}};
        for (int a = 255; a > 0; a-=5) {
            pico_output_clear();
            pico_set_color_draw((Pico_Color){255,0,0,a});
            pico_output_draw_poly(poly, 3);
            pico_input_delay(50);
            if (a == 120) {
                _pico_check("poly_dimmed");
            }
        }
    }

    pico_init(0);
    return 0;
}
