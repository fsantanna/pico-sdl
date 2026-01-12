#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);

    {
        Pico_Rect r = { 10,10, 0,10 };
        pico_output_draw_text_raw(r, "hg - gh");
        _pico_check("font-01");
    }
    {
        Pico_Rect_Pct r = { 0.5,0.5, 0,0.2, PICO_ANCHOR_C, NULL };
        pico_output_draw_text_pct(&r, "hg - gh");
        _pico_check("font-02");
    }

    pico_init(0);
    return 0;
}
