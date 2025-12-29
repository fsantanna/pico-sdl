#include "pico.h"

int main() {
    pico_init(1);
    pico_set_dim_window((Pico_Dim){200,200});
    pico_set_dim_world((Pico_Dim){20,20});

    Pico_Rect_Pct r = { 0.5,0.5, 0.5,0.5, PICO_ANCHOR_C, NULL };

    puts("pos_vs_rect");
    for (float y=0; y<1.2; y+=0.1) {
        for (float x=0; x<1.2; x+=0.1) {
            pico_output_clear();
            pico_set_color_draw((Pico_Color){255,255,255});
            pico_output_draw_rect_pct(&r);

            Pico_Pos_Pct p = {x,y, PICO_ANCHOR_C, &r};
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_pixel_pct(&p);

            int in = pico_pos_vs_rect_pct(&p, &r);
            puts(in ? "in" : "out");
            pico_input_event(NULL, PICO_KEYDOWN);
        }
    }

#if 0
    puts("pos_vs_rect_ext - px bottom-right, rct top-left");
    for (int y = r.y; y < r.y+r.h+2; y++) {
        for (int x = r.x; x < r.x+r.w+2; x++) {
            pico_output_clear();
            pico_set_color_draw((Pico_Color){255,255,255});
            pico_set_anchor_pos((Pico_Anchor){PICO_LEFT,PICO_TOP});
            pico_output_draw_rect(r);

            Pico_Pos pt = {x, y};
            int in = pico_pos_vs_rect_ext(pt, r,
                        (Pico_Anchor) {PICO_RIGHT,PICO_BOTTOM},
                        (Pico_Anchor) {PICO_LEFT,PICO_TOP}
            );
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_set_anchor_pos((Pico_Anchor){PICO_RIGHT,PICO_BOTTOM});
            pico_output_draw_pixel(pt);

            puts(in ? "in  " : "out ");

            pico_input_event(NULL, PICO_KEYDOWN);
        }
    }

    puts("rect_vs_rect - same anchor");
    pico_set_anchor_pos((Pico_Anchor){PICO_LEFT,PICO_TOP});
    for (int y = r.y-r.h; y < r.y+r.h+1; y++) {
        for (int x = r.x-r.w; x < r.x+r.w+1; x++) {
            pico_output_clear();
            pico_set_color_draw((Pico_Color){255,255,255});
            pico_output_draw_rect(r);

            Pico_Rect r2 = {x,y,4,4};
            int in = pico_rect_vs_rect(r2, r);
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_rect(r2);

            puts(in ? "overlap" : "naw");

            pico_input_event(NULL, PICO_KEYDOWN);
        }
    }

    puts("rect_vs_rect_ext - bottom-right, top-left");
    for (int y = r.y; y < r.y+2*r.h+1; y++) {
        for (int x = r.x; x < r.x+2*r.w+1; x++) {
            pico_output_clear();
            pico_set_color_draw((Pico_Color){255,255,255});
            pico_set_anchor_pos((Pico_Anchor){PICO_LEFT,PICO_TOP});
            pico_output_draw_rect(r);

            Pico_Rect r2 = {x,y,4,4};
            int in = pico_rect_vs_rect_ext(r2, r,
                (Pico_Anchor){PICO_RIGHT,PICO_BOTTOM},
                (Pico_Anchor){PICO_LEFT,PICO_TOP}
            );
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_set_anchor_pos((Pico_Anchor){PICO_RIGHT,PICO_BOTTOM});
            pico_output_draw_rect(r2);

            puts(in ? "overlap" : "naw");

            pico_input_event(NULL, PICO_KEYDOWN);
        }
    }
#endif

    pico_init(0);
    return 0;
}
