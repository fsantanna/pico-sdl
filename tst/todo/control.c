#include "pico.h"

int main (void) {
    pico_open(NULL);
    Pico_Rel_Rect r = { '%', {0.5,0.5, 0,0.2}, PICO_ANCHOR_C };
    while (1) {
        pico_output_draw_text_fix("Hello!", r);
        pico_input_event(NULL, PICO_EVENT_ANY);
        pico_output_clear();
    }
    pico_close();
    return 0;
}
