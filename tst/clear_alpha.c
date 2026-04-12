#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_window("Clear Alpha", -1, &(Pico_Rel_Dim){'!', {640, 360}, NULL});
    pico_set_view(-1, &(Pico_Rel_Dim){'!', {64, 36}, NULL}, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    // red background on main
    pico_set_show_color(NULL, (Pico_Color){0xFF, 0x00, 0x00, 0xFF});
    pico_output_clear();

    // overlay layer: blue clear (semi-transparent), small white rect
    pico_layer_empty(NULL, "overlay", (Pico_Abs_Dim){64, 36}, NULL);
    pico_set_layer("overlay");
    pico_set_show_color(NULL, (Pico_Color){0x00, 0x00, 0xFF, 0x80});
    pico_output_clear();
    pico_set_draw_color(NULL, (Pico_Color){0xFF, 0xFF, 0xFF, 0xFF});
    pico_output_draw_rect(&(Pico_Rel_Rect){'%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, NULL});

    // composite overlay on main
    pico_set_layer(NULL);
    pico_output_draw_layer("overlay", &(Pico_Rel_Rect){'%', {0.5, 0.5, 0.75, 0.75}, PICO_ANCHOR_C, NULL});
    _pico_check("clear_alpha-01");

    pico_init(0);
    return 0;
}
