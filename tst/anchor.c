#include "pico.h"
#include "tst.c"

int main (void) {
    pico_init(1);
    pico_set_title("Anchoring...");
    pico_set_size((Pico_Dim){200,200}, (Pico_Dim){10,10});

    // PIXELS
    {
        puts("centered pixel - 1dir/1baixo");
        pico_set_anchor_draw((Pico_Anchor){PICO_CENTER, PICO_MIDDLE});
        Pico_Pos pt = pico_pos(50, 50);
        pico_output_clear();
        pico_output_draw_pixel(pt);
        _pico_check_assert("pixel50x50y_center");
    }

    {
        puts("centered pixel - 1dir/1baixo");
        pico_set_anchor_draw((Pico_Anchor){PICO_LEFT, PICO_TOP});
        Pico_Pos pt = pico_pos(50, 50);
        pico_output_clear();
        pico_output_draw_pixel(pt);
        _pico_check_assert("pixel50x50y_lefttop");
    }

    {
        puts("centered pixel - 1esq/1cima");
        pico_set_anchor_draw((Pico_Anchor){PICO_RIGHT, PICO_BOTTOM});
        Pico_Pos pt = pico_pos(50, 50);
        pico_output_clear();
        pico_output_draw_pixel(pt);
        _pico_check_assert("pixel50x50y_rightbottom");
    }

    // RECTS
    {
        puts("centered rect - exact");
        pico_set_anchor_draw((Pico_Anchor){PICO_CENTER, PICO_MIDDLE});
        Pico_Pos pt = pico_pos(50, 50);
        Pico_Rect rct = { pt.x, pt.y, 4, 4 };
        pico_output_clear();
        pico_output_draw_rect(rct);
        _pico_check_assert("rect50x50y_center");
    }

    {
        puts("topleft centered - 1lin/1col");
        pico_set_anchor_draw((Pico_Anchor){PICO_LEFT, PICO_TOP});
        Pico_Pos pt = pico_pos(50, 50);
        Pico_Rect rct = { pt.x, pt.y, 4, 4 };
        pico_output_clear();
        pico_output_draw_rect(rct);
        _pico_check_assert("rect50x50y_lefttop");
    }

    {
        puts("bottomright centered - 1lin/1col");
        pico_set_anchor_draw((Pico_Anchor){PICO_RIGHT, PICO_BOTTOM});
        Pico_Pos pt = pico_pos(50, 50);
        Pico_Rect rct = { pt.x, pt.y, 4, 4 };
        pico_output_clear();
        pico_output_draw_rect(rct);
        _pico_check_assert("rect50x50y_rightbottom");
    }

    {
        puts("rightmiddle centered - 1col");
        pico_set_anchor_draw((Pico_Anchor){PICO_RIGHT, PICO_MIDDLE});
        Pico_Pos pt = pico_pos(50, 50);
        Pico_Rect rct = { pt.x, pt.y, 4, 4 };
        pico_output_clear();
        pico_output_draw_rect(rct);
        _pico_check_assert("rect50x50y_rightcenter");
    }

    {
        puts("anchor 25%25% 20%20% - 1lin/1col");
        pico_set_anchor_draw((Pico_Anchor){25, 25});
        Pico_Pos pt = pico_pos(20, 20);
        Pico_Rect rct = { pt.x, pt.y, 4, 4 };
        pico_output_clear();
        pico_output_draw_rect(rct);
        _pico_check_assert("rect20x20y_25x25y");
    }
    
    // RECTS out of [0,100]
    {
        puts("anchor -25%-25% centered - touching border");
        pico_set_anchor_draw((Pico_Anchor){-25, -25});
        Pico_Pos pt = pico_pos(50, 50);
        Pico_Rect rct = { pt.x, pt.y, 4, 4 };
        pico_output_clear();
        pico_output_draw_rect(rct);
        _pico_check_assert("rect20x20y_-25x-25y");
    }

    {
        puts("anchor 125%125% centered - touching border");
        pico_set_anchor_draw((Pico_Anchor){125, 125});
        Pico_Pos pt = pico_pos(50, 50);
        Pico_Rect rct = { pt.x, pt.y, 4, 4 };
        pico_output_clear();
        pico_output_draw_rect(rct);
        _pico_check_assert("rect50x50y_125x125y");
    }

    pico_init(0);
    return 0;
}
