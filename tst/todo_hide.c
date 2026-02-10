#include "pico.h"

int main (void) {
    pico_init(1);
    pico_set_window("Show - Hide", -1, NULL);
    pico_input_event(NULL, PICO_EVENT_KEY_DOWN);

    pico_set_show(0);
    puts("hidden");
    pico_input_delay(1000);

    pico_set_show(1);
    puts("shown");
    pico_input_event(NULL, PICO_EVENT_KEY_DOWN);

    pico_set_show(0);
    puts("hidden");
    pico_input_delay(1000);

    pico_init(0);
    return 0;
}
