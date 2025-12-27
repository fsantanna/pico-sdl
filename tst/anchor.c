#include "pico.h"
#include "tst.c"

int main (void) {
    pico_init(1);
    pico_set_title("Anchoring...");
    pico_set_dim_window((Pico_Dim){200,200});
    pico_set_dim_world((Pico_Dim){10,10});

    // PIXELS
    {
        // TODO: 0.51
        puts("centered pixel - 1dir/1baixo");
        Pico_PixelX p = { 0.5, 0.51, PICO_ANCHOR_C, NULL };
        pico_output_clear();
        pico_output_draw_pixelX(&p);
        _pico_check("pixel50x50y_center");
    }

    {
        puts("centered pixel - 1dir/1baixo");
        Pico_PixelX p = { 0.5, 0.5, PICO_ANCHOR_NW, NULL };
        pico_output_clear();
        pico_output_draw_pixelX(&p);
        _pico_check("pixel50x50y_lefttop");
    }

    {
        puts("centered pixel - 1esq/1cima");
        Pico_PixelX p = { 0.5, 0.5, PICO_ANCHOR_SE, NULL };
        pico_output_clear();
        pico_output_draw_pixelX(&p);
        _pico_check("pixel50x50y_rightbottom");
    }

    // RECTS
    {
        puts("centered rect - exact");
        Pico_RectX r = { 0.5, 0.5, 0.4, 0.4, PICO_ANCHOR_C, NULL };
        pico_output_clear();
        pico_output_draw_rectX(&r);
        _pico_check("rect50x50y_center");
    }

    {
        puts("topleft centered - 1lin/1col");
        Pico_RectX r = { 0.5, 0.5, 0.4, 0.4, PICO_ANCHOR_NW, NULL };
        pico_output_clear();
        pico_output_draw_rectX(&r);
        _pico_check("rect50x50y_lefttop");
    }

    {
        puts("bottomright centered - 1lin/1col");
        Pico_RectX r = { 0.5, 0.5, 0.4, 0.4, PICO_ANCHOR_SE, NULL };
        pico_output_clear();
        pico_output_draw_rectX(&r);
        _pico_check("rect50x50y_rightbottom");
    }

    {
        puts("rightmiddle centered - 1col");
        Pico_RectX r = { 0.5, 0.5, 0.4, 0.4, PICO_ANCHOR_E, NULL };
        pico_output_clear();
        pico_output_draw_rectX(&r);
        _pico_check("rect50x50y_rightcenter");
    }

    {
        puts("anchor 25%25% 20%20% - 1lin/1col");
        Pico_RectX r = { 0.2, 0.2, 0.4, 0.4, {0.2,0.2}, NULL };
        pico_output_clear();
        pico_output_draw_rectX(&r);
        _pico_check("rect20x20y_25x25y");
    }
    
    // RECTS out of [0,100]
    {
        puts("anchor -25%-25% centered - touching border");
        Pico_RectX r = { 0.5, 0.5, 0.4, 0.4, {-0.25,-0.25}, NULL };
        pico_output_clear();
        pico_output_draw_rectX(&r);
        _pico_check("rect20x20y_-25x-25y");
    }

    {
        puts("anchor 125%125% centered - touching border");
        Pico_RectX r = { 0.5, 0.5, 0.4, 0.4, {1.25,1.25}, NULL };
        pico_output_clear();
        pico_output_draw_rectX(&r);
        _pico_check("rect50x50y_125x125y");
    }

    pico_init(0);
    return 0;
}
