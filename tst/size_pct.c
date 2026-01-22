#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_title("Size - Fullscreen");

    Pico_Rect_Pct r = {0.5, 0.5, 0.5, 0.5, PICO_ANCHOR_C, NULL};

    // phy: normal -> double -> half -> normal
    puts("WINDOW");
    {
        puts("normal");
        pico_set_view_pct(-1, -1, &(Pico_Pct_WH){1,1}, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        _pico_check("size_pct-01");
    }
    {
        puts("double");
        pico_set_view_pct(-1, -1, &(Pico_Pct_WH){2,2}, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        _pico_check("size_pct-02");
    }
    {
        puts("half");
        pico_set_view_pct(-1, -1, &(Pico_Pct_WH){0.25,0.25}, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        _pico_check("size_pct-03");
    }
    {
        puts("normal");
        pico_set_view_pct(-1, -1, &(Pico_Pct_WH){2,2}, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        _pico_check("size_pct-04");
    }

    // phy: normal -> full -> normal
    {
        puts("full");
        pico_set_view_pct(-1, 1, NULL, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        //_pico_check("size_pct-05");
    }
    {
        puts("normal");
        pico_set_view_pct(-1, 0, NULL, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        _pico_check("size_pct-06");
    }

    // log: normal -> double -> half -> normal
    puts("WORLD");
    {
        puts("normal");
        pico_set_view_pct(-1, -1, NULL, NULL, &(Pico_Pct_WH){1,1}, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        _pico_check("size_pct-07");
    }
    {
        puts("double");
        pico_set_view_pct(-1, -1, NULL, NULL, &(Pico_Pct_WH){2,2}, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        _pico_check("size_pct-08");
    }
    {
        puts("half");
        pico_set_view_pct(-1, -1, NULL, NULL, &(Pico_Pct_WH){0.25,0.25}, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        _pico_check("size_pct-09");
    }
    {
        puts("normal");
        pico_set_view_pct(-1, -1, NULL, NULL, &(Pico_Pct_WH){2,2}, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        _pico_check("size_pct-10");
    }

    pico_init(0);
    return 0;
}
