#include "pico.h"
#include <assert.h>
#include <string.h>

int main (void) {
    pico_init(1);

    Pico_Abs_Dim dim = { 64, 64 };

    // get_layer returns NULL (main layer)
    puts("get_layer returns NULL initially");
    const char* layer = pico_get_layer();
    assert(layer == NULL);

    // set_layer(NULL) switches to main
    puts("set_layer(NULL) keeps main layer");
    pico_set_layer(NULL);
    layer = pico_get_layer();
    assert(layer == NULL);

    // create layer, set it, verify get returns it
    puts("create and switch to layer");
    const char* bg = pico_layer_empty("background", dim);
    assert(strcmp(bg, "background") == 0);
    pico_set_layer(bg);
    layer = pico_get_layer();
    assert(layer == bg);
    assert(strcmp(layer, "background") == 0);

    // switch to another layer
    puts("switch to another layer");
    const char* ui = pico_layer_empty("ui", dim);
    pico_set_layer(ui);
    layer = pico_get_layer();
    assert(layer == ui);
    assert(strcmp(layer, "ui") == 0);

    // draw on layer (no auto-present)
    puts("draw on layer (no auto-present)");
    pico_set_layer(bg);
    pico_output_clear();
    pico_output_draw_rect(&(Pico_Rel_Rect){ '!', {10, 10, 20, 20}, PICO_ANCHOR_NW, NULL });

    // switch back to main
    puts("switch back to main");
    pico_set_layer(NULL);
    layer = pico_get_layer();
    assert(layer == NULL);

    // present works on main
    puts("present works on main");
    pico_output_present();

    pico_init(0);
    return 0;
}
