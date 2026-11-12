#include "pico.h"
#include "../check.h"

/* Scene-graph smoke test: one child attached to world.
 *
 * Creates an empty layer under pico_layer_world(), draws a red square into it,
 * and relies on auto-composite (present walk) to blit it onto world.
 * No explicit pico_output_draw_layer call.
 */
int main (void) {
    pico_open(NULL);

    pico_set_effect_color(PICO_COLOR_BLACK);
    pico_output_clear();

    Pico_Layer* L = pico_layer_empty(pico_layer_world(), 1, (Pico_Rel_Dim){'!', {8, 8}}, NULL);

    Pico_Layer* old = pico_set_layer(L);
    pico_set_scene_clear(0);
    pico_set_scene_dst((Pico_Rel_Rect){'%', {0.3, 0.3, 0.4, 0.4}, PICO_ANCHOR_C});
    pico_set_effect_color(PICO_COLOR_WHITE);
    pico_output_clear();
    pico_set_pencil_color(PICO_COLOR_RED);
    pico_output_draw_rect(
        (Pico_Rel_Rect){'%', {.5, .5, 0.5, 0.5}, PICO_ANCHOR_C}
    );

    pico_set_layer(old);
    pico_output_present(1);
    _pico_check("layer-hier-01");

    Pico_Layer* img = pico_layer_image(pico_layer_world(), "../res/open.png");
    old = pico_set_layer(img);
    pico_set_scene_dst((Pico_Rel_Rect){'%', {0.7, 0.7, 0.4, 0.4}, PICO_ANCHOR_C});

    pico_set_layer(old);
    pico_output_present(1);
    _pico_check("layer-hier-02");

    static Pico_Color buf[4] = {
        {255, 0, 0, 255}, {0, 255, 0, 255},
        {0, 0, 255, 255}, {255, 255, 0, 255},
    };
    Pico_Layer* pix = pico_layer_pixmap(pico_layer_world(), (Pico_Abs_Dim){2, 2}, buf);
    old = pico_set_layer(pix);
    pico_set_scene_dst((Pico_Rel_Rect){'%', {0.7, 0.3, 0.4, 0.4}, PICO_ANCHOR_C});

    pico_set_layer(old);
    pico_output_present(1);
    _pico_check("layer-hier-03");

    pico_set_pencil_color(PICO_COLOR_GREEN);
    Pico_Layer* txt = pico_layer_text(pico_layer_world(), (Pico_Rel_Dim){'!', {0, 10}}, "hello");
    old = pico_set_layer(txt);
    pico_set_scene_dst((Pico_Rel_Rect){'%', {0.3, 0.7, 0.4, 0.4}, PICO_ANCHOR_C});

    pico_set_layer(old);
    pico_output_present(1);
    _pico_check("layer-hier-04");

    ///////////////////////////////////////////////////////////////////////////

    // transparent overlay with yellow background
    Pico_Layer* over = pico_layer_empty(pico_layer_world(), 1, (Pico_Rel_Dim){'!', {500, 500}}, NULL);
    old = pico_set_layer(over);
    pico_set_effect_color((Pico_Color){0xFF, 0xFF, 0x00, 0xFF});
    pico_output_clear();
    pico_set_scene_dst((Pico_Rel_Rect){'%', {0.5, 0.5, 1, 1}, PICO_ANCHOR_C});
    pico_set_effect_alpha(0x80);
    pico_set_layer(old);
    pico_output_present(1);
    _pico_check("layer-hier-05");

    // sub-layers: crop blue and green from pix, parented under over
    Pico_Layer* blue = pico_layer_sub(over, pix,
        &(Pico_Rel_Rect){'!', {0, 1, 1, 1}, PICO_ANCHOR_NW});
    old = pico_set_layer(blue);
    pico_set_scene_dst((Pico_Rel_Rect){'%', {0.20, 0.9, 0.10, 0.10}, PICO_ANCHOR_C});

    Pico_Layer* green = pico_layer_sub(over, pix,
        &(Pico_Rel_Rect){'!', {1, 0, 1, 1}, PICO_ANCHOR_NW});
    pico_set_layer(green);
    pico_set_scene_dst((Pico_Rel_Rect){'%', {0.40, 0.9, 0.10, 0.10}, PICO_ANCHOR_C});

    pico_set_layer(old);
    pico_output_present(1);
    _pico_check("layer-hier-06");

    pico_layer_free(green);
    pico_layer_free(blue);
    pico_layer_free(over);
    pico_layer_free(txt);
    pico_layer_free(pix);
    pico_layer_free(img);
    pico_layer_free(L);
    pico_close();
    return 0;
}
