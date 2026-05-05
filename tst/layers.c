#include "pico.h"
#include <assert.h>
#include <string.h>
#include "../check.h"

int main (void) {
    pico_init(1);

    // create bg layer (32x32) - detached (up=NULL)
    puts("create background layer");
    pico_layer_empty(NULL, "background", (Pico_Abs_Dim){32, 32}, NULL);

    // create ui layer (48x48) - detached
    puts("create ui layer");
    pico_layer_empty(NULL, "ui", (Pico_Abs_Dim){48, 48}, NULL);

    // draw on bg layer (red background)
    puts("draw on background layer directly");
    pico_set_effect_color("background", (Pico_Color){0x80, 0x00, 0x00, 0xFF});
    pico_output_clear("background");
    pico_set_pencil_color("background", (Pico_Color){0xFF, 0xFF, 0xFF, 0xFF});
    pico_output_draw_rect("background", &(Pico_Rel_Rect){ '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, NULL });
    // composite onto root
    pico_output_clear("root");
    pico_output_draw_layer("root", "background", &(Pico_Rel_Rect){ '%', {0.5, 0.5, 1, 1}, PICO_ANCHOR_C, NULL });
    _pico_check("layers-01");

    // draw on ui layer (blue background)
    puts("draw on ui layer directly");
    pico_set_effect_color("ui", (Pico_Color){0x00, 0x00, 0x80, 0xFF});
    pico_output_clear("ui");
    pico_set_pencil_color("ui", (Pico_Color){0x00, 0xFF, 0x00, 0xFF});
    pico_output_draw_rect("ui", &(Pico_Rel_Rect){ '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, NULL });
    // composite onto root
    pico_output_clear("root");
    pico_output_draw_layer("root", "ui", &(Pico_Rel_Rect){ '%', {0.5, 0.5, 1, 1}, PICO_ANCHOR_C, NULL });
    _pico_check("layers-02");

    // composite both layers onto root
    puts("composite layers onto root");
    pico_set_effect_color("root", (Pico_Color){0x00, 0x00, 0x00, 0xFF});
    pico_output_clear("root");
    pico_output_draw_layer("root", "background", &(Pico_Rel_Rect){ '%', {1.0/3, 1.0/3, 1.0/3, 1.0/3}, PICO_ANCHOR_C, NULL });
    pico_output_draw_layer("root", "ui", &(Pico_Rel_Rect){ '%', {2.0/3, 2.0/3, 1.0/3, 1.0/3}, PICO_ANCHOR_C, NULL });
    _pico_check("layers-03");

    // present works
    puts("present works");
    pico_output_present();

    // pico_layer_empty reuse (content preserved)
    puts("layer_empty reuse");
    pico_layer_empty_mode('=', NULL, "reuse", (Pico_Abs_Dim){32, 32}, NULL);
    pico_set_effect_color("reuse", (Pico_Color){0x00, 0x80, 0x00, 0xFF});
    pico_output_clear("reuse");
    pico_set_pencil_color("reuse", (Pico_Color){0xFF, 0xFF, 0x00, 0xFF});
    pico_output_draw_rect("reuse", &(Pico_Rel_Rect){'%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, NULL});
    pico_layer_empty_mode('=', NULL, "reuse", (Pico_Abs_Dim){64, 64}, NULL);
    pico_set_effect_color("root", (Pico_Color){0x00, 0x00, 0x00, 0xFF});
    pico_output_clear("root");
    pico_output_draw_layer("root", "reuse", &(Pico_Rel_Rect){'%', {0.5, 0.5, 1, 1}, PICO_ANCHOR_C, NULL});
    _pico_check("layers-04");

    // pico_layer_pixmap with name (reuse)
    puts("layer_pixmap with name");
    static Pico_Color buf1[4] = {
        {255, 0, 0, 255}, {0, 255, 0, 255},
        {0, 0, 255, 255}, {255, 255, 0, 255}
    };
    pico_layer_pixmap_mode('=', NULL, "mybuf", (Pico_Abs_Dim){2, 2}, buf1);
    pico_layer_pixmap_mode('=', NULL, "mybuf", (Pico_Abs_Dim){2, 2}, buf1);

    // pico_layer_pixmap with different name
    puts("layer_pixmap different name");
    pico_layer_pixmap_mode('=', NULL, "xxx", (Pico_Abs_Dim){2, 2}, buf1);
    pico_layer_pixmap_mode('=', NULL, "xxx", (Pico_Abs_Dim){2, 2}, buf1);

    // draw pixmap layer
    puts("draw pixmap layer");
    pico_set_effect_color("root", (Pico_Color){0x00, 0x00, 0x00, 0xFF});
    pico_output_clear("root");
    pico_output_draw_layer("root", "mybuf", &(Pico_Rel_Rect){'%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, NULL});
    _pico_check("layers-05");

    pico_init(0);
    return 0;
}
