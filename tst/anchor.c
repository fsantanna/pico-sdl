#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_title("Anchoring...");

    Pico_Dim phy = {200,200};
    Pico_Dim log = { 10, 10};
    pico_set_view_raw(-1, -1, &phy, NULL, &log, NULL, NULL);

    // PIXELS
    {
        puts("centered pixel - 1dir/1baixo");
        Pico_Pos_Pct p = { 0.5, 0.50, PICO_ANCHOR_C, NULL };
        pico_output_clear();
        pico_output_draw_pixel_pct(&p);
        _pico_check("anchor-01");
    }

    {
        puts("centered pixel - 1dir/1baixo");
        Pico_Pos_Pct p = { 0.5, 0.5, PICO_ANCHOR_NW, NULL };
        pico_output_clear();
        pico_output_draw_pixel_pct(&p);
        _pico_check("anchor-02");
    }

    {
        puts("centered pixel - 1esq/1cima");
        Pico_Pos_Pct p = { 0.5, 0.5, PICO_ANCHOR_SE, NULL };
        pico_output_clear();
        pico_output_draw_pixel_pct(&p);
        _pico_check("anchor-03");
    }

    // RECTS
    {
        puts("centered rect - exact");
        Pico_Rect_Pct r = { 0.5, 0.5, 0.4, 0.4, PICO_ANCHOR_C, NULL };
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        _pico_check("anchor-04");
    }

    {
        puts("topleft centered - 1lin/1col");
        Pico_Rect_Pct r = { 0.5, 0.5, 0.4, 0.4, PICO_ANCHOR_NW, NULL };
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        _pico_check("anchor-05");
    }

    {
        puts("bottomright centered - 1lin/1col");
        Pico_Rect_Pct r = { 0.5, 0.5, 0.4, 0.4, PICO_ANCHOR_SE, NULL };
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        _pico_check("anchor-06");
    }

    {
        puts("rightmiddle centered - 1col");
        Pico_Rect_Pct r = { 0.5, 0.5, 0.4, 0.4, PICO_ANCHOR_E, NULL };
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        _pico_check("anchor-07");
    }

    {
        puts("anchor 25%25% 20%20% - 1lin/1col");
        Pico_Rect_Pct r = { 0.2, 0.2, 0.4, 0.4, {0.2,0.2}, NULL };
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        _pico_check("anchor-08");
    }

    // RECTS out of [0,100]
    {
        puts("anchor -25%-25% centered - touching border");
        Pico_Rect_Pct r = { 0.5, 0.5, 0.4, 0.4, {-0.25,-0.25}, NULL };
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        _pico_check("anchor-09");
    }

    {
        puts("anchor 125%125% centered - touching border");
        Pico_Rect_Pct r = { 0.5, 0.5, 0.4, 0.4, {1.25,1.25}, NULL };
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        _pico_check("anchor-10");
    }

    pico_init(0);
    return 0;
}
