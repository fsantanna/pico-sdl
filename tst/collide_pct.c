#include "pico.h"
#include "../check.h"

int main() {
    pico_init(1);

    Pico_Rel_Dim phy = { '!', {200,200}, NULL };
    Pico_Rel_Dim log = { '!', { 20, 20}, NULL };
    pico_set_view(NULL, -1, -1, &phy, NULL, &log, NULL, NULL, NULL);

    Pico_Rel_Rect r = { '%', {0.5,0.5, 0.5,0.5}, PICO_ANCHOR_C, NULL };

    puts("pos_vs_rect");
    for (float y=-1; y<=10; y+=1) {
        for (float x=-1; x<=10; x+=1) {
            pico_output_clear();
            pico_set_color_draw((Pico_Color){255,255,255});
            pico_output_draw_rect(&r);

            Pico_Rel_Pos p = { '%', {x/10.0,y/10.0}, PICO_ANCHOR_C, &r };
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_pixel(&p);

            int in = pico_vs_pos_rect(&p, &r);
            puts(in ? "in" : "out");

            if (x==-1 && y==-1) {
                assert(!in);
                _pico_check("collide_pct-01");
            }
            if (x==0 && y==0) {
                assert(in);
                _pico_check("collide_pct-02");
            }
            if (x==9 && y==9) {
                assert(in);
                _pico_check("collide_pct-03");
            }
            if (x==10 && y==10) {
                assert(!in);
                _pico_check("collide_pct-04");
            }

            pico_input_delay(10);
        }
    }

    puts("rect_vs_rect - same anchor");
    for (float y=-25; y<=125; y+=10) {
        for (float x=-25; x<=125; x+=10) {
            pico_output_clear();
            pico_set_color_draw((Pico_Color){255,255,255});
            pico_output_draw_rect(&r);

            Pico_Rel_Rect r2 = { '%', {x/100.0,y/100.0, 0.5,0.5}, PICO_ANCHOR_C, &r };
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_rect(&r2);

            int in = pico_vs_rect_rect(&r2, &r);
            puts(in ? "overlap" : "naw");

            if (x==-25 && y==-25) {
                assert(!in);
                _pico_check("collide_pct-05");
            }
            if (x==-15 && y==-15) {
                assert(in);
                _pico_check("collide_pct-06");
            }
            if (x==115 && y==115) {
                assert(in);
                _pico_check("collide_pct-07");
            }
            if (x==125 && y==125) {
                assert(!in);
                _pico_check("collide_pct-08");
            }

            pico_input_delay(10);
        }
    }

    puts("rect_vs_rect - diff anchor");
    for (float y=-5; y<=10; y+=1) {
        for (float x=-5; x<=10; x+=1) {
            pico_output_clear();
            pico_set_color_draw((Pico_Color){255,255,255});
            pico_output_draw_rect(&r);

            Pico_Rel_Rect r2 = { '%', {x/10.0,y/10.0, 0.5,0.5}, PICO_ANCHOR_NW, &r };
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_rect(&r2);

            int in = pico_vs_rect_rect(&r2, &r);
            puts(in ? "overlap" : "naw");

            if (x==-5 && y==-5) {
                assert(!in);
                _pico_check("collide_pct-09");
            }
            if (x==-4 && y==-4) {
                assert(in);
                _pico_check("collide_pct-10");
            }
            if (x==9 && y==9) {
                assert(in);
                _pico_check("collide_pct-11");
            }
            if (x==10 && y==10) {
                assert(!in);
                _pico_check("collide_pct-12");
            }

            pico_input_delay(10);
        }
    }

    pico_init(0);
    return 0;
}
