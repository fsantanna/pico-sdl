#include "pico.h"
#include "../check.h"

int main (void) {
    pico_open(NULL);
    pico_set_window_title("Expert");

    Pico_Rel_Rect r = { '%', {0.5,0.5, 0.5,0.5}, PICO_ANCHOR_C };
    pico_output_draw_rect(r);
    _pico_check("expert-01");

    pico_set_expert(1, 0);
    pico_output_clear();
    _pico_check("expert-02");

    Pico_Layer* prev = pico_set_layer(pico_layer_window());
    pico_output_draw_layers();
    pico_set_layer(prev);

    pico_output_present(0);
    _pico_check("expert-03");

    // test: delay returns dt
    int dt = pico_input_delay(100);
    assert(dt>=100 && dt<=105);

    pico_close();
    return 0;
}
