#include "pico.h"
#include "tst.c"

int main (void) {
    pico_init(1);
    pico_set_title("Dim");

    Pico_Rect_Pct r = { 0.5,0.5, 0,0, PICO_ANCHOR_C, NULL };
    char fmt[64];

    for (float i=0; i<=1; i+=0.25) {
        pico_output_clear();
        r.w = r.h = i;
        pico_output_draw_rect_pct(&r);
        sprintf(fmt, "size: %.2f", i);
        puts(fmt);
        //_pico_check(fmt);
        pico_input_event(NULL, PICO_KEYDOWN);
    }

    r.w = r.h = 0.8;
    for (float i=0; i<=1.25; i+=0.25) {
        pico_output_clear();

        pico_set_alpha(255);
        pico_set_color_draw((Pico_Color){255,255,255});
        pico_output_draw_rect_pct(&r);

        Pico_Rect_Pct rr = { 0.5,0.5, i,i, PICO_ANCHOR_C, &r };
        pico_set_alpha(150);
        pico_set_color_draw((Pico_Color){255,0,0});
        pico_output_draw_rect_pct(&rr);

        sprintf(fmt, "size: %.2f", i);
        puts(fmt);
        //_pico_check(fmt);
        pico_input_event(NULL, PICO_KEYDOWN);
    }

    pico_init(0);
    return 0;
}
