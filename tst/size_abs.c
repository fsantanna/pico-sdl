#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_title("Size - Fullscreen");

    Pico_Abs_Dim phy = PICO_DIM_PHY;
    Pico_Abs_Dim log = PICO_DIM_LOG;
    Pico_Rel_Rect r = { '!', {50-25, 50-25, 50, 50}, PICO_ANCHOR_NW, NULL };

    // phy: normal -> double -> half -> normal
    puts("WINDOW");
    {
        puts("normal");
        Pico_Rel_Dim dim = { '!', {phy.w, phy.h}, NULL };
        pico_set_view(-1, -1, &dim, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect(&r);
        _pico_check("size_abs-01");
    }
    {
        puts("double");
        Pico_Rel_Dim dim = { '!', {phy.w*2, phy.h*2}, NULL };
        pico_set_view(-1, -1, &dim, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect(&r);
        _pico_check("size_abs-02");
    }
    {
        puts("half");
        Pico_Rel_Dim dim = { '!', {phy.w/2, phy.h/2}, NULL };
        pico_set_view(-1, -1, &dim, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect(&r);
        _pico_check("size_abs-03");
    }
    {
        puts("normal");
        Pico_Rel_Dim dim = { '!', {phy.w, phy.h}, NULL };
        pico_set_view(-1, -1, &dim, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect(&r);
        _pico_check("size_abs-04");
    }

    // phy: normal -> full -> normal
    {
        puts("full");
        pico_set_view(-1, 1, NULL, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect(&r);
        //_pico_check("size_abs-05");
    }
    {
        puts("normal");
        pico_set_view(-1, 0, NULL, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect(&r);
        _pico_check("size_abs-06");
    }

    // log: normal -> double -> half -> normal
    puts("WORLD");
    {
        puts("normal");
        Pico_Rel_Dim dim = { '!', {log.w, log.h}, NULL };
        pico_set_view(-1, -1, NULL, NULL, &dim, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect(&r);
        _pico_check("size_abs-07");
    }
    {
        puts("double");
        Pico_Rel_Dim dim = { '!', {log.w*2, log.h*2}, NULL };
        pico_set_view(-1, -1, NULL, NULL, &dim, NULL, NULL);
        pico_output_clear();
        Pico_Rel_Rect r = { '!', {log.w-25, log.h-25, 50, 50}, PICO_ANCHOR_NW, NULL };
        pico_output_draw_rect(&r);
        _pico_check("size_abs-08");
    }
    {
        puts("half");
        Pico_Rel_Dim dim = { '!', {log.w/2, log.h/2}, NULL };
        pico_set_view(-1, -1, NULL, NULL, &dim, NULL, NULL);
        pico_output_clear();
        Pico_Rel_Rect r = { '!', {log.w/4-25, log.w/4-25, 50, 50}, PICO_ANCHOR_NW, NULL };
        pico_output_draw_rect(&r);
        _pico_check("size_abs-09");
    }
    {
        puts("normal");
        Pico_Rel_Dim dim = { '!', {log.w, log.h}, NULL };
        pico_set_view(-1, -1, NULL, NULL, &dim, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect(&r);
        _pico_check("size_abs-10");
    }

    pico_init(0);
    return 0;
}
