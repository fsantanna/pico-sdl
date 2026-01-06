#include <pico.h>
#include "../check.h"

int main(void) {
    pico_init(1);
    pico_set_title("Scale");
    pico_set_grid(0);
    pico_set_view_raw(-1, &(Pico_Dim){500, 500}, NULL, &(Pico_Dim){100, 100}, NULL, NULL);

    Pico_Pos pt = {50, 50};  // Center of 100x100 world
    Pico_Rect rect = {pt.x, pt.y, 30, 30};

    pico_output_clear();
    pico_output_draw_rect(rect);
    puts("RECT");
    _pico_check("scale-01");

    pico_set_scale((Pico_Pct){150,150});
    pico_output_clear();
    pico_output_draw_rect(rect);
    puts("RECT - scale up");
    _pico_check("scale-02");

    pico_set_scale((Pico_Pct){50,50});
    pico_output_clear();
    pico_output_draw_rect(rect);
    puts("RECT - scale dn");
    _pico_check("scale-03");

    pico_set_anchor_pos((Pico_Anchor){PICO_RIGHT, PICO_BOTTOM});

    pico_output_clear();
    pico_set_scale((Pico_Pct){100,100});
    pico_output_draw_rect(rect);
    puts("RECT (bottom-right)");
    _pico_check("scale-04");

    pico_set_scale((Pico_Pct){150,150});
    pico_output_clear();
    pico_output_draw_rect(rect);
    puts("RECT (bottom-right) - scale up");
    _pico_check("scale-05");

    pico_set_scale((Pico_Pct){50,50});
    pico_output_clear();
    pico_output_draw_rect(rect);
    puts("RECT (bottom-right) - scale dn");
    _pico_check("scale-06");
    
    pico_init(0);
    return 0;
}
