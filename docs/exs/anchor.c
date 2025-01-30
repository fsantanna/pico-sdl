#include <pico.h>

int main() {
    pico_init(1);
    // ...
    SDL_Rect r = {10, 10, 20, 20};
    pico_set_anchor(PICO_TOP, PICO_LEFT);
    pico_output_draw_rect(r); // (r.x, r.y) is the top-left of r
    pico_set_anchor(PICO_CENTER, PICO_MIDDLE);
    pico_output_draw_rect(r); // (r.x, r.y) is the center of r
    pico_set_anchor(PICO_BOTTOM, PICO_RIGHT);
    pico_output_draw_rect(r); // (r.x, r.y) is the bottom-right of r
    // ...
    pico_init(0);
    return 0;
}