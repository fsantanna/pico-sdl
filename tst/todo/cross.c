#include "pico.h"

int main (void) {
    pico_init(1);
    for (int i=0; i<100; i++) {
        pico_output_draw_pixel (
            (Pico_Rel_Pos) { '!', {i, i}, PICO_ANCHOR_C }
        );
        pico_output_draw_pixel (
            (Pico_Rel_Pos) { '!', {99-i, i}, PICO_ANCHOR_C }
        );
        pico_input_delay(20);
    }
    pico_input_event(NULL, PICO_EVENT_QUIT);
    pico_init(0);
    return 0;
}
