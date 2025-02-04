
#include <pico.h>

int main() {
    pico_init(1);

    while (1) {
        int timeout = 16, accum = 0; // timeout is 1000 / FRAMERATE
        while (timeout > 0) {
            int before = pico_get_ticks();
            SDL_Event event;
            pico_input_event_timeout(&event, 0, timeout);
            if (event.type == SDL_QUIT) {
                break;
            }
            // process events ...
            int delta = pico_get_ticks() - before;
            timeout -= delta;
            accum += delta;
        }

        float delta = 0.001f * accum;
        // update ...
        pico_output_clear();
        // draw ...
        pico_output_present();
    }

    pico_init(0);
    return 0;
}