#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    Pico_Rel_Dim phy = { '!', {100,100}, NULL };
    Pico_Rel_Dim log = { '!', { 10, 10}, NULL };
    pico_set_window("Buffer", -1, &phy);
    pico_set_view(-1, &log, NULL, NULL, NULL, NULL, NULL, NULL);

    // .x.
    // xxx
    // .x.
    Pico_Color_A buffer[] = {
        { 0x00, 0x00, 0x00, 0xFF },
        { 0xFF, 0xFF, 0x00, 0xFF },
        { 0x00, 0x00, 0x00, 0xFF },
        { 0xFF, 0x00, 0x00, 0xFF },
        { 0x00, 0xFF, 0x00, 0xFF },
        { 0x00, 0x00, 0xFF, 0xFF },
        PICO_COLOR_TRANSPARENT,
        { 0x00, 0xFF, 0xFF, 0xFF },
        PICO_COLOR_TRANSPARENT,
    };

    {
        puts("centered 3x3 on black - 1dir/1baixo");
        Pico_Rel_Rect r = { '!', {5-1,5-1,0,0}, PICO_ANCHOR_NW, NULL };
        pico_output_clear();
        pico_output_draw_buffer("buf1", (Pico_Abs_Dim){3,3}, buffer, &r);
        _pico_check("buffer-01");       // same as pct
    }

    {
        puts("bottomright 9x1 on white");
        pico_set_color_clear((Pico_Color){0xFF, 0xFF, 0xFF});
        pico_output_clear();
        Pico_Rel_Rect r = { '!', {1,9,0,0}, PICO_ANCHOR_NW, NULL };
        pico_output_draw_buffer("buf2", (Pico_Abs_Dim){9,1}, buffer, &r);
        _pico_check("buffer-02");       // same as pct
    }

    pico_init(0);
    return 0;
}
