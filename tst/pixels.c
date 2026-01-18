#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_title("pixels");
    pico_set_view_raw(-1, &(Pico_Dim){100, 100}, NULL, &(Pico_Dim){5, 5}, NULL, NULL);

    {
        Pico_Pos pixels[9] = {
            {1, 1}, {2, 1}, {3, 1},
            {1, 2}, {2, 2}, {3, 2},
            {1, 3}, {2, 3}, {3, 3}
        };
        for (int i=1; i<=9; i++) {
            pico_output_clear();
            pico_output_draw_pixels_raw(i, pixels);
            printf("%d pixels\n", i);
            pico_input_delay(10);
        }
        _pico_check("pixels-01");
    }

    {
        Pico_Pos_Pct pixels[4] = {
            {0, 0, PICO_ANCHOR_NW, NULL},
            {1, 0, PICO_ANCHOR_NE, NULL},
            {0, 1, PICO_ANCHOR_SW, NULL},
            {1, 1, PICO_ANCHOR_SE, NULL},
        };
        pico_output_draw_pixels_pct(4, pixels);
        _pico_check("pixels-02");
    }

    pico_init(0);
    return 0;
}
