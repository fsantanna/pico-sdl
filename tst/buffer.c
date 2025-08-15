#include "pico.h"

int main (void) {
    pico_init(1);
    pico_set_title("Buffer");
    pico_set_size((Pico_Dim){100,100}, (Pico_Dim){10,10});
    pico_output_clear();

    // .x.
    // xxx
    // .x.
    Pico_Color buffer[] = {
        { 0x00, 0x00, 0x00, 0x00 },
        { 0xFF, 0xFF, 0x00, 0xFF },
        { 0x00, 0x00, 0x00, 0x00 },
        { 0xFF, 0x00, 0x00, 0xFF },
        { 0x00, 0xFF, 0x00, 0xFF },
        { 0x00, 0x00, 0xFF, 0xFF },
        { 0x00, 0x00, 0x00, 0x00 },
        { 0x00, 0xFF, 0xFF, 0xFF },
        { 0x00, 0x00, 0x00, 0x00 },
    };

    puts("centered transparent multi-color '+' over black");
    Pico_Pos p1 = pico_pos(50,50);
    pico_output_draw_buffer(p1, buffer, (Pico_Dim){3,3});

    puts("press any key");
    pico_input_event(NULL, PICO_KEYDOWN);

    puts("bottom right multi-color '.x.xxx.x.' over white");
    pico_set_color_clear((Pico_Color){0xFF, 0xFF, 0xFF, 0xFF});
    pico_output_clear();
    Pico_Pos p2 = pico_pos(100,100);
    pico_set_anchor((Pico_Anchor){PICO_RIGHT, PICO_BOTTOM});
    pico_output_draw_buffer(p2, buffer, (Pico_Dim){9,1});
    
    puts("press any key");
    pico_input_event(NULL, PICO_KEYDOWN);

    pico_init(0);
    return 0;
}
