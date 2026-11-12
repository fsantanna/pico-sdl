#include "pico.h"
#include "../check.h"

int main (void) {
    pico_open(NULL);
    Pico_Rel_Dim log = { '!', { 10, 10} };
    pico_set_window((Pico_Window){ .fs=0, .show=1, .title="Pixmap" });

    pico_set_layer(pico_layer_window());
    pico_set_scene_dim((Pico_Rel_Dim){ '!', {100, 100} });
    pico_set_layer(pico_layer_world());
    pico_set_scene_dim(log);

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
        Pico_Rel_Rect r = { '!', {5-1,5-1,0,0}, PICO_ANCHOR_NW };
        pico_output_clear();
        Pico_Layer* l = pico_layer_pixmap(NULL, (Pico_Abs_Dim){3,3}, pixmap);
        pico_output_draw_layer(l, &r);
        _pico_check("pixmap-01");       // same as pct
        pico_layer_free(l);
    }

    {
        puts("bottomright 9x1 on white");
        pico_set_effect_color((Pico_Color){0xFF, 0xFF, 0xFF, 0xFF});
        pico_output_clear();
        Pico_Rel_Rect r = { '!', {1,9,0,0}, PICO_ANCHOR_NW };
        Pico_Layer* l = pico_layer_pixmap(NULL, (Pico_Abs_Dim){9,1}, pixmap);
        pico_output_draw_layer(l, &r);
        _pico_check("pixmap-02");       // same as pct
        pico_layer_free(l);
    }

    // aspect derivation: only w given, h derived from cell aspect
    // 3x3 pixmap with w=6 → expect 6x6 render
    {
        puts("aspect: w=6 only on 3x3 → 6x6");
        pico_set_effect_color((Pico_Color){0x00, 0x00, 0x00, 0xFF});
        pico_output_clear();
        Pico_Rel_Rect r = { '!', {1,1,6,0}, PICO_ANCHOR_NW };
        Pico_Layer* l = pico_layer_pixmap(NULL, (Pico_Abs_Dim){3,3}, pixmap);
        pico_output_draw_layer(l, &r);
        _pico_check("pixmap-03");       // same as pct
        pico_layer_free(l);
    }

    // distort: both w and h given, non-natural ratio
    // 3x3 pixmap → 6 wide × 3 tall (regression guard, x-stretch)
    {
        puts("distort: w=6, h=3 → x-stretched");
        pico_set_effect_color((Pico_Color){0xFF, 0xFF, 0xFF, 0xFF});
        pico_output_clear();
        Pico_Rel_Rect r = { '!', {1,1,6,3}, PICO_ANCHOR_NW };
        Pico_Layer* l = pico_layer_pixmap(NULL, (Pico_Abs_Dim){3,3}, pixmap);
        pico_output_draw_layer(l, &r);
        _pico_check("pixmap-04");       // same as pct
        pico_layer_free(l);
    }

    pico_close();
    return 0;
}
