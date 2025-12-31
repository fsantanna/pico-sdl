#include "pico.h"

int main() {
    pico_init(1);

    Pico_Dim phy = {200,200};
    Pico_Dim log = { 20, 20};
    pico_set_view(-1, &phy, NULL, &log, NULL, NULL);

    Pico_Rect r = { 10-2, 10-2, 4, 4 };

    puts("pos_vs_rect");
    for (int y=r.y-1; y<=r.y+r.h; y++) {
        for (int x=r.x-1; x<=r.x+r.w; x++) {
            pico_output_clear();
            pico_set_color_draw((Pico_Color){255,255,255});
            pico_output_draw_rect_raw(r);

            Pico_Pos p = {x, y};
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_pixel_raw(p);

            int in = pico_pos_vs_rect_raw(p, r);
            puts(in ? "in" : "out");
            pico_input_event(NULL, PICO_KEYDOWN);
        }
    }

    puts("rect_vs_rect");
    for (int y=r.y-r.h; y<=r.y+r.h; y++) {
        for (int x=r.x-r.w; x<=r.x+r.w; x++) {
            pico_output_clear();
            pico_set_color_draw((Pico_Color){255,255,255});
            pico_output_draw_rect_raw(r);

            Pico_Rect r2 = {x,y,4,4};
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_rect_raw(r2);

            int in = pico_rect_vs_rect_raw(r2, r);
            puts(in ? "overlap" : "naw");
            pico_input_event(NULL, PICO_KEYDOWN);
        }
    }

    pico_init(0);
    return 0;
}
