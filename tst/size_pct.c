#include "pico.h"

int main (void) {
    pico_init(1);
    pico_set_title("Size - Fullscreen");

    Pico_Dim xlog = PICO_DIM_LOG;
    Pico_Rect xr = {50-25, 50-25, 50, 50};

    Pico_Pct phy = { 0.4, 0.4 };
    Pico_Rect_Pct r = {0.5, 0.5, 0.5, 0.5, PICO_ANCHOR_C, NULL};

    // phy: normal -> double -> half -> normal
    puts("WINDOW");
    {
        puts("normal");
        pico_set_view_pct(-1, &phy, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("double");
        Pico_Pct pct = { 0.8, 0.8 };
        pico_set_view_pct(-1, &pct, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("half");
        Pico_Pct pct = { 0.2, 0.2 };
        pico_set_view_pct(-1, &pct, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("normal");
        pico_set_view_pct(-1, &phy, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }

    // phy: normal -> full -> normal
    {
        puts("full");
        pico_set_view_pct(1, NULL, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("normal");
        pico_set_view_pct(0, NULL, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }

    // log: normal -> double -> half -> normal
    puts("WORLD");
    {
        puts("1/4");
        Pico_Pct log = { 0.25, 0.25 };
        pico_set_view_pct(-1, NULL, NULL, &log, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("double");
        Pico_Pct pct = { 2, 2 };
        pico_set_view_pct(-1, NULL, NULL, &pct, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("half");
        Pico_Dim dim = (Pico_Dim){xlog.w/2,xlog.h/2};
        pico_set_view_raw(-1, NULL, NULL, &dim, NULL, NULL);
        pico_output_clear();
        Pico_Rect xr = {xlog.w/4-25, xlog.w/4-25, 50, 50};
        pico_output_draw_rect_raw(xr);
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("normal");
        pico_set_view_raw(-1, NULL, NULL, &xlog, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_raw(xr);
        pico_input_event(NULL, PICO_KEYDOWN);
    }

    pico_init(0);
    return 0;
}
