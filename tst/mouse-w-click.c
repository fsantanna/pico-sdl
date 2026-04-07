#include "pico.h"
#include "../check.h"
#include <assert.h>

int main (void) {
    pico_init(1);

    Pico_Rel_Rect btn = { '%', {0.5, 0.5, 0.15, 0.30}, PICO_ANCHOR_C, NULL };

    /* Draw btn into layer A. */
    pico_layer_empty("A", (Pico_Abs_Dim){120, 50}, NULL);
    pico_set_layer("A");
    pico_set_color_clear(PICO_COLOR_NAVY);
    pico_output_clear();
    pico_set_color_draw(PICO_COLOR_WHITE);
    pico_output_draw_rect(&btn);
    pico_set_layer(NULL);

    /* Composite layer A distorted at SE, 35%x35% of screen. */
    Pico_Rel_Rect r = { '%', {0.99, 0.99, 0.35, 0.35}, PICO_ANCHOR_SE, NULL };
    pico_set_color_clear(PICO_COLOR_BLACK);
    pico_output_clear();
    pico_output_draw_layer("A", &r);

    btn.up = &r;

    /* Win pixel (400,382) is inside btn in r-% (continuous), but 'w' draw
       snaps through the screen log grid (5 win px / log px) to a cell that
       lies above btn's top edge (381.25 win). Collision and rendering
       disagree even with btn.up = &r. */
    pico_set_mouse(&(Pico_Rel_Pos){ 'w', {400, 382}, PICO_ANCHOR_NW, NULL });
    Pico_Mouse pct = pico_get_mouse('%', &r);
    Pico_Rel_Pos pos = { '%', {pct.x, pct.y}, PICO_ANCHOR_NW, &r };

    assert(pico_vs_pos_rect(&pos, &btn));

    pico_set_color_draw(PICO_COLOR_GREEN);
    pico_output_draw_pixel(&(Pico_Rel_Pos){ 'w', {400, 382}, PICO_ANCHOR_NW, NULL });
    _pico_check("mouse-w-click-01");

    pico_init(0);
    return 0;
}
