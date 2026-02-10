#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_init(0);
    pico_init(1);
    _pico_check("init-01");
    pico_init(0);
}
