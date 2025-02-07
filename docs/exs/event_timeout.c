// file: event_timeout.c
#include <pico.h>

int main() {
    pico_init(1);

    SDL_Event event;

    printf("Waiting for an event for the next 2 seconds...\n");
    int happened = pico_input_event_timeout(&event, 0, 5000);
    if (happened) {
        printf("Event detected! Type: %d\n", event.type);
    }
    else {
        printf("No event detected in those 2 seconds\n");
    }

    pico_init(0);
    return 0;
}
