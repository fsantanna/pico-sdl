#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_window_title("Hello World!");
    for (int i=0; i<100; i++) {
        pico_output_draw_pixel (
            (Pico_Rel_Pos) { '!', {i, i}, PICO_ANCHOR_C }
        );
        pico_output_draw_pixel (
            (Pico_Rel_Pos) { '!', {99-i, i}, PICO_ANCHOR_C }
        );
        pico_input_delay(5);
        switch (i) {
            case 0:
                _pico_check("cross-01");
                break;
            case 49:
                _pico_check("cross-02");
                break;
            case 99:
                _pico_check("cross-03");
                break;
        }
    }
    pico_init(0);
    return 0;
}
