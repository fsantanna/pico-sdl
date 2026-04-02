#include "pico.h"

int main (void) {
    pico_init(1);
    pico_output_clear();

    Pico_Rel_Rect r = { '%', {0.7, 0.3, 0.6, 0.4}, PICO_ANCHOR_C, NULL };
    Pico_Mouse m = pico_get_mouse_rect(&r);
    printf(">>> %f %f\n", m.x, m.y);

    pico_init(0);
    return 0;
}
