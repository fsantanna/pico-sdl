#include "pico.h"
#include "tst.c"

int main (void) {
    pico_init(1);
    pico_set_title("Dim");

    Pico_Pos p = pico_pos((Pico_Pct){50, 50});
    char fmt[64];

    pico_output_clear();
    for (int i = 0; i <= 100; i+=25) {
        Pico_Dim d = pico_dim((Pico_Pct){i, i});
        pico_output_draw_rect((Pico_Rect){p.x,p.y,d.x,d.y});

        sprintf(fmt, "%d_screen_size", i);
        puts(fmt);
        _pico_check(fmt);
    }

    for (int i = 0; i <= 120; i+=20) {
        pico_output_clear();
        Pico_Rect r = {p.x,p.y,50,20};
        pico_set_alpha(255);
        pico_set_color_draw((Pico_Color){255,255,255});
        pico_output_draw_rect(r);

        Pico_Dim d = pico_dim_ext((Pico_Pct){i, i}, (Pico_Dim){r.w,r.h});
        pico_set_alpha(150);
        pico_set_color_draw((Pico_Color){255,0,0});
        pico_output_draw_rect((Pico_Rect){p.x,p.y,d.x,d.y});

        sprintf(fmt, "%d_rect_size", i);
        puts(fmt);
        _pico_check(fmt);
    }

    puts("ASSERT ERROR EXPECTED:");
    pico_dim((Pico_Pct){-1, -1});

    pico_init(0);
    return 0;
}
