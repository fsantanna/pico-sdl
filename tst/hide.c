#include "../src/pico.h"

int main (void) {
    pico_init(1);
    pico_set_title("Show - Hide");
    pico_input_event(NULL, SDL_KEYDOWN);

    pico_output_show(0);
    puts("hidden");
    pico_input_delay(1000);

    pico_output_show(1);
    puts("shown");
    pico_input_event(NULL, SDL_KEYDOWN);

    pico_output_show(0);
    puts("hidden");
    pico_input_delay(1000);

    pico_init(0);
    return 0;
}
