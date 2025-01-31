#include "../src/pico.h"

int main (void) {
    pico_init(1);
    pico_set_title("pixels");
    pico_set_anchor(PICO_LEFT, PICO_TOP);

    SDL_Point pixels[9] = {
        {0, 0}, {1, 0}, {2, 0},
        {0, 1}, {1, 1}, {2, 1},
        {0, 2}, {1, 2}, {2, 2}
    };

    printf("press any key to start\n");
    pico_input_event(NULL, SDL_KEYDOWN);
    
    for (int i = 0; i < 9; i++) {
        pico_output_clear();
        pico_output_draw_pixels(pixels, i);
        printf("%d pixels\n", i + 1);
        pico_output_present();
        pico_input_event(NULL, SDL_KEYDOWN);
    }

    pico_init(0);
    return 0;
}