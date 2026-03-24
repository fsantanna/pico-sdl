#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);

    // pico_get_text: empty string, abs mode -> {0, 0}
    {
        Pico_Rel_Dim d = { '!', {0, 10}, NULL };
        Pico_Abs_Dim r = pico_get_text("", &d);
        assert(r.w==0 && r.h==0);
    }
    // pico_get_text: empty string, pct mode -> {0, 0}
    {
        Pico_Rel_Dim d = { '%', {0, 0.1}, NULL };
        Pico_Abs_Dim r = pico_get_text("", &d);
        assert(r.w==0 && r.h==0);
    }

    // pico_output_draw_text: empty string, abs mode -> no crash, screen stays clear
    {
        pico_output_clear();
        Pico_Rel_Rect r = { '!', {10, 10, 0, 10}, PICO_ANCHOR_NW, NULL };
        pico_output_draw_text("", &r);
        _pico_check("text_empty-01");
    }
    // pico_output_draw_text: empty string, pct mode -> no crash, screen stays clear
    {
        pico_output_clear();
        Pico_Rel_Rect r = { '%', {0.5, 0.5, 0, 0.2}, PICO_ANCHOR_C, NULL };
        pico_output_draw_text("", &r);
        _pico_check("text_empty-02");
    }

    // draw non-empty text, then empty text over it -> original remains
    {
        pico_output_clear();
        Pico_Rel_Rect r1 = { '!', {10, 10, 0, 10}, PICO_ANCHOR_NW, NULL };
        pico_output_draw_text("HELLO", &r1);
        Pico_Rel_Rect r2 = { '!', {10, 10, 0, 10}, PICO_ANCHOR_NW, NULL };
        pico_output_draw_text("", &r2);
        _pico_check("text_empty-03");
    }

    pico_init(0);
    return 0;
}
