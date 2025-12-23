#include "pico.h"

int main (void) {
    pico_init(1);

    pico_set_dim_phy((Pico_Dim){500,500});
    pico_set_dim_log((Pico_Dim){50,50});

    pico_set_anchor_pos((Pico_Anchor){PICO_LEFT,PICO_TOP});
    pico_set_style(PICO_STROKE);

    pico_output_clear();
    pico_output_draw_rect((Pico_Rect) { 0,0,50,50 });
    pico_output_draw_pixel((Pico_Pos) { 25,25 });
    pico_input_event(NULL, PICO_MOUSEBUTTONDOWN);

    pico_output_clear();
    pico_set_crop((Pico_Rect){0, 0, 50, 50});
    pico_output_draw_rect((Pico_Rect) { 0,0,50,50 });
    pico_output_draw_pixel((Pico_Pos) { 25,25 });
    pico_input_event(NULL, PICO_MOUSEBUTTONDOWN);

    pico_output_clear();
    pico_set_crop((Pico_Rect){12, 12, 25, 25});
    pico_output_draw_rect((Pico_Rect) { 12,12,25,25 });
    pico_output_draw_pixel((Pico_Pos) { 13,13 });
    pico_output_draw_pixel((Pico_Pos) { 25,25 });
    pico_input_event(NULL, PICO_MOUSEBUTTONDOWN);

    pico_output_clear();
    pico_set_crop((Pico_Rect){-25, -25, 100, 100});
    pico_output_draw_rect((Pico_Rect) { 0,0,50,50 });
    pico_output_draw_pixel((Pico_Pos) { 25,25 });
    pico_input_event(NULL, PICO_MOUSEBUTTONDOWN);

    pico_init(0);
    return 0;
}
