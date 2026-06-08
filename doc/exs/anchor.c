// file: anchor.c
#include <pico.h>

int main() {
    pico_init(1);
    // ...
    // The anchor is now part of each Pico_Rel_Rect, set per draw call.
    Pico_Rel_Rect r = { '!', {10, 10, 20, 20}, PICO_ANCHOR_NW };
    pico_output_draw_rect(r); // (x, y) is the top-left of r
    r.anchor = PICO_ANCHOR_C;
    pico_output_draw_rect(r); // (x, y) is the center of r
    r.anchor = PICO_ANCHOR_SE;
    pico_output_draw_rect(r); // (x, y) is the bottom-right of r
    // ...
    pico_init(0);
    return 0;
}
