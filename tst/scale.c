#include <pico.h>
#include "tst.c"

int main(void) {
    pico_init(1);
    pico_set_title("Scale");
    pico_set_grid(0);
    pico_set_size((Pico_Dim){500,500}, (Pico_Dim){100, 100});

    Pico_Pos pt = pico_pos((Pico_Pct){50, 50});
    Pico_Rect rect = {pt.x, pt.y, 30, 30};

    pico_output_clear();
    pico_output_draw_rect(rect);
    puts("RECT");
    _pico_check("rect_scale_normal");

    pico_set_scale((Pico_Pct){150,150});
    pico_output_clear();
    pico_output_draw_rect(rect);
    puts("RECT - scale up");
    _pico_check("rect_scale_up");

    pico_set_scale((Pico_Pct){50,50});
    pico_output_clear();
    pico_output_draw_rect(rect);
    puts("RECT - scale dn");
    _pico_check("rect_scale_dn");

    pico_set_anchor_draw((Pico_Anchor){PICO_RIGHT, PICO_BOTTOM});

    pico_output_clear();
    pico_set_scale((Pico_Pct){100,100});
    pico_output_draw_rect(rect);
    puts("RECT (bottom-right)");
    _pico_check("rect_br_scale_normal");

    pico_set_scale((Pico_Pct){150,150});
    pico_output_clear();
    pico_output_draw_rect(rect);
    puts("RECT (bottom-right) - scale up");
    _pico_check("rect_br_scale_up");

    pico_set_scale((Pico_Pct){50,50});
    pico_output_clear();
    pico_output_draw_rect(rect);
    puts("RECT (bottom-right) - scale dn");
    _pico_check("rect_br_scale_dn");
    
    pico_init(0);
    return 0;
}
