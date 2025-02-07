// file: delay.c
#include <pico.h>

int main() {
    pico_init(1);

    printf("Waiting for 2 seconds...\n");
    pico_input_delay(5000);
    printf("Done waiting. Exitting...\n");

    pico_init(0);
    return 0;
}
