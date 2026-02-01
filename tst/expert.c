#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_window("Expert", -1, NULL);

    Pico_Rel_Rect r = { '%', {0.5,0.5, 0.5,0.5}, PICO_ANCHOR_C, NULL };
    pico_output_draw_rect(&r);
    _pico_check("expert-01");

    pico_set_expert(1);
    pico_output_clear();
    _pico_check("expert-02");

    pico_output_present();
    _pico_check("expert-03");

    int a = pico_get_ticks();
    pico_input_delay(100);
    int b = pico_get_ticks();
    assert(b>=a+100 && b<=a+101);

    pico_init(0);
    return 0;
}
