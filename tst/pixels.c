#include "pico.h"

int main (void) {
    pico_init(1);
    pico_set_title("pixels");
    pico_set_view_raw(-1, &(Pico_Dim){100, 100}, NULL, &(Pico_Dim){5, 5}, NULL, NULL);

    Pico_Pos pixels[9] = {
        {1, 1}, {2, 1}, {3, 1},
        {1, 2}, {2, 2}, {3, 2},
        {1, 3}, {2, 3}, {3, 3}
    };

    printf("press any key to start\n");
    pico_input_event(NULL, PICO_KEYDOWN);

    for (int i=1; i<=9; i++) {
        pico_output_clear();
        pico_output_draw_pixels_raw(pixels, i);
        printf("%d pixels\n", i);
        pico_input_event(NULL, PICO_KEYDOWN);
    }

    pico_init(0);
    return 0;
}
