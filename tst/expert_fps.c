#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_window("Expert FPS", -1, NULL);

    // test 1: set.expert returns ms
    int ms = pico_set_expert(1, 40);
    assert(ms == 25);

    // test 2: fps=0 returns 0
    int ms0 = pico_set_expert(1, 0);
    assert(ms0 == 0);

    // test 3: frame timing with compensation
    pico_set_expert(1, 40);
    int t0 = pico_get_ticks();
    Pico_Event e;
    for (int i=0; i<4; i++) {
        pico_input_event(&e, PICO_EVENT_ANY);
    }
    int elapsed = pico_get_ticks() - t0;
    assert(elapsed>=100 && elapsed<=110);

    // test 4: getter returns fps via pointer
    int fps;
    pico_set_expert(1, 40);
    assert(pico_get_expert(&fps) == 1);
    assert(fps == 40);

    // test 5: disable clears fps
    pico_set_expert(0, 0);
    assert(pico_get_expert(&fps) == 0);
    assert(fps == 0);

    // test 6: getter with NULL fps
    pico_set_expert(1, 60);
    assert(pico_get_expert(NULL) == 1);

    pico_init(0);
    return 0;
}
