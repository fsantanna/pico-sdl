#include "pico.h"

int main (void) {
    pico_init(1);

    {
        Pico_Rect r = { 50,50, 0,10 };
        pico_output_draw_text_raw(r, "Hello!");
        pico_input_delay(1000);
    }
    {
        Pico_Rect_Pct r = { 0.5,0.5, 0,0.2, PICO_ANCHOR_C, NULL };
        pico_output_draw_text_pct(&r, "Hello!");
        pico_input_delay(1000);
    }

    pico_init(0);
    return 0;
}
