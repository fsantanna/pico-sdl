#include "pico.h"
#include <assert.h>
#include <string.h>
#include "../check.h"

int main (void) {
    pico_open(NULL);

    // get_layer returns pico_layer_world() initially
    puts("get_layer returns world initially");
    Pico_Layer* layer = pico_get_layer();
    assert(layer == pico_layer_world());

    // set_layer(pico_layer_world()) is idempotent
    puts("set_layer(world) keeps world layer");
    pico_set_layer(pico_layer_world());
    layer = pico_get_layer();
    assert(layer == pico_layer_world());

    // create bg layer (32x32)
    puts("create and switch to layer");
    Pico_Layer* background = pico_layer_empty(NULL, 1, (Pico_Rel_Dim){'!', {32, 32}}, NULL);
    pico_set_layer(background);
    layer = pico_get_layer();
    assert(layer == background);

    // create ui layer (48x48)
    puts("switch to another layer");
    Pico_Layer* ui = pico_layer_empty(NULL, 1, (Pico_Rel_Dim){'!', {48, 48}}, NULL);
    pico_set_layer(ui);
    layer = pico_get_layer();
    assert(layer == ui);

    // draw on bg layer (red background)
    puts("draw on layer (no auto-present)");
    pico_set_layer(background);
    pico_set_effect_color((Pico_Color){0x80, 0x00, 0x00, 0xFF});
    pico_output_clear();
    pico_output_draw_rect((Pico_Rel_Rect){ '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C });
    pico_set_layer(pico_layer_world());
    pico_output_clear();
    pico_output_draw_layer(background, &(Pico_Rel_Rect){ '%', {0.5, 0.5, 1, 1}, PICO_ANCHOR_C });
    _pico_check("layers-01");

    // draw on ui layer (blue background)
    puts("draw on ui layer");
    pico_set_layer(ui);
    pico_set_effect_color((Pico_Color){0x00, 0x00, 0x80, 0xFF});
    pico_output_clear();
    pico_set_pencil_color((Pico_Color){0x00, 0xFF, 0x00, 0xFF});
    pico_output_draw_rect((Pico_Rel_Rect){ '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C });
    pico_set_layer(pico_layer_world());
    pico_output_clear();
    pico_output_draw_layer(ui, &(Pico_Rel_Rect){ '%', {0.5, 0.5, 1, 1}, PICO_ANCHOR_C });
    _pico_check("layers-02");

    // switch back to world
    puts("switch back to world");
    pico_set_layer(pico_layer_world());
    layer = pico_get_layer();
    assert(layer == pico_layer_world());

    // composite layers onto main
    puts("draw layers onto main");
    pico_set_effect_color((Pico_Color){0x00, 0x00, 0x00, 0xFF});
    pico_output_clear();
    pico_output_draw_layer(background, &(Pico_Rel_Rect){ '%', {1.0/3, 1.0/3, 1.0/3, 1.0/3}, PICO_ANCHOR_C });
    pico_output_draw_layer(ui, &(Pico_Rel_Rect){ '%', {2.0/3, 2.0/3, 1.0/3, 1.0/3}, PICO_ANCHOR_C });
    _pico_check("layers-03");

    // present works on world
    puts("present works on world");
    pico_output_present(1);

    // pico_layer_empty (content preserved)
    puts("layer_empty");
    Pico_Layer* reuse = pico_layer_empty(NULL, 1, (Pico_Rel_Dim){'!', {32, 32}}, NULL);
    pico_set_layer(reuse);
    pico_set_effect_color((Pico_Color){0x00, 0x80, 0x00, 0xFF});
    pico_output_clear();
    pico_set_pencil_color((Pico_Color){0xFF, 0xFF, 0x00, 0xFF});
    pico_output_draw_rect((Pico_Rel_Rect){'%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C});
    pico_set_layer(pico_layer_world());
    pico_set_effect_color((Pico_Color){0x00, 0x00, 0x00, 0xFF});
    pico_output_clear();
    pico_output_draw_layer(reuse, &(Pico_Rel_Rect){'%', {0.5, 0.5, 1, 1}, PICO_ANCHOR_C});
    _pico_check("layers-04");

    // pico_layer_pixmap
    puts("layer_pixmap");
    static Pico_Color buf1[4] = {
        {255, 0, 0, 255}, {0, 255, 0, 255},
        {0, 0, 255, 255}, {255, 255, 0, 255}
    };
    Pico_Layer* mybuf = pico_layer_pixmap(NULL, (Pico_Abs_Dim){2, 2}, buf1);

    // draw pixmap layer
    puts("draw pixmap layer");
    pico_set_layer(pico_layer_world());
    pico_set_effect_color((Pico_Color){0x00, 0x00, 0x00, 0xFF});
    pico_output_clear();
    pico_output_draw_layer(mybuf, &(Pico_Rel_Rect){'%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C});
    _pico_check("layers-05");

    pico_layer_free(mybuf);
    pico_layer_free(reuse);
    pico_layer_free(ui);
    pico_layer_free(background);
    pico_close();
    return 0;
}
