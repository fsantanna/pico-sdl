#include "pico.h"

int main (void) {
    pico_init(1);
    pico_set_title("Size - Fullscreen");

    Pico_Dim xphy = PICO_DIM_PHY;
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
        Pico_Dim dim = (Pico_Dim){xphy.x*2,xphy.y*2};
        pico_set_view_raw(-1, &dim, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("half");
        Pico_Dim dim = (Pico_Dim){xphy.x/2,xphy.y/2};
        pico_set_view_raw(-1, &dim, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_raw(xr);
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("normal");
        pico_set_view_raw(-1, &xphy, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_raw(xr);
        pico_input_event(NULL, PICO_KEYDOWN);
    }

    // phy: normal -> full -> normal
    {
        puts("full");
        pico_set_view_raw(1, NULL, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_raw(xr);
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("normal");
        pico_set_view_raw(0, NULL, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_raw(xr);
        pico_input_event(NULL, PICO_KEYDOWN);
    }

    // xlog: normal -> double -> half -> normal
    puts("WORLD");
    {
        puts("normal");
        pico_set_view_raw(-1, NULL, NULL, &xlog, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_raw(xr);
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("double");
        Pico_Dim dim = (Pico_Dim){xlog.x*2,xlog.y*2};
        pico_set_view_raw(-1, NULL, NULL, &dim, NULL, NULL);
        pico_output_clear();
        Pico_Rect xr = {xlog.x-25, xlog.y-25, 50, 50};
        pico_output_draw_rect_raw(xr);
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("half");
        Pico_Dim dim = (Pico_Dim){xlog.x/2,xlog.y/2};
        pico_set_view_raw(-1, NULL, NULL, &dim, NULL, NULL);
        pico_output_clear();
        Pico_Rect xr = {xlog.x/4-25, xlog.x/4-25, 50, 50};
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
