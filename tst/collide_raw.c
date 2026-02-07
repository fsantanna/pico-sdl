#include "pico.h"
#include "../check.h"

int main() {
    pico_init(1);

    Pico_Rel_Dim phy = { '!', {200,200}, NULL };
    Pico_Rel_Dim log = { '!', { 20, 20}, NULL };
    pico_set_window(NULL, -1, &phy);
    pico_set_view(-1, &log, NULL, NULL, NULL, NULL, NULL, NULL);

    Pico_Rel_Rect r = { '!', { 10-2, 10-2, 4, 4 }, PICO_ANCHOR_NW, NULL };

    puts("pos_vs_rect");
    for (int y=r.y-1; y<=r.y+r.h; y++) {
        for (int x=r.x-1; x<=r.x+r.w; x++) {
            pico_output_clear();
            pico_set_color_draw((Pico_Color){255,255,255});
            pico_output_draw_rect(&r);

            Pico_Rel_Pos p = { '!', {x, y}, PICO_ANCHOR_NW, NULL };
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_pixel(&p);

            int in = pico_vs_pos_rect(&p, &r);
            //puts(in ? "in" : "out");
            pico_input_delay(10);

            if (x==7 && y==7) {
                assert(!in);
                _pico_check("collide_raw-01");
            }
            if (x==8 && y==8) {
                assert(in);
                _pico_check("collide_raw-02");
            }
            if (x==9 && y==9) {
                assert(in);
                _pico_check("collide_raw-03");
            }
            if (x==10 && y==10) {
                assert(in);
                _pico_check("collide_raw-04");
            }
            if (x==11 && y==11) {
                assert(in);
                _pico_check("collide_raw-05");
            }
            if (x==12 && y==12) {
                assert(!in);
                _pico_check("collide_raw-06");
            }
        }
    }

    puts("rect_vs_rect");
    for (int y=r.y-r.h; y<=r.y+r.h; y++) {
        for (int x=r.x-r.w; x<=r.x+r.w; x++) {
            pico_output_clear();
            pico_set_color_draw((Pico_Color){255,255,255});
            pico_output_draw_rect(&r);

            Pico_Rel_Rect r2 = { '!', {x,y,4,4}, PICO_ANCHOR_NW, NULL };
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_rect(&r2);

            int in = pico_vs_rect_rect(&r2, &r);
            //puts(in ? "overlap" : "naw");
            pico_input_delay(10);

            printf("%d/%d\n", x,y);
            if (x==4 && y==4) {
                assert(!in);
                _pico_check("collide_raw-07");
            }
            if (x==5 && y==5) {
                assert(in);
                _pico_check("collide_raw-08");
            }
            if (x==11 && y==11) {
                assert(in);
                _pico_check("collide_raw-09");
            }
            if (x==12 && y==12) {
                assert(!in);
                _pico_check("collide_raw-10");
            }
        }
    }

    pico_init(0);
    return 0;
}
