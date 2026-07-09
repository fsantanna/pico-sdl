#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);

    // pico_get_text reports the NATIVE drawn size: width from the glyph
    // run, height = the font's content-independent cell (>= requested h,
    // here 11 for a requested 10) -- same box the draw path blits.
    // pico_get_text: abs mode, fill w from h
    {
        Pico_Rel_Dim d = { '!', {0, 10} };
        Pico_Abs_Dim r = pico_get_text(&d, "ABC");
        assert(r.w==17 && r.h==11);
    }
    // pico_get_text: pct mode, NULL up (world 100x100, h=0.1 -> 10px)
    {
        Pico_Rel_Dim d = { '%', {0, 0.1} };
        Pico_Abs_Dim r = pico_get_text(&d, "ABC");
        assert(r.w==17 && r.h==11);
    }
    // pico_get_text: pct mode with up (up 50x50, h=0.2 of up -> 10px)
    // after pico_in_dim, d is in scene's % frame
    {
        Pico_Rel_Rect up = { '%', {0, 0, 0.5, 0.5}, PICO_ANCHOR_NW };
        Pico_Rel_Dim d = pico_in_dim(up, (Pico_Rel_Dim){ '%', {0, 0.2} });
        Pico_Abs_Dim r = pico_get_text(&d, "ABC");
        assert(r.w==17 && r.h==11);
        assert(d.w==0.17f && d.h==0.11f);
    }

    {
        Pico_Rel_Rect r = { '!', {10,10, 0,10}, PICO_ANCHOR_NW };
        pico_output_draw_text("hg - gh", r);
        _pico_check("font-01");
    }
    {
        Pico_Rel_Rect r = { '%', {0.5,0.5, 0,0.2}, PICO_ANCHOR_C };
        pico_output_draw_text("hg - gh", r);
        _pico_check("font-02");
    }

    pico_output_clear();

    // pico_set_pencil_font: DejaVuSans.ttf
    pico_set_pencil_font("../res/DejaVuSans.ttf");
    {
        Pico_Rel_Rect r = { '!', {10,10, 0,10}, PICO_ANCHOR_NW };
        pico_output_draw_text("hg - gh", r);
        _pico_check("font-03");
    }
    {
        Pico_Rel_Rect r = { '%', {0.5,0.5, 0,0.2}, PICO_ANCHOR_C };
        pico_output_draw_text("hg - gh", r);
        _pico_check("font-04");
    }

    pico_init(0);
    return 0;
}
