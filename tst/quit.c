#include "pico.h"

int main (void) {
    pico_init(1);
    pico_set_expert(1);

    pico_quit();

    Pico_Event e;
    int has = pico_input_event_ask(&e, PICO_EVENT_QUIT);
    assert(has);
    assert(e.type == PICO_EVENT_QUIT);

    pico_init(0);
    return 0;
}
