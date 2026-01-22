#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);

    // pico_get_text_raw: fill w from h
    {
        Pico_Dim d = { 0, 10 };
        pico_get_text_raw("ABC", &d);
        assert(d.w==17 && d.h==10);
    }
    // pico_get_text_pct: NULL ref
    {
        Pico_Pct_WH p = { 0, 0.1 };
        pico_get_text_pct("ABC", &p, NULL);
        assert(p.w==0.17f && p.h==0.1f);
    }
    // pico_get_text_pct: with ref (ref 50x50, h=0.2 -> 10px in ref)
    {
        Pico_Rect_Pct ref = { 0, 0, 0.5, 0.5, PICO_ANCHOR_NW, NULL };
        Pico_Pct_WH p = { 0, 0.2 };
        pico_get_text_pct("ABC", &p, &ref);
        assert(p.w==0.34f && p.h==0.2f);
    }

    {
        Pico_Rect r = { 10,10, 0,10 };
        pico_output_draw_text_raw("hg - gh", r);
        _pico_check("font-01");
    }
    {
        Pico_Rect_Pct r = { 0.5,0.5, 0,0.2, PICO_ANCHOR_C, NULL };
        pico_output_draw_text_pct("hg - gh", &r);
        _pico_check("font-02");
    }

    pico_init(0);
    return 0;
}
