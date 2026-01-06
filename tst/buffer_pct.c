#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_title("Buffer");

    Pico_Dim phy = {100,100};
    Pico_Dim log = { 10, 10};
    pico_set_view_raw(-1, &phy, NULL, &log, NULL, NULL);

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
        pico_output_draw_buffer_pct(&r, buffer, (Pico_Dim){3,3});
        //_pico_check("buf3w3h_center_black");
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("bottomright 9x1 on white");
        Pico_Rect_Pct r = { 1,1,0,0, PICO_ANCHOR_SE, NULL };
        pico_set_color_clear((Pico_Color){0xFF, 0xFF, 0xFF});
        pico_output_clear();
        pico_output_draw_buffer_pct(&r, buffer, (Pico_Dim){9,1});
        //_pico_check("buf9w1h_rightbottom_white");
        pico_input_event(NULL, PICO_KEYDOWN);
    }

    pico_init(0);
    return 0;
}
