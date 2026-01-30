#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_view(NULL, -1, -1,
        &(Pico_Rel_Dim){ '!', {160, 160}, NULL },
        NULL,
        &(Pico_Rel_Dim){ '!', {16, 16}, NULL },
        NULL, NULL, NULL
    );
    pico_output_clear();
    for (int i=0; i<16; i++) {
        pico_output_draw_pixel (
            &(Pico_Rel_Pos) { '!', {i, i}, PICO_ANCHOR_NW, NULL }
        );
        pico_output_draw_pixel (
            &(Pico_Rel_Pos) { '!', {15-i, i}, PICO_ANCHOR_NW, NULL }
        );
        pico_input_delay(10);
        switch (i) {
            case 0:
                _pico_check("cross-01");
                break;
            case 8:
                _pico_check("cross-02");
                break;
            case 15:
                _pico_check("cross-03");
                break;
        }
    }
    pico_init(0);
    return 0;
}
