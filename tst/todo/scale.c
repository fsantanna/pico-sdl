#include <pico.h>
#include "../check.h"

// Scaling is now expressed by resizing the world scene_dim:
// a smaller world zooms in (content larger), a larger world zooms out.
int main(void) {
    pico_init(1);
    pico_set_window((Pico_Window){ .fs=0, .show=1, .title="Scale" });
    pico_set_layer("window");
    pico_set_scene_dim((Pico_Rel_Dim){ '!', {500, 500} });
    pico_set_layer("world");

    // Rect at the world origin point (50, 50), anchored top-left.
    Pico_Rel_Rect rect = { '!', {50, 50, 30, 30}, PICO_ANCHOR_NW };

    pico_set_scene_dim((Pico_Rel_Dim){ '!', {100, 100} });
    pico_output_clear();
    pico_output_draw_rect(rect);
    puts("RECT");
    _pico_check("scale-01");

    // zoom in: shrink world (100 / 1.5)
    pico_set_scene_dim((Pico_Rel_Dim){ '!', {67, 67} });
    pico_output_clear();
    pico_output_draw_rect(rect);
    puts("RECT - scale up");
    _pico_check("scale-02");

    // zoom out: grow world (100 / 0.5)
    pico_set_scene_dim((Pico_Rel_Dim){ '!', {200, 200} });
    pico_output_clear();
    pico_output_draw_rect(rect);
    puts("RECT - scale dn");
    _pico_check("scale-03");

    // anchor the rect at its bottom-right corner instead
    rect.anchor = PICO_ANCHOR_SE;

    pico_set_scene_dim((Pico_Rel_Dim){ '!', {100, 100} });
    pico_output_clear();
    pico_output_draw_rect(rect);
    puts("RECT (bottom-right)");
    _pico_check("scale-04");

    pico_set_scene_dim((Pico_Rel_Dim){ '!', {67, 67} });
    pico_output_clear();
    pico_output_draw_rect(rect);
    puts("RECT (bottom-right) - scale up");
    _pico_check("scale-05");

    pico_set_scene_dim((Pico_Rel_Dim){ '!', {200, 200} });
    pico_output_clear();
    pico_output_draw_rect(rect);
    puts("RECT (bottom-right) - scale dn");
    _pico_check("scale-06");

    pico_init(0);
    return 0;
}
