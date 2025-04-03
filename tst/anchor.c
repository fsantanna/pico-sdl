#include "pico.h"

int main (void) {
    pico_init(1);
    pico_set_title("Anchoring...");
    pico_set_size((Pico_Dim){200,200}, (Pico_Dim){10,10});

    // PIXELS
    Pico_Pos pt = pico_pct_to_pos(50, 50);
    {
        pico_set_anchor(PICO_CENTER, PICO_MIDDLE);
        pico_output_clear();
        pico_output_draw_pixel(pt);
        puts("pixel centralizado - center/middle - fica 1px pra dir/baixo");
        pico_input_event(NULL, PICO_KEYDOWN);

        pico_set_anchor(PICO_LEFT, PICO_TOP);
        pico_output_clear();
        pico_output_draw_pixel(pt);
        puts("pixel centralizado - left/top - fica 1px pra dir/baixo");
        pico_input_event(NULL, PICO_KEYDOWN);

        pico_set_anchor(PICO_RIGHT, PICO_BOTTOM);
        pico_output_clear();
        pico_output_draw_pixel(pt);
        puts("pixel centralizado - right/bottom - fica 1px pra esq/cima");
        pico_input_event(NULL, PICO_KEYDOWN);
    }

    // RECTS
    Pico_Rect rct = { pt.x, pt.y, 4, 4 };
    {
        pico_set_anchor(PICO_CENTER, PICO_MIDDLE);
        pico_output_clear();
        pico_output_draw_rect(rct);
        puts("retangulo centralizado - fica exato");
        pico_input_event(NULL, PICO_KEYDOWN);

        pico_set_anchor(PICO_LEFT, PICO_TOP);
        pico_output_clear();
        pico_output_draw_rect(rct);
        puts("retangulo left/top - sobra uma lin/col");
        pico_input_event(NULL, PICO_KEYDOWN);

        pico_set_anchor(PICO_RIGHT, PICO_BOTTOM);
        pico_output_clear();
        pico_output_draw_rect(rct);
        puts("retangulo right/bottom - sobra uma lin/col");
        pico_input_event(NULL, PICO_KEYDOWN);

        pico_set_anchor(PICO_RIGHT, PICO_MIDDLE);
        pico_output_clear();
        pico_output_draw_rect(rct);
        puts("retangulo right/middle - sobra uma col");
        pico_input_event(NULL, PICO_KEYDOWN);
    }

    pico_init(0);
    return 0;
}
