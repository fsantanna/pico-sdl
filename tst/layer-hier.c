#include "pico.h"
#include "../check.h"

/* Scene-graph smoke test: one child attached to root.
 *
 * Creates an empty layer under "root", draws a red square into it,
 * and relies on auto-composite (present walk) to blit it onto root.
 * No explicit pico_output_draw_layer call.
 */
int main (void) {
    pico_init(1);

    pico_set_color_clear(PICO_COLOR_BLACK);
    pico_output_clear();

    pico_layer_empty("root", "L", (Pico_Abs_Dim){8, 8}, NULL);

    pico_set_layer("L");
    pico_set_color_clear(PICO_COLOR_BLACK);
    pico_output_clear();
    pico_set_color_draw(PICO_COLOR_RED);
    pico_output_draw_rect(
        &(Pico_Rel_Rect){'%', {.5, .5, 1, 1}, PICO_ANCHOR_C, NULL}
    );
    pico_set_layer(NULL);

    pico_output_present();
    _pico_check("layer-hier-01");

    pico_init(0);
    return 0;
}
