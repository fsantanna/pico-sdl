#include "pico.h"
#include <assert.h>

int main (void) {
    pico_init(1);

    // get_layer returns NULL (main layer)
    const char* layer = pico_get_layer();
    assert(layer == NULL);

    // set_layer(NULL) switches to main (no-op)
    pico_set_layer(NULL);
    layer = pico_get_layer();
    assert(layer == NULL);

    pico_init(0);
    return 0;
}
