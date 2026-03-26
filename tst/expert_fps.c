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
    int t0 = pico_get_now();
    Pico_Event e;
    for (int i=0; i<4; i++) {
        pico_input_event(&e, PICO_EVENT_ANY);
    }
    int elapsed = pico_get_now() - t0;
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

    // test 7: fps=-1 returns ms=0
    int ms1 = pico_set_expert(1, -1);
    assert(ms1 == 0);

    // test 8: getter returns fps=-1
    pico_set_expert(1, -1);
    assert(pico_get_expert(&fps) == 1);
    assert(fps == -1);

    // test 9: fps=-1 returns immediately (dt ~0)
    pico_set_expert(1, -1);
    Pico_Event e2;
    int dt = pico_input_event(&e2, PICO_EVENT_ANY);
    assert(dt <= 5);
    assert(e2.type == PICO_EVENT_NONE);

    // test 10: event returns dt with fps timing
    pico_set_expert(1, 40);
    Pico_Event e3;
    int dt2 = pico_input_event(&e3, PICO_EVENT_ANY);
    assert(dt2>=25 && dt2<=30);

    pico_init(0);
    return 0;
}
