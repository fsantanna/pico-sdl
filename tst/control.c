#include "pico.h"

int main (void) {
    pico_init(1);
    Pico_Pos pt = pico_pos(50, 50);
    while (1) {
        pico_output_draw_text(pt, "Hello!");
        pico_input_event(NULL, PICO_ANY);
        pico_output_clear();
    }
    pico_init(0);
    return 0;
}
