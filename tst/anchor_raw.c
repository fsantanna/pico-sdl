#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    Pico_Rel_Dim phy = { '!', {200,200}, NULL };
    Pico_Rel_Dim log = { '!', { 10, 10}, NULL };
    pico_set_view("Anchoring Abs...", -1, -1, &phy, NULL, &log, NULL, NULL, NULL);

    // PIXELS
    {
        puts("centered pixel - 1dir/1baixo");
        Pico_Rel_Pos p = { '!', {5,5}, PICO_ANCHOR_C, NULL };
        pico_output_clear();
        pico_output_draw_pixel(&p);
        _pico_check("anchor-01");
    }

    {
        puts("centered pixel - 1dir/1baixo");
        Pico_Rel_Pos p = { '!', {5,5}, PICO_ANCHOR_NW, NULL };
        pico_output_clear();
        pico_output_draw_pixel(&p);
        _pico_check("anchor-02");
    }

    {
        puts("centered pixel - 1esq/1cima");
        Pico_Rel_Pos p = { '!', {5,5}, PICO_ANCHOR_SE, NULL };
        pico_output_clear();
        pico_output_draw_pixel(&p);
        _pico_check("anchor-03");
    }

    // RECTS
    {
        puts("centered rect - exact");
        Pico_Rel_Rect r = { '!', {5, 5, 4, 4}, PICO_ANCHOR_C, NULL };
        pico_output_clear();
        pico_output_draw_rect(&r);
        _pico_check("anchor-04");
    }

    {
        puts("topleft centered - 1lin/1col");
        Pico_Rel_Rect r = { '!', {5, 5, 4, 4}, PICO_ANCHOR_NW, NULL };
        pico_output_clear();
        pico_output_draw_rect(&r);
        _pico_check("anchor-05");
    }

    {
        puts("bottomright centered - 1lin/1col");
        Pico_Rel_Rect r = { '!', {5, 5, 4, 4}, PICO_ANCHOR_SE, NULL };
        pico_output_clear();
        pico_output_draw_rect(&r);
        _pico_check("anchor-06");
    }

    {
        puts("rightmiddle centered - 1col");
        Pico_Rel_Rect r = { '!', {5, 5, 4, 4}, PICO_ANCHOR_E, NULL };
        pico_output_clear();
        pico_output_draw_rect(&r);
        _pico_check("anchor-07");
    }

    {
        puts("anchor 25%25% 20%20% - 1lin/1col");
        Pico_Rel_Rect r = { '!', {2, 2, 4, 4}, {.x=0.2,.y=0.2}, NULL };
        pico_output_clear();
        pico_output_draw_rect(&r);
        _pico_check("anchor-08");
    }

    // RECTS out of [0,10]
    {
        puts("anchor -25%-25% centered - touching border");
        Pico_Rel_Rect r = { '!', {5, 5, 4, 4}, {.x=-0.25,.y=-0.25}, NULL };
        pico_output_clear();
        pico_output_draw_rect(&r);
        _pico_check("anchor-09");
    }

    {
        puts("anchor 125%125% centered - touching border");
        Pico_Rel_Rect r = { '!', {5, 5, 4, 4}, {.x=1.25,.y=1.25}, NULL };
        pico_output_clear();
        pico_output_draw_rect(&r);
        _pico_check("anchor-10");
    }

    pico_init(0);
    return 0;
}
