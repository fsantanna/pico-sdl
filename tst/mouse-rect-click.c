#include "pico.h"
#include "../check.h"
#include <assert.h>

int main (void) {
    pico_init(1);

    /* Layer 120x50 with 3 centered buttons equally spaced */
    Pico_Rel_Rect btn1 = { '%', {0.25, 0.50, 0.15, 0.30}, PICO_ANCHOR_C };
    Pico_Rel_Rect btn2 = { '%', {0.50, 0.50, 0.15, 0.30}, PICO_ANCHOR_C };
    Pico_Rel_Rect btn3 = { '%', {0.75, 0.50, 0.15, 0.30}, PICO_ANCHOR_C };

    pico_layer_empty(NULL, "A", (Pico_Abs_Dim){120, 50}, NULL);
    pico_set_layer("A");
    pico_set_effect_color(PICO_COLOR_NAVY);
    pico_output_clear();
    pico_set_pencil_color(PICO_COLOR_WHITE);
    pico_output_draw_rect(&btn1);
    pico_output_draw_rect(&btn2);
    pico_output_draw_rect(&btn3);
    pico_set_layer("world");

    /* Draw layer at bottom-right, 35%x35% of screen (distorted) */
    Pico_Rel_Rect r = { '%', {0.99, 0.99, 0.35, 0.35}, PICO_ANCHOR_SE };
    pico_set_effect_color(PICO_COLOR_BLACK);
    pico_output_clear();
    pico_output_draw_layer("A", &r);
    _pico_check("mouse-rect-click-01");

    /* re-express buttons within r (layer's composite rect) for
       collision against mouse sampled in world-pct coords */
    Pico_Rel_Rect b1 = pico_in_rect(&r, &btn1);
    Pico_Rel_Rect b2 = pico_in_rect(&r, &btn2);
    Pico_Rel_Rect b3 = pico_in_rect(&r, &btn3);

    /* no collision */
    puts("no collision (394,355)");
    {
        pico_set_layer("window");
        pico_set_mouse(&(Pico_Rel_Pos){'!', {394, 355}, PICO_ANCHOR_C});
        pico_set_layer("world");
        Pico_Mouse pct = pico_get_mouse('%', NULL);
        Pico_Rel_Pos pos = { '%', {pct.x, pct.y}, PICO_ANCHOR_C };
        printf("  pct %5.3f %5.3f\n", pct.x, pct.y);
        assert(!pico_vs_pos_rect(&pos, &b1));
        assert(!pico_vs_pos_rect(&pos, &b2));
        assert(!pico_vs_pos_rect(&pos, &b3));
        pico_set_pencil_color(PICO_COLOR_RED);
        pico_output_draw_pixel(&pos);
        _pico_check("mouse-rect-click-02");
    }

    /* click 3 */
    puts("click 3 (457,431)");
    {
        pico_set_layer("window");
        pico_set_mouse(&(Pico_Rel_Pos){'!', {457, 431}, PICO_ANCHOR_C});
        pico_set_layer("world");
        Pico_Mouse pct = pico_get_mouse('%', NULL);
        Pico_Rel_Pos pos = { '%', {pct.x, pct.y}, PICO_ANCHOR_C };
        printf("  pct %5.3f %5.3f\n", pct.x, pct.y);
        assert(!pico_vs_pos_rect(&pos, &b1));
        assert(!pico_vs_pos_rect(&pos, &b2));
        assert( pico_vs_pos_rect(&pos, &b3));
        pico_set_pencil_color(PICO_COLOR_GREEN);
        pico_output_draw_pixel(&pos);
        _pico_check("mouse-rect-click-03");
    }

    /* click 1 */
    puts("click 1 (362,405)");
    {
        pico_set_layer("window");
        pico_set_mouse(&(Pico_Rel_Pos){'!', {362, 405}, PICO_ANCHOR_C});
        pico_set_layer("world");
        Pico_Mouse pct = pico_get_mouse('%', NULL);
        Pico_Rel_Pos pos = { '%', {pct.x, pct.y}, PICO_ANCHOR_C };
        printf("  pct %5.3f %5.3f\n", pct.x, pct.y);
        assert( pico_vs_pos_rect(&pos, &b1));
        assert(!pico_vs_pos_rect(&pos, &b2));
        assert(!pico_vs_pos_rect(&pos, &b3));
        pico_set_pencil_color(PICO_COLOR_GREEN);
        pico_output_draw_pixel(&pos);
        _pico_check("mouse-rect-click-04");
    }

    /* click 2 */
    puts("click 2 (415,392)");
    {
        pico_set_layer("window");
        pico_set_mouse(&(Pico_Rel_Pos){'!', {419, 392}, PICO_ANCHOR_C});
        pico_set_layer("world");
        Pico_Mouse pct = pico_get_mouse('%', NULL);
        Pico_Rel_Pos pos = { '%', {pct.x, pct.y}, PICO_ANCHOR_C };
        printf("  pct %5.3f %5.3f\n", pct.x, pct.y);
        assert(!pico_vs_pos_rect(&pos, &b1));
        assert( pico_vs_pos_rect(&pos, &b2));
        assert(!pico_vs_pos_rect(&pos, &b3));
        pico_set_pencil_color(PICO_COLOR_GREEN);
        pico_output_draw_pixel(&pos);
        _pico_check("mouse-rect-click-05");
    }

    pico_init(0);
    return 0;
}
