#include "pico.h"

int main (void) {
    pico_init(1);
    pico_set_view_raw(-1, &(Pico_Dim){160, 160}, NULL, &(Pico_Dim){16, 16}, NULL, NULL);
    pico_output_clear();
    pico_input_event(NULL, PICO_KEYUP);
    for (int i=0; i<16; i++) {
        pico_output_draw_pixel_raw((Pico_Pos) {i, i});
        pico_output_draw_pixel_raw((Pico_Pos) {15-i, i});
        pico_input_delay(100);
    }
    pico_input_delay(1000);
    pico_init(0);
    return 0;
}
