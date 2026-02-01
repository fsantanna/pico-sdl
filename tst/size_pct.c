#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_window("Size - Fullscreen", -1, NULL);

    Pico_Rel_Rect r = { '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, NULL };

    // phy: normal -> double -> half -> normal
    puts("WINDOW");
    {
        puts("normal");
        Pico_Rel_Dim dim = { '%', {1, 1}, NULL };
        pico_set_window(NULL, -1, &dim);
        pico_output_clear();
        pico_output_draw_rect(&r);
        _pico_check("size_pct-01");
    }
    {
        puts("double");
        Pico_Rel_Dim dim = { '%', {2, 2}, NULL };
        pico_set_window(NULL, -1, &dim);
        pico_output_clear();
        pico_output_draw_rect(&r);
        _pico_check("size_pct-02");
    }
    {
        puts("half");
        Pico_Rel_Dim dim = { '%', {0.25, 0.25}, NULL };
        pico_set_window(NULL, -1, &dim);
        pico_output_clear();
        pico_output_draw_rect(&r);
        _pico_check("size_pct-03");
    }
    {
        puts("normal");
        Pico_Rel_Dim dim = { '%', {2, 2}, NULL };
        pico_set_window(NULL, -1, &dim);
        pico_output_clear();
        pico_output_draw_rect(&r);
        _pico_check("size_pct-04");
    }

    // phy: normal -> full -> normal
    {
        puts("full");
        pico_set_window(NULL, 1, NULL);
        pico_output_clear();
        pico_output_draw_rect(&r);
        //_pico_check("size_pct-05");
    }
    {
        puts("normal");
        pico_set_window(NULL, 0, NULL);
        pico_output_clear();
        pico_output_draw_rect(&r);
        _pico_check("size_pct-06");
    }

    // log: normal -> double -> half -> normal
    puts("WORLD");
    {
        puts("normal");
        Pico_Rel_Dim dim = { '%', {1, 1}, NULL };
        pico_set_view(-1, &dim, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect(&r);
        _pico_check("size_pct-07");
    }
    {
        puts("double");
        Pico_Rel_Dim dim = { '%', {2, 2}, NULL };
        pico_set_view(-1, &dim, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect(&r);
        _pico_check("size_pct-08");
    }
    {
        puts("half");
        Pico_Rel_Dim dim = { '%', {0.25, 0.25}, NULL };
        pico_set_view(-1, &dim, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect(&r);
        _pico_check("size_pct-09");
    }
    {
        puts("normal");
        Pico_Rel_Dim dim = { '%', {2, 2}, NULL };
        pico_set_view(-1, &dim, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect(&r);
        _pico_check("size_pct-10");
    }

    pico_init(0);
    return 0;
}
