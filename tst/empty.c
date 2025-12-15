#include "pico.h"
#include "tst.c"

int main (void) {
    pico_init(1);
    pico_set_title("Empty");
    pico_input_event(NULL, PICO_KEYDOWN);
    pico_init(0);
    return 0;
}
