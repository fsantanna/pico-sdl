#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_window_title("Rect-Circle");
    pico_set_dim((Pico_Rel_Dim){ '!', {200, 100} });

    {
        puts("square from w only (pct)");
        pico_output_clear();
        pico_output_draw_rect((Pico_Rel_Rect){ '%', {0.5, 0.5, 0.25, 0}, PICO_ANCHOR_C });
        _pico_check("rect-circle-01");
    }

    {
        puts("square from h only (pct)");
        pico_output_clear();
        pico_output_draw_rect((Pico_Rel_Rect){ '%', {0.5, 0.5, 0, 0.5}, PICO_ANCHOR_C });
        _pico_check("rect-circle-02");
    }

    {
        puts("circle from w only (pct)");
        pico_output_clear();
        pico_output_draw_oval((Pico_Rel_Rect){ '%', {0.5, 0.5, 0.25, 0}, PICO_ANCHOR_C });
        _pico_check("rect-circle-03");
    }

    {
        puts("circle from h only (raw)");
        pico_output_clear();
        pico_output_draw_oval((Pico_Rel_Rect){ '!', {100, 50, 0, 60}, PICO_ANCHOR_C });
        _pico_check("rect-circle-04");
    }

    {
        puts("both given: rect and oval unchanged");
        pico_output_clear();
        pico_output_draw_rect((Pico_Rel_Rect){ '%', {0.5, 0.5, 0.8, 0.4}, PICO_ANCHOR_C });
        pico_set_pencil_color((Pico_Color){0xFF,0x00,0x00, 0xFF});
        pico_output_draw_oval((Pico_Rel_Rect){ '%', {0.5, 0.5, 0.8, 0.4}, PICO_ANCHOR_C });
        _pico_check("rect-circle-05");
    }

    pico_init(0);
    return 0;
}
