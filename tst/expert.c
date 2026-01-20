#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_title("Expert");

    Pico_Rect_Pct r = { 0.5,0.5, 0.5,0.5, PICO_ANCHOR_C, NULL };
    pico_output_draw_rect_pct(&r);
    _pico_check("expert-01");

    pico_set_expert(1);
    pico_output_clear();
    _pico_check("expert-02");

    pico_output_present();
    _pico_check("expert-03");

    pico_init(0);
    return 0;
}
