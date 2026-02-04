#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_window("Dim", -1, NULL);

    Pico_Rel_Rect r = { '%', {0.5,0.5, 0,0}, PICO_ANCHOR_C, NULL };
    char fmt[64];
    int N = 1;

    for (float i=0; i<=100; i+=25) {
        pico_output_clear();
        r.w = r.h = i/100.0;
        pico_output_draw_rect(&r);
        sprintf(fmt, "dim-%02d", N++);
        puts(fmt);
        _pico_check(fmt);
    }

    r.w = r.h = 0.8;
    for (float i=0; i<=125; i+=25) {
        pico_output_clear();

        pico_set_alpha(255);
        pico_set_color_draw((Pico_Color){255,255,255});
        pico_output_draw_rect(&r);

        Pico_Rel_Rect rr = { '%', {0.5,0.5, i/100.0,i/100.0}, PICO_ANCHOR_C, &r };
        pico_set_alpha(150);
        pico_set_color_draw((Pico_Color){255,0,0});
        pico_output_draw_rect(&rr);

        sprintf(fmt, "dim-%02d", N++);
        puts(fmt);
        _pico_check(fmt);
    }

    pico_init(0);
    return 0;
}
