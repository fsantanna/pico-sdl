#include "pico.h"
#include "tst.c"

int main (void) {
    pico_init(1);
    pico_set_title("Buffer");

    Pico_Dim phy = {100,100};
    Pico_Dim log = { 10, 10};
    pico_set_view(-1, &phy, NULL, &log, NULL, NULL);

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
        Pico_Rect r = { 5-1,5-1, 0,0 };
        pico_output_clear();
        pico_output_draw_buffer_raw(r, buffer, (Pico_Dim){3,3});
        _pico_check("buf3w3h_center_black");
    }

    {
        puts("bottomright 9x1 on white");
        pico_set_color_clear((Pico_Color){0xFF, 0xFF, 0xFF});
        pico_output_clear();
        Pico_Rect r = { 1,9, 0,0 };
        pico_set_anchor_pos((Pico_Anchor){PICO_RIGHT, PICO_BOTTOM});
        pico_output_draw_buffer_raw(r, buffer, (Pico_Dim){9,1});
        _pico_check("buf9w1h_rightbottom_white");
    }

    pico_init(0);
    return 0;
}
