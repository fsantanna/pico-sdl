#include "pico.h"
#include "../check.h"
#include <assert.h>

int main (void) {
    pico_init(1);
    pico_set_expert(1, 0);   // expert: window-direct draws persist

    Pico_Rel_Rect btn = { '%', {0.5, 0.5, 0.15, 0.30}, PICO_ANCHOR_C };

    /* Draw btn into layer A. */
    pico_layer_empty(NULL, "A", (Pico_Abs_Dim){120, 50}, NULL);
    const char* old = pico_set_layer("A");
    pico_set_effect_color(PICO_COLOR_NAVY);
    pico_output_clear();
    pico_set_pencil_color(PICO_COLOR_WHITE);
    pico_output_draw_rect(&btn);
    pico_set_layer(old);

    /* Composite layer A distorted at SE, 35%x35% of screen. */
    Pico_Rel_Rect r = { '%', {0.99, 0.99, 0.35, 0.35}, PICO_ANCHOR_SE };
    pico_set_effect_color(PICO_COLOR_BLACK);
    pico_output_clear();
    pico_output_draw_layer("A", &r);

    /* btn was drawn in layer A's frame; re-express btn within r (the
       layer's composite rect on screen) for collision against a mouse
       sampled in scene coords. */
    Pico_Rel_Rect btn_in_r = pico_in_rect(&r, &btn);

    /* Set cursor at window pixel (400, 382); collision in pct (world's
       coords via mouse->pct) confirms inside btn; mark the position
       with a green pixel drawn directly on the window. */
    {
        const char* prev = pico_set_layer("window");
        pico_set_mouse(&(Pico_Rel_Pos){ '!', {400, 382}, PICO_ANCHOR_NW });
        pico_set_layer(prev);
    }
    Pico_Mouse pct = pico_get_mouse('%', NULL);
    Pico_Rel_Pos pos = { '%', {pct.x, pct.y}, PICO_ANCHOR_NW };
    assert(pico_vs_pos_rect(&pos, &btn_in_r));

    pico_set_pencil_color(PICO_COLOR_GREEN);
    {
        const char* prev = pico_set_layer("window");
        pico_output_draw_pixel(&(Pico_Rel_Pos){ '!', {400, 382}, PICO_ANCHOR_NW });
        pico_set_layer(prev);
    }
    pico_output_present();
    _pico_check("mouse-w-click-01");

    pico_init(0);
    return 0;
}
