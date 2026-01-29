#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_view("Blend", -1, -1,
        &(Pico_Rel_Dim){'!', {640, 360}, NULL},
        NULL,
        &(Pico_Rel_Dim){'!', {64, 36}, NULL},
        NULL,
        NULL
    );

    {
        puts("pixel dimming");
        for (int a=255; a>0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_pixel(&(Pico_Rel_Pos){'!', {32,18}, PICO_ANCHOR_NW, NULL});
            pico_input_delay(10);
            if (a == 120) {
                _pico_check("blend_raw-01");
            }
        }
    }
    {
        puts("text dimming");
        Pico_Abs_Dim dim = pico_get_text("SOME TEXT", &(Pico_Rel_Dim){'!',{0,10},NULL});
        Pico_Rel_Rect r = {'!', {32-dim.w/2, 18-5, 0, 10}, PICO_ANCHOR_NW, NULL};
        for (int a=255; a>0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_text("SOME TEXT", &r);
            pico_input_delay(10);
            if (a == 120) {
                _pico_check("blend_raw-02");
            }
        }
    }
    {
        puts("rect dimming");
        Pico_Rel_Rect r = {'!', {32-30, 18-6, 60, 12}, PICO_ANCHOR_NW, NULL};
        for (int a=255; a>0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_rect(&r);
            pico_input_delay(10);
            if (a == 120) {
                _pico_check("blend_raw-03");
            }
        }
    }
    {
        puts("oval dimming");
        Pico_Rel_Rect r = {'!', {32-30, 18-6, 60, 12}, PICO_ANCHOR_NW, NULL};
        for (int a=255; a>0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_oval(&r);
            pico_input_delay(10);
            if (a == 120) {
                _pico_check("blend_raw-04");
            }
        }
    }
    {
        puts("tri dimming");
        Pico_Rel_Pos p1 = {'!', { 2, 12}, PICO_ANCHOR_NW, NULL};
        Pico_Rel_Pos p2 = {'!', { 2, 24}, PICO_ANCHOR_NW, NULL};
        Pico_Rel_Pos p3 = {'!', {62, 24}, PICO_ANCHOR_NW, NULL};
        for (int a=255; a>0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_tri(&p1, &p2, &p3);
            pico_input_delay(10);
            if (a == 120) {
                _pico_check("blend_raw-05");
            }
        }
    }
    {
        puts("line dimming");
        Pico_Rel_Pos p1 = {'!', {12, 12}, PICO_ANCHOR_NW, NULL};
        Pico_Rel_Pos p2 = {'!', {52, 24}, PICO_ANCHOR_NW, NULL};
        for (int a=255; a>0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_line(&p1, &p2);
            pico_input_delay(10);
            if (a == 120) {
                _pico_check("blend_raw-06");
            }
        }
    }
    {
        puts("poly dimming");
        Pico_Rel_Pos poly[] = {
            {'!', { 5,  5}, PICO_ANCHOR_NW, NULL},
            {'!', {59, 10}, PICO_ANCHOR_NW, NULL},
            {'!', {20, 31}, PICO_ANCHOR_NW, NULL},
        };

        for (int a=255; a>0; a-=5) {
            pico_output_clear();
            pico_set_alpha(a);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_poly(3, poly);
            pico_input_delay(10);
            if (a == 120) {
                _pico_check("blend_raw-07");
            }
        }
    }

    pico_init(0);
    return 0;
}
