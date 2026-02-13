#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);

    // pico_get_text: abs mode, fill w from h
    {
        Pico_Rel_Dim d = { '!', {0, 10}, NULL };
        Pico_Abs_Dim r = pico_get_text("ABC", &d);
        assert(r.w==17 && r.h==10);
    }
    // pico_get_text: pct mode, NULL up (world 100x100, h=0.1 -> 10px)
    {
        Pico_Rel_Dim d = { '%', {0, 0.1}, NULL };
        Pico_Abs_Dim r = pico_get_text("ABC", &d);
        assert(r.w==17 && r.h==10);
    }
    // pico_get_text: pct mode with up (up 50x50, h=0.2 -> 10px in up)
    {
        Pico_Rel_Rect up = { '%', {0, 0, 0.5, 0.5}, PICO_ANCHOR_NW, NULL };
        Pico_Rel_Dim d = { '%', {0, 0.2}, &up };
        Pico_Abs_Dim r = pico_get_text("ABC", &d);
        assert(r.w==17 && r.h==10);
        assert(d.w==0.34f && d.h==0.2f);
    }

    {
        Pico_Rel_Rect r = { '!', {10,10, 0,10}, PICO_ANCHOR_NW, NULL };
        pico_output_draw_text("hg - gh", &r);
        _pico_check("font-01");
    }
    {
        Pico_Rel_Rect r = { '%', {0.5,0.5, 0,0.2}, PICO_ANCHOR_C, NULL };
        pico_output_draw_text("hg - gh", &r);
        _pico_check("font-02");
    }

    // non-default font (DejaVuSans)
    pico_set_font("DejaVuSans.ttf");
    {
        pico_output_clear();
        Pico_Rel_Rect r = {
            '!', {10,10, 0,10}, PICO_ANCHOR_NW, NULL
        };
        pico_output_draw_text("hg - gh", &r);
        _pico_check("font-03");
    }

    // switch back to default font
    pico_set_font(NULL);
    {
        pico_output_clear();
        Pico_Rel_Rect r = {
            '!', {10,10, 0,10}, PICO_ANCHOR_NW, NULL
        };
        pico_output_draw_text("hg - gh", &r);
        _pico_check("font-04");
    }

    // push/pop preserves font across switches
    pico_set_font("DejaVuSans.ttf");
    pico_push();
    pico_set_font(NULL);
    pico_pop();
    {
        pico_output_clear();
        Pico_Rel_Rect r = {
            '!', {10,10, 0,10}, PICO_ANCHOR_NW, NULL
        };
        pico_output_draw_text("hg - gh", &r);
        _pico_check("font-05");
    }

    pico_init(0);
    return 0;
}
