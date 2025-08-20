#include "pico.h"
#include "tst.c"

int main (void) {
    pico_init(1);
    pico_set_title("Anchoring...");
    pico_set_size((Pico_Dim){200,200}, (Pico_Dim){10,10});

    // PIXELS
    {
        pico_set_anchor((Pico_Anchor){PICO_CENTER, PICO_MIDDLE});
        Pico_Pos pt = pico_pos(50, 50);
        pico_output_clear();
        pico_output_draw_pixel(pt);
        _pico_assert_output("pixel50x50y_center");
    }

    {
        pico_set_anchor((Pico_Anchor){PICO_LEFT, PICO_TOP});
        Pico_Pos pt = pico_pos(50, 50);
        pico_output_clear();
        pico_output_draw_pixel(pt);
        _pico_assert_output("pixel50x50y_lefttop");
    }

    {
        pico_set_anchor((Pico_Anchor){PICO_RIGHT, PICO_BOTTOM});
        Pico_Pos pt = pico_pos(50, 50);
        pico_output_clear();
        pico_output_draw_pixel(pt);
        _pico_assert_output("pixel50x50y_rightbottom");
    }

    // RECTS
    {
        pico_set_anchor((Pico_Anchor){PICO_CENTER, PICO_MIDDLE});
        Pico_Pos pt = pico_pos(50, 50);
        Pico_Rect rct = { pt.x, pt.y, 4, 4 };
        pico_output_clear();
        pico_output_draw_rect(rct);
        _pico_assert_output("rect50x50y_center");
    }

    {
        pico_set_anchor((Pico_Anchor){PICO_LEFT, PICO_TOP});
        Pico_Pos pt = pico_pos(50, 50);
        Pico_Rect rct = { pt.x, pt.y, 4, 4 };
        pico_output_clear();
        pico_output_draw_rect(rct);
        _pico_assert_output("rect50x50y_lefttop");
    }

    {
        pico_set_anchor((Pico_Anchor){PICO_RIGHT, PICO_BOTTOM});
        Pico_Pos pt = pico_pos(50, 50);
        Pico_Rect rct = { pt.x, pt.y, 4, 4 };
        pico_output_clear();
        pico_output_draw_rect(rct);
        _pico_assert_output("rect50x50y_rightbottom");
    }

    {
        pico_set_anchor((Pico_Anchor){PICO_RIGHT, PICO_MIDDLE});
        Pico_Pos pt = pico_pos(50, 50);
        Pico_Rect rct = { pt.x, pt.y, 4, 4 };
        pico_output_clear();
        pico_output_draw_rect(rct);
        _pico_assert_output("rect50x50y_rightcenter");
    }

    {
        pico_set_anchor((Pico_Anchor){25, 25});
        Pico_Pos pt = pico_pos(20, 20);
        Pico_Rect rct = { pt.x, pt.y, 4, 4 };
        pico_output_clear();
        pico_output_draw_rect(rct);
        _pico_assert_output("rect20x20y_25x25y");
    }
    
    // RECTS out of [0,100]
    {
        pico_set_anchor((Pico_Anchor){-25, -25});
        Pico_Pos pt = pico_pos(50, 50);
        Pico_Rect rct = { pt.x, pt.y, 4, 4 };
        pico_output_clear();
        pico_output_draw_rect(rct);
        _pico_assert_output("rect20x20y_-25x-25y");
    }

    {
        pico_set_anchor((Pico_Anchor){125, 125});
        Pico_Pos pt = pico_pos(50, 50);
        Pico_Rect rct = { pt.x, pt.y, 4, 4 };
        pico_output_clear();
        pico_output_draw_rect(rct);
        _pico_assert_output("rect50x50y_125x125y");
    }

    pico_init(0);
    return 0;
}
