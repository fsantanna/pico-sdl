#include "pico.h"
#include "../check.h"

int main() {
    pico_init(1);

    Pico_Dim phy = {200,200};
    Pico_Dim log = { 20, 20};
    pico_set_view_abs(-1, -1, &phy, NULL, &log, NULL, NULL);

    Pico_Rect r = { 10-2, 10-2, 4, 4 };

    puts("pos_vs_rect");
    for (int y=r.y-1; y<=r.y+r.h; y++) {
        for (int x=r.x-1; x<=r.x+r.w; x++) {
            pico_output_clear();
            pico_set_color_draw((Pico_Color){255,255,255});
            pico_output_draw_rect_abs(r);

            Pico_Pos p = {x, y};
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_pixel_abs(p);

            int in = pico_vs_pos_rect_abs(p, r);
            puts(in ? "in" : "out");
            pico_input_delay(10);

            if (x==7 && y==7) {
                assert(!in);
                _pico_check("collide_abs-01");
            }
            if (x==8 && y==8) {
                assert(in);
                _pico_check("collide_abs-02");
            }
            if (x==9 && y==9) {
                assert(in);
                _pico_check("collide_abs-03");
            }
            if (x==10 && y==10) {
                assert(in);
                _pico_check("collide_abs-04");
            }
            if (x==11 && y==11) {
                assert(in);
                _pico_check("collide_abs-05");
            }
            if (x==12 && y==12) {
                assert(!in);
                _pico_check("collide_abs-06");
            }
        }
    }

    puts("rect_vs_rect");
    for (int y=r.y-r.h; y<=r.y+r.h; y++) {
        for (int x=r.x-r.w; x<=r.x+r.w; x++) {
            pico_output_clear();
            pico_set_color_draw((Pico_Color){255,255,255});
            pico_output_draw_rect_abs(r);

            Pico_Rect r2 = {x,y,4,4};
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_rect_abs(r2);

            int in = pico_vs_rect_rect_abs(r2, r);
            puts(in ? "overlap" : "naw");
            pico_input_delay(10);

            printf("%d/%d\n", x,y);
            if (x==4 && y==4) {
                assert(!in);
                _pico_check("collide_abs-07");
            }
            if (x==5 && y==5) {
                assert(in);
                _pico_check("collide_abs-08");
            }
            if (x==11 && y==11) {
                assert(in);
                _pico_check("collide_abs-09");
            }
            if (x==12 && y==12) {
                assert(!in);
                _pico_check("collide_abs-10");
            }
        }
    }

    pico_init(0);
    return 0;
}
