#include "pico.h"
#include "../check.h"
#include <assert.h>

int main (void) {
    pico_init(1);

    Pico_Rel_Rect btn = { '%', {0.5, 0.5, 0.15, 0.30}, PICO_ANCHOR_C, NULL };

    /* Draw btn into layer A. */
    pico_layer_empty("A", (Pico_Abs_Dim){120, 50});
    pico_set_layer("A");
    pico_set_color_clear(PICO_COLOR_NAVY);
    pico_output_clear();
    pico_set_color_draw(PICO_COLOR_WHITE);
    pico_output_draw_rect(&btn);
    pico_set_layer(NULL);

    /* Composite layer A centered, 50% of screen. */
    Pico_Rel_Rect r = { '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, NULL };
    pico_set_color_clear(PICO_COLOR_BLACK);
    pico_output_clear();
    pico_output_draw_layer("A", &r);

    /* Mouse near visible btn's right edge. */
    pico_set_mouse(&(Pico_Rel_Pos){ 'w', {268, 250}, PICO_ANCHOR_NW, NULL });
    Pico_Mouse pct = pico_get_mouse('%', &r);
    Pico_Rel_Pos pos = { '%', {pct.x, pct.y}, PICO_ANCHOR_NW, NULL };

    /* Collision passes against the phantom screen-% btn... */
    assert(pico_vs_pos_rect(&pos, &btn));

    /* ...but pixel renders outside the visible (layer-A composited) btn. */
    pico_set_color_draw(PICO_COLOR_RED);
    pico_output_draw_pixel(&(Pico_Rel_Pos){ 'w', {268, 250}, PICO_ANCHOR_NW, NULL });
    pico_output_draw_pixel(&pos);
    _pico_check("mouse-w-click-01");

    pico_init(0);
    return 0;
}
