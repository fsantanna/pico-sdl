#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);

    // pico_get_text_raw: fill w from h
    {
        Pico_Dim d = { 0, 10 };
        pico_get_text_raw("ABC", &d);
        assert(d.w>0 && d.h==10);
    }
    // pico_get_text_pct: fill x from y
    {
        Pico_Pct p = { 0, 0.1 };
        pico_get_text_pct("ABC", &p, NULL);
        assert(p.x>0 && p.y==0.1f);
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
