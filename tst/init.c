#include "pico.h"
#include "../check.h"

int main (void) {
    pico_open(NULL);
    pico_close();
    pico_open(NULL);
    _pico_check("init-01");
    pico_close();
}
