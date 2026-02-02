#include "pico.h"
#include <assert.h>
#include <string.h>

int main (void) {
    pico_init(1);

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
    const char* bg = pico_layer_empty("background");
    assert(strcmp(bg, "background") == 0);
    pico_set_layer(bg);
    layer = pico_get_layer();
    assert(layer == bg);
    assert(strcmp(layer, "background") == 0);

    // switch to another layer
    puts("switch to another layer");
    const char* ui = pico_layer_empty("ui");
    pico_set_layer(ui);
    layer = pico_get_layer();
    assert(layer == ui);
    assert(strcmp(layer, "ui") == 0);

    // switch back to main
    puts("switch back to main");
    pico_set_layer(NULL);
    layer = pico_get_layer();
    assert(layer == NULL);

    pico_init(0);
    return 0;
}
