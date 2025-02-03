#include <pico.h>

int main() {
    pico_init(1);

    SDL_Event event;

    printf("Waiting for an event...\n");
    pico_input_event(&event, 0);
    printf("Event detected! Type: %d\n", event.type);

    pico_init(0);
    return 0;
}