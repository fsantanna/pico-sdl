#include "../src/pico.h"

int main (void) {
    pico_init(1);
    Pico_Pos pt = pico_pct_to_pos(50, 50);
    pico_output_draw_text(pt, "Hello!");
    pico_input_delay(1000);
    pico_init(0);
    return 0;
}
