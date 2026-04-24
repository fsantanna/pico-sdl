#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    Pico_Rel_Dim log = { '!', {10, 10},  NULL };
    pico_set_window((Pico_Window){ .dim={100,100}, .fs=0, .show=1, .title="Pixmap" });
    pico_set_view_dim(NULL, &log);

    // .x.
    // xxx
    // .x.
    Pico_Color pixmap[] = {
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
        Pico_Rel_Rect r = { '%', {0.5,0.5,0,0}, PICO_ANCHOR_C, NULL };
        pico_output_clear();
        pico_output_draw_pixmap("buf1", (Pico_Abs_Dim){3,3}, pixmap, &r);
        _pico_check("pixmap-01");   // same as raw
    }

    {
        puts("bottomright 9x1 on white");
        Pico_Rel_Rect r = { '%', {1,1,0,0}, PICO_ANCHOR_SE, NULL };
        pico_set_show_color(NULL, (Pico_Color){0xFF, 0xFF, 0xFF, 0xFF});
        pico_output_clear();
        pico_output_draw_pixmap("buf2", (Pico_Abs_Dim){9,1}, pixmap, &r);
        _pico_check("pixmap-02");   // same as raw
    }

    pico_init(0);
    return 0;
}
