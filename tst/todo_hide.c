#include "pico.h"

int main (void) {
    pico_init(1);
    pico_set_view("Show - Hide", -1, -1, NULL, NULL, NULL, NULL, NULL);
    pico_input_event(NULL, PICO_KEYDOWN);

    pico_set_show(0);
    puts("hidden");
    pico_input_delay(1000);

    pico_set_show(1);
    puts("shown");
    pico_input_event(NULL, PICO_KEYDOWN);

    pico_set_show(0);
    puts("hidden");
    pico_input_delay(1000);

    pico_init(0);
    return 0;
}
