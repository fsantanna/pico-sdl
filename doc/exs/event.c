// file: event.c
#include <pico.h>

int main() {
    pico_open(NULL);

    SDL_Event event;

    printf("Waiting for an event...\n");
    pico_input_event(&event, 0);
    printf("Event detected! Type: %d\n", event.type);

    pico_close();
    return 0;
}
