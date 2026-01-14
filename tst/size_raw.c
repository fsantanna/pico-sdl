#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_title("Size - Fullscreen");

    Pico_Dim phy = PICO_DIM_PHY;
    Pico_Dim log = PICO_DIM_LOG;
    Pico_Rect r = {50-25, 50-25, 50, 50};

    // phy: normal -> double -> half -> normal
    puts("WINDOW");
    {
        puts("normal");
        pico_set_view_raw(-1, &phy, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_raw(r);
        _pico_check("size_raw-01");
    }
    {
        puts("double");
        Pico_Dim dim = (Pico_Dim){phy.w*2,phy.h*2};
        pico_set_view_raw(-1, &dim, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_raw(r);
        _pico_check("size_raw-02");
    }
    {
        puts("half");
        Pico_Dim dim = (Pico_Dim){phy.w/2,phy.h/2};
        pico_set_view_raw(-1, &dim, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_raw(r);
        _pico_check("size_raw-03");
    }
    {
        puts("normal");
        pico_set_view_raw(-1, &phy, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_raw(r);
        _pico_check("size_raw-04");
    }

    // phy: normal -> full -> normal
    {
        puts("full");
        pico_set_view_raw(1, NULL, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_raw(r);
        //_pico_check("size_raw-05");
    }
    {
        puts("normal");
        pico_set_view_raw(0, NULL, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_raw(r);
        _pico_check("size_raw-06");
    }

    // log: normal -> double -> half -> normal
    puts("WORLD");
    {
        puts("normal");
        pico_set_view_raw(-1, NULL, NULL, &log, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_raw(r);
        _pico_check("size_raw-07");
    }
    {
        puts("double");
        Pico_Dim dim = (Pico_Dim){log.w*2,log.h*2};
        pico_set_view_raw(-1, NULL, NULL, &dim, NULL, NULL);
        pico_output_clear();
        Pico_Rect r = {log.w-25, log.h-25, 50, 50};
        pico_output_draw_rect_raw(r);
        _pico_check("size_raw-08");
    }
    {
        puts("half");
        Pico_Dim dim = (Pico_Dim){log.w/2,log.h/2};
        pico_set_view_raw(-1, NULL, NULL, &dim, NULL, NULL);
        pico_output_clear();
        Pico_Rect r = {log.w/4-25, log.w/4-25, 50, 50};
        pico_output_draw_rect_raw(r);
        _pico_check("size_raw-09");
    }
    {
        puts("normal");
        pico_set_view_raw(-1, NULL, NULL, &log, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_raw(r);
        _pico_check("size_raw-10");
    }

    pico_init(0);
    return 0;
}
