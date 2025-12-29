#include "pico.h"
#include "tst.c"

int main (void) {
    pico_init(1);
    pico_set_title("Buffer");
    pico_set_dim_window((Pico_Dim){100,100});
    pico_set_dim_world((Pico_Dim){10,10});

    // .x.
    // xxx
    // .x.
    Pico_Color buffer[] = {
        { 0x00, 0x00, 0x00 },
        { 0xFF, 0xFF, 0x00 },
        { 0x00, 0x00, 0x00 },
        { 0xFF, 0x00, 0x00 },
        { 0x00, 0xFF, 0x00 },
        { 0x00, 0x00, 0xFF },
        { 0x00, 0x00, 0x00 },
        { 0x00, 0xFF, 0xFF },
        { 0x00, 0x00, 0x00 },
    };
    
    {
        puts("centered 3x3 on black - 1dir/1baixo");
        Pico_Rect_Pct r = { 0.5,0.5,0,0, PICO_ANCHOR_C, NULL };
        pico_output_clear();
        pico_output_draw_buffer_pct(&r, buffer, 3, 3);
        //_pico_check("buf3w3h_center_black");
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("bottomright 9x1 on white");
        Pico_Rect_Pct r = { 1,1,0,0, PICO_ANCHOR_SE, NULL };
        pico_set_color_clear((Pico_Color){0xFF, 0xFF, 0xFF});
        pico_output_clear();
        pico_output_draw_buffer_pct(&r, buffer, 9, 1);
        //_pico_check("buf9w1h_rightbottom_white");
        pico_input_event(NULL, PICO_KEYDOWN);
    }

    pico_init(0);
    return 0;
}
