#include "pico.h"
#include "../check.h"
#include <assert.h>

int main (void) {
    pico_init(1);

    Pico_Rel_Rect btn = { '%', {0.5, 0.5, 0.15, 0.30}, PICO_ANCHOR_C };

    /* Draw btn into layer A. */
    pico_layer_empty(NULL, "A", 1, (Pico_Rel_Dim){'!', {120, 50}}, NULL);
    const char* old = pico_set_layer("A");
    pico_set_effect_color(PICO_COLOR_NAVY);
    pico_output_clear();
    pico_set_pencil_color(PICO_COLOR_WHITE);
    pico_output_draw_rect(btn);
    pico_set_layer(old);

    /* Composite layer A distorted at SE, 35%x35% of screen. */
    Pico_Rel_Rect r = { '%', {0.99, 0.99, 0.35, 0.35}, PICO_ANCHOR_SE };
    pico_set_effect_color(PICO_COLOR_BLACK);
    pico_output_clear();
    pico_output_draw_layer("A", &r);

    /* btn was drawn in layer A's frame; re-express btn within r (the
       layer's composite rect on screen) for collision against a mouse
       sampled in scene coords. */
    Pico_Rel_Rect btn_in_r = pico_in_rect(r, btn);

    /* Set cursor at world pct (0.8, 0.764) ~= window (400, 382); collision
       in pct (world's coords via mouse->pct) confirms inside btn; mark
       the position with a green pixel drawn in world (composited 5x to
       window). */
    pico_set_mouse(&(Pico_Rel_Pos){ '%', {0.8, 0.76}, PICO_ANCHOR_NW });
    Pico_Mouse pct = pico_get_mouse('%', NULL);
    Pico_Rel_Pos pos = { '%', {pct.x, pct.y}, PICO_ANCHOR_NW };
    assert(pico_vs_pos_rect(NULL, &pos, NULL, &btn_in_r));

    pico_set_pencil_color(PICO_COLOR_GREEN);
    pico_output_draw_pixel(pos);
    _pico_check("mouse-w-click-01");

    pico_init(0);
    return 0;
}
