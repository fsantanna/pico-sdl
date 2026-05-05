#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_window((Pico_Window){ .dim={640,360}, .fs=0, .show=1, .title="Clear Alpha" });
    pico_set_scene_dim("root", &(Pico_Rel_Dim){'!', {64, 36}, NULL});

    // red background on main
    pico_set_effect_color("root", (Pico_Color){0xFF, 0x00, 0x00, 0xFF});
    pico_output_clear("root");

    // overlay layer: blue clear (semi-transparent), small white rect
    pico_layer_empty(NULL, "overlay", (Pico_Abs_Dim){64, 36}, NULL);
    pico_set_effect_color("overlay", (Pico_Color){0x00, 0x00, 0xFF, 0x80});
    pico_output_clear("overlay");
    pico_set_pencil_color("overlay", (Pico_Color){0xFF, 0xFF, 0xFF, 0xFF});
    pico_output_draw_rect("overlay", &(Pico_Rel_Rect){'%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, NULL});

    // composite overlay on main
    pico_output_draw_layer("root", "overlay", &(Pico_Rel_Rect){'%', {0.5, 0.5, 0.75, 0.75}, PICO_ANCHOR_C, NULL});
    _pico_check("clear_alpha-01");

    pico_init(0);
    return 0;
}
