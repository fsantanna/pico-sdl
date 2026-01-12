#include "pico.h"
#include "../check.h"

int main() {
    pico_init(1);

    Pico_Dim phy = {200,200};
    Pico_Dim log = { 20, 20};
    pico_set_view_raw(-1, &phy, NULL, &log, NULL, NULL);

    Pico_Rect_Pct r = { 0.5,0.5, 0.5,0.5, PICO_ANCHOR_C, NULL };

    puts("pos_vs_rect");
    for (float y=-0.1; y<=1.0; y+=0.1) {
        for (float x=-0.1; x<=1.0; x+=0.1) {
            pico_output_clear();
            pico_set_color_draw((Pico_Color){255,255,255});
            pico_output_draw_rect_pct(&r);

            Pico_Pos_Pct p = {x,y, PICO_ANCHOR_C, &r};
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_pixel_pct(&p);

            int in = pico_vs_pos_rect_pct(&p, &r);
            puts(in ? "in" : "out");

            if (x==-0.1 && y==-0.1) {
                assert(!in);
                _pico_check("collide_pct-01");
            }
            if (x==0.0 && y==0.0) {
                assert(in);
                _pico_check("collide_pct-02");
            }
            if (x==0.9 && y==0.9) {
                assert(in);
                _pico_check("collide_pct-03");
            }
            if (x==1.0 && y==1.0) {
                assert(!in);
                _pico_check("collide_pct-04");
            }

            pico_input_delay(10);
        }
    }

    puts("rect_vs_rect - same anchor");
    for (float y=-0.25; y<1.251; y+=0.1) {
        for (float x=-0.25; x<1.251; x+=0.1) {
            pico_output_clear();
            pico_set_color_draw((Pico_Color){255,255,255});
            pico_output_draw_rect_pct(&r);

            Pico_Rect_Pct r2 = { x,y, 0.5,0.5, PICO_ANCHOR_C, &r };
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_rect_pct(&r2);

            int in = pico_vs_rect_rect_pct(&r2, &r);
            puts(in ? "overlap" : "naw");

            if (x==-0.25 && y==-0.25) {
                assert(!in);
                _pico_check("collide_pct-05");
            }
            if (x==-0.15 && y==-0.15) {
                assert(in);
                _pico_check("collide_pct-06");
            }
            if (x==1.15 && y==1.15) {
                assert(in);
                _pico_check("collide_pct-07");
            }
            if (x==1.25 && y==1.25) {
                assert(!in);
                _pico_check("collide_pct-08");
            }

            pico_input_delay(10);
        }
    }

    puts("rect_vs_rect - diff anchor");
    for (float y=-0.5; y<1.01; y+=0.1) {
        for (float x=-0.5; x<1.01; x+=0.1) {
            pico_output_clear();
            pico_set_color_draw((Pico_Color){255,255,255});
            pico_output_draw_rect_pct(&r);

            Pico_Rect_Pct r2 = { x,y, 0.5,0.5, PICO_ANCHOR_NW, &r };
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_rect_pct(&r2);

            int in = pico_vs_rect_rect_pct(&r2, &r);
            puts(in ? "overlap" : "naw");

            if (x==-0.5 && y==-0.5) {
                assert(!in);
                _pico_check("collide_pct-09");
            }
            if (x==-0.4 && y==-0.4) {
                assert(in);
                _pico_check("collide_pct-10");
            }
            if (x==0.9 && y==0.9) {
                assert(in);
                _pico_check("collide_pct-11");
            }
            if (x==1.0 && y==1.0) {
                assert(!in);
                _pico_check("collide_pct-12");
            }

            pico_input_delay(10);
        }
    }

    pico_init(0);
    return 0;
}
