#include "pico.h"

int main() {
    pico_init(1);
    pico_set_size((Pico_Dim){200,200},(Pico_Dim){20,20});

    Pico_Pos pt = pico_pos((Pico_Pct){50, 50});
    Pico_Rect r = {pt.x,pt.y,4,4};

    puts("pos_vs_rect - same anchor");
    pico_set_anchor_draw((Pico_Anchor){PICO_RIGHT,PICO_BOTTOM});
    for (int y = r.y-r.w; y < r.y+2; y++) {
        for (int x = r.x-r.w; x < r.x+2; x++) {
            pico_output_clear();
            pico_set_color_draw((Pico_Color){255,255,255,255});
            pico_output_draw_rect(r);

            Pico_Pos pt = {x, y};
            int in = pico_pos_vs_rect(pt, r);
            pico_set_color_draw((Pico_Color){255,0,0,255});
            pico_output_draw_pixel(pt);

            puts(in ? "in" : "out");

            pico_input_event(NULL, PICO_KEYDOWN);
        }
    }

    puts("pos_vs_rect_ext - px bottom-right, rct top-left");
    for (int y = r.y; y < r.y+r.h+2; y++) {
        for (int x = r.x; x < r.x+r.w+2; x++) {
            pico_output_clear();
            pico_set_color_draw((Pico_Color){255,255,255,255});
            pico_set_anchor_draw((Pico_Anchor){PICO_LEFT,PICO_TOP});
            pico_output_draw_rect(r);

            Pico_Pos pt = {x, y};
            int in = pico_pos_vs_rect_ext(pt, (Pico_Anchor){PICO_RIGHT,PICO_BOTTOM},
                    r, (Pico_Anchor){PICO_LEFT,PICO_TOP});
            pico_set_color_draw((Pico_Color){255,0,0,255});
            pico_set_anchor_draw((Pico_Anchor){PICO_RIGHT,PICO_BOTTOM});
            pico_output_draw_pixel(pt);

            puts(in ? "in  " : "out ");

            pico_input_event(NULL, PICO_KEYDOWN);
        }
    }

    puts("rect_vs_rect - same anchor");
    pico_set_anchor_draw((Pico_Anchor){PICO_LEFT,PICO_TOP});
    for (int y = r.y-r.h; y < r.y+r.h+1; y++) {
        for (int x = r.x-r.w; x < r.x+r.w+1; x++) {
            pico_output_clear();
            pico_set_color_draw((Pico_Color){255,255,255,255});
            pico_output_draw_rect(r);

            Pico_Rect r2 = {x,y,4,4};
            int in = pico_rect_vs_rect(r2, r);
            pico_set_color_draw((Pico_Color){255,0,0,255});
            pico_output_draw_rect(r2);

            puts(in ? "overlap" : "naw");

            pico_input_event(NULL, PICO_KEYDOWN);
        }
    }

    puts("rect_vs_rect_ext - bottom-right, top-left");
    for (int y = r.y; y < r.y+2*r.h+1; y++) {
        for (int x = r.x; x < r.x+2*r.w+1; x++) {
            pico_output_clear();
            pico_set_color_draw((Pico_Color){255,255,255,255});
            pico_set_anchor_draw((Pico_Anchor){PICO_LEFT,PICO_TOP});
            pico_output_draw_rect(r);

            Pico_Rect r2 = {x,y,4,4};
            int in = pico_rect_vs_rect_ext(r2, (Pico_Anchor){PICO_RIGHT,PICO_BOTTOM},
                                            r, (Pico_Anchor){PICO_LEFT,PICO_TOP});
            pico_set_color_draw((Pico_Color){255,0,0,255});
            pico_set_anchor_draw((Pico_Anchor){PICO_RIGHT,PICO_BOTTOM});
            pico_output_draw_rect(r2);

            puts(in ? "overlap" : "naw");

            pico_input_event(NULL, PICO_KEYDOWN);
        }
    }

    puts("assert error");
    pico_set_rotate(10);
    pico_pos_vs_rect(pt, r);

    pico_init(0);
    return 0;
}
