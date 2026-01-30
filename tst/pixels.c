#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_view("pixels", -1, -1,
        &(Pico_Rel_Dim){ '!', {100, 100}, NULL },
        NULL,
        &(Pico_Rel_Dim){ '!', {5, 5}, NULL },
        NULL, NULL, NULL);

    {
        Pico_Rel_Pos pixels[9] = {
            { '!', {1, 1}, PICO_ANCHOR_NW, NULL },
            { '!', {2, 1}, PICO_ANCHOR_NW, NULL },
            { '!', {3, 1}, PICO_ANCHOR_NW, NULL },
            { '!', {1, 2}, PICO_ANCHOR_NW, NULL },
            { '!', {2, 2}, PICO_ANCHOR_NW, NULL },
            { '!', {3, 2}, PICO_ANCHOR_NW, NULL },
            { '!', {1, 3}, PICO_ANCHOR_NW, NULL },
            { '!', {2, 3}, PICO_ANCHOR_NW, NULL },
            { '!', {3, 3}, PICO_ANCHOR_NW, NULL }
        };
        for (int i=1; i<=9; i++) {
            pico_output_clear();
            pico_output_draw_pixels(i, pixels);
            printf("%d pixels\n", i);
            pico_input_delay(10);
        }
        _pico_check("pixels-01");
    }

    {
        Pico_Rel_Pos pixels[4] = {
            { '%', {0, 0}, PICO_ANCHOR_NW, NULL },
            { '%', {1, 0}, PICO_ANCHOR_NE, NULL },
            { '%', {0, 1}, PICO_ANCHOR_SW, NULL },
            { '%', {1, 1}, PICO_ANCHOR_SE, NULL },
        };
        pico_output_draw_pixels(4, pixels);
        _pico_check("pixels-02");
    }

    pico_init(0);
    return 0;
}
