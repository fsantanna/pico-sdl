#include "pico.h"
#include <assert.h>
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_window((Pico_Window){ .dim={640,360}, .fs=0, .show=1, .title="Keep" });
    pico_set_view_dim(NULL, &(Pico_Rel_Dim){'!', {64, 36}, NULL});
    pico_set_show_grid(NULL, 0);
    pico_output_clear();

    // two empty layers side by side
    pico_layer_empty("root", "left",  (Pico_Abs_Dim){32, 36}, NULL);
    pico_layer_empty("root", "right", (Pico_Abs_Dim){32, 36}, NULL);

    pico_set_layer("left");
    pico_set_view_dst(NULL, (Pico_Rel_Rect){'%', {0.25, 0.5, 0.5, 1}, PICO_ANCHOR_C, NULL});

    pico_set_layer("right");
    pico_set_view_dst(NULL, (Pico_Rel_Rect){'%', {0.75, 0.5, 0.5, 1}, PICO_ANCHOR_C, NULL});

    // right keeps content across frames
    pico_set_show_keep("right", 1);

    // frame 1: draw red rect on both layers
    pico_set_layer("left");
    pico_set_draw_color(NULL, PICO_COLOR_RED);
    pico_output_draw_rect(&(Pico_Rel_Rect){'%', {0.5, 0.5, 0.8, 0.8}, PICO_ANCHOR_C, NULL});

    pico_set_layer("right");
    pico_set_draw_color(NULL, PICO_COLOR_RED);
    pico_output_draw_rect(&(Pico_Rel_Rect){'%', {0.5, 0.5, 0.8, 0.8}, PICO_ANCHOR_C, NULL});

    // composite frame 1: both red
    pico_set_layer(NULL);
    pico_output_present();
    _pico_check("keep-01");

    // frame 2: do NOT draw on layers, just composite again
    // left (keep=0) should be cleared after frame 1 composite
    // right (keep=1) should retain red rect
    pico_output_present();
    _pico_check("keep-02");

    pico_init(0);
    return 0;
}
