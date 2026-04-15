#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_window_title("Size - Fullscreen");

    Pico_Rel_Rect r = { '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, NULL };

    // phy: normal -> double -> half -> normal
    puts("WINDOW");
    {
        puts("normal");
        Pico_Abs_Dim cur = pico_get_window_dim();
        Pico_Rel_Dim dim = { '!', {cur.w * 1, cur.h * 1}, NULL };
        pico_set_window_dim(&dim);
        pico_output_clear();
        pico_output_draw_rect(&r);
        _pico_check("size_pct-01");
    }
    {
        puts("double");
        Pico_Abs_Dim cur = pico_get_window_dim();
        Pico_Rel_Dim dim = { '!', {cur.w * 2, cur.h * 2}, NULL };
        pico_set_window_dim(&dim);
        pico_output_clear();
        pico_output_draw_rect(&r);
        _pico_check("size_pct-02");
    }
    {
        puts("half");
        Pico_Abs_Dim cur = pico_get_window_dim();
        Pico_Rel_Dim dim = { '!', {cur.w * 0.25, cur.h * 0.25}, NULL };
        pico_set_window_dim(&dim);
        pico_output_clear();
        pico_output_draw_rect(&r);
        _pico_check("size_pct-03");
    }
    {
        puts("normal");
        Pico_Abs_Dim cur = pico_get_window_dim();
        Pico_Rel_Dim dim = { '!', {cur.w * 2, cur.h * 2}, NULL };
        pico_set_window_dim(&dim);
        pico_output_clear();
        pico_output_draw_rect(&r);
        _pico_check("size_pct-04");
    }

    // phy: normal -> full -> normal
    {
        puts("full");
        pico_set_window_fs(1);
        pico_output_clear();
        pico_output_draw_rect(&r);
        //_pico_check("size_pct-05");
    }
    {
        puts("normal");
        pico_set_window_fs(0);
        pico_output_clear();
        pico_output_draw_rect(&r);
        _pico_check("size_pct-06");
    }

    // log: normal -> double -> half -> normal
    puts("WORLD");
    {
        puts("normal");
        Pico_Abs_Dim cur;
        cur = pico_get_view_dim(NULL);
        Pico_Rel_Dim dim = { '!', {cur.w * 1, cur.h * 1}, NULL };
        pico_set_view_dim(NULL, &dim);
        pico_output_clear();
        pico_output_draw_rect(&r);
        _pico_check("size_pct-07");
    }
    {
        puts("double");
        Pico_Abs_Dim cur;
        cur = pico_get_view_dim(NULL);
        Pico_Rel_Dim dim = { '!', {cur.w * 2, cur.h * 2}, NULL };
        pico_set_view_dim(NULL, &dim);
        pico_output_clear();
        pico_output_draw_rect(&r);
        _pico_check("size_pct-08");
    }
    {
        puts("half");
        Pico_Abs_Dim cur;
        cur = pico_get_view_dim(NULL);
        Pico_Rel_Dim dim = { '!', {cur.w * 0.25, cur.h * 0.25}, NULL };
        pico_set_view_dim(NULL, &dim);
        pico_output_clear();
        pico_output_draw_rect(&r);
        _pico_check("size_pct-09");
    }
    {
        puts("normal");
        Pico_Abs_Dim cur;
        cur = pico_get_view_dim(NULL);
        Pico_Rel_Dim dim = { '!', {cur.w * 2, cur.h * 2}, NULL };
        pico_set_view_dim(NULL, &dim);
        pico_output_clear();
        pico_output_draw_rect(&r);
        _pico_check("size_pct-10");
    }

    pico_init(0);
    return 0;
}
