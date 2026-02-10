#include "pico.h"
#include <stdio.h>

int main (void) {
    pico_init(1);
    for (int i=0; i<100; i++) {
        pico_output_draw_pixel (
            &(Pico_Rel_Pos) { '!', {i, i}, PICO_ANCHOR_C, NULL }
        );
        pico_output_draw_pixel (
            &(Pico_Rel_Pos) { '!', {99-i, i}, PICO_ANCHOR_C, NULL }
        );
        char path[64];
        sprintf(path, "out/cross-%03d.png", i);
        pico_output_screenshot(path, NULL);
    }
    pico_init(0);
    return 0;
}
