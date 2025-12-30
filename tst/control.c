#include "pico.h"

int main (void) {
    pico_init(1);
    Pico_Rect_Pct r = { 0.5,0.5, 0,0.2, PICO_ANCHOR_C, NULL };
    while (1) {
        pico_output_draw_text_pct(&r, "Hello!");
        pico_input_event(NULL, PICO_ANY);
        pico_output_clear();
    }
    pico_init(0);
    return 0;
}
