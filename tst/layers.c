#include "pico.h"
#include <assert.h>
#include <string.h>
#include "../check.h"

int main (void) {
    pico_init(1);

    // get_layer returns NULL (main layer)
    puts("get_layer returns NULL initially");
    const char* layer = pico_get_layer();
    assert(!strcmp(layer,"root"));

    // set_layer(NULL) switches to main
    puts("set_layer(NULL) keeps main layer");
    pico_set_layer(NULL);
    layer = pico_get_layer();
    assert(!strcmp(layer,"root"));

    // create bg layer (32x32)
    puts("create and switch to layer");
    pico_layer_empty(NULL, "background", (Pico_Abs_Dim){32, 32}, NULL);
    pico_set_layer("background");
    layer = pico_get_layer();
    assert(strcmp(layer, "background") == 0);

    // create ui layer (48x48)
    puts("switch to another layer");
    pico_layer_empty(NULL, "ui", (Pico_Abs_Dim){48, 48}, NULL);
    pico_set_layer("ui");
    layer = pico_get_layer();
    assert(strcmp(layer, "ui") == 0);

    // draw on bg layer (red background)
    puts("draw on layer (no auto-present)");
    pico_set_layer("background");
    pico_set_color_clear((Pico_Color){0x80, 0x00, 0x00, 0xFF});
    pico_output_clear();
    pico_output_draw_rect(&(Pico_Rel_Rect){ '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, NULL });
    pico_set_layer(NULL);
    pico_output_clear();
    pico_output_draw_layer("background", &(Pico_Rel_Rect){ '%', {0.5, 0.5, 1, 1}, PICO_ANCHOR_C, NULL });
    _pico_check("layers-01");

    // draw on ui layer (blue background)
    puts("draw on ui layer");
    pico_set_layer("ui");
    pico_set_color_clear((Pico_Color){0x00, 0x00, 0x80, 0xFF});
    pico_output_clear();
    pico_set_color_draw((Pico_Color){0x00, 0xFF, 0x00, 0xFF});
    pico_output_draw_rect(&(Pico_Rel_Rect){ '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, NULL });
    pico_set_layer(NULL);
    pico_output_clear();
    pico_output_draw_layer("ui", &(Pico_Rel_Rect){ '%', {0.5, 0.5, 1, 1}, PICO_ANCHOR_C, NULL });
    _pico_check("layers-02");

    // switch back to main
    puts("switch back to main");
    pico_set_layer(NULL);
    layer = pico_get_layer();
    assert(!strcmp(layer,"root"));

    // composite layers onto main
    puts("draw layers onto main");
    pico_set_color_clear((Pico_Color){0x00, 0x00, 0x00, 0xFF});
    pico_output_clear();
    pico_output_draw_layer("background", &(Pico_Rel_Rect){ '%', {1.0/3, 1.0/3, 1.0/3, 1.0/3}, PICO_ANCHOR_C, NULL });
    pico_output_draw_layer("ui", &(Pico_Rel_Rect){ '%', {2.0/3, 2.0/3, 1.0/3, 1.0/3}, PICO_ANCHOR_C, NULL });
    _pico_check("layers-03");

    // present works on main
    puts("present works on main");
    pico_output_present();

    // pico_layer_empty reuse (content preserved)
    puts("layer_empty reuse");
    pico_layer_empty_mode('=', NULL, "reuse", (Pico_Abs_Dim){32, 32}, NULL);
    pico_set_layer("reuse");
    pico_set_color_clear((Pico_Color){0x00, 0x80, 0x00, 0xFF});
    pico_output_clear();
    pico_set_color_draw((Pico_Color){0xFF, 0xFF, 0x00, 0xFF});
    pico_output_draw_rect(&(Pico_Rel_Rect){'%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, NULL});
    pico_layer_empty_mode('=', NULL, "reuse", (Pico_Abs_Dim){64, 64}, NULL);
    pico_set_layer(NULL);
    pico_set_color_clear((Pico_Color){0x00, 0x00, 0x00, 0xFF});
    pico_output_clear();
    pico_output_draw_layer("reuse", &(Pico_Rel_Rect){'%', {0.5, 0.5, 1, 1}, PICO_ANCHOR_C, NULL});
    _pico_check("layers-04");

    // pico_layer_buffer with name (reuse)
    puts("layer_buffer with name");
    static Pico_Color buf1[4] = {
        {255, 0, 0, 255}, {0, 255, 0, 255},
        {0, 0, 255, 255}, {255, 255, 0, 255}
    };
    pico_layer_buffer_mode('=', NULL, "mybuf", (Pico_Abs_Dim){2, 2}, buf1);
    pico_layer_buffer_mode('=', NULL, "mybuf", (Pico_Abs_Dim){2, 2}, buf1);

    // pico_layer_buffer with different name
    puts("layer_buffer different name");
    pico_layer_buffer_mode('=', NULL, "xxx", (Pico_Abs_Dim){2, 2}, buf1);
    pico_layer_buffer_mode('=', NULL, "xxx", (Pico_Abs_Dim){2, 2}, buf1);

    // draw buffer layer
    puts("draw buffer layer");
    pico_set_layer(NULL);
    pico_set_color_clear((Pico_Color){0x00, 0x00, 0x00, 0xFF});
    pico_output_clear();
    pico_output_draw_layer("mybuf", &(Pico_Rel_Rect){'%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, NULL});
    _pico_check("layers-05");

    pico_init(0);
    return 0;
}
