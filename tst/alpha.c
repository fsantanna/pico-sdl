#include "pico.h"
#include "tst.c"

int main (void) {
    pico_init(1);
    pico_set_title("Alpha");

    {
        Pico_Rect r1 = pico_rect_log((Pico_Pct){50,50}, (Pico_Pct){50,50});
        pico_set_color_draw((Pico_Color){0xFF, 0x00, 0x00, 0xFF});
        pico_output_draw_rect(r1);
        pico_input_event(NULL, PICO_KEYDOWN);
    }

    {
        Pico_Rect r2 = pico_rect_log((Pico_Pct){50,50}, (Pico_Pct){30,30});
        pico_set_panel("rect");
        pico_set_dim_phy((Pico_Dim){r2.w,r2.h});
        pico_set_dim_log((Pico_Dim){r2.w/10,r2.h/10});
        pico_set_pos_phy((Pico_Pos){r2.x,r2.y});
        pico_set_color_clear((Pico_Color){0xCC, 0xCC, 0xCC, 0xFF});
        pico_output_clear();
        pico_input_event(NULL, PICO_KEYDOWN);
    }

    pico_set_alpha(0x88);
    pico_input_event(NULL, PICO_KEYDOWN);

    pico_init(0);
    return 0;
}

