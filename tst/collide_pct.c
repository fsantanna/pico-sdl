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

    puts("rect_vs_rect - same anchor");
    for (float y=-0.25; y<1.26; y+=0.1) {
        for (float x=-0.25; x<1.26; x+=0.1) {
            pico_output_clear();
            pico_set_color_draw((Pico_Color){255,255,255});
            pico_output_draw_rect_pct(&r);

            Pico_Rect_Pct r2 = { x,y, 0.5,0.5, PICO_ANCHOR_C, &r };
            pico_set_color_draw((Pico_Color){255,0,0});
            pico_output_draw_rect_pct(&r2);

            int in = pico_rect_vs_rect_pct(&r2, &r);
            puts(in ? "overlap" : "naw");
            pico_input_event(NULL, PICO_KEYDOWN);
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

            int in = pico_rect_vs_rect_pct(&r2, &r);
            puts(in ? "overlap" : "naw");
            pico_input_event(NULL, PICO_KEYDOWN);
        }
    }

    pico_init(0);
    return 0;
}
