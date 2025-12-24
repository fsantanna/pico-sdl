#include "pico.h"

int main (void) {
    pico_init(1);
    pico_set_dim_phy((Pico_Dim){500,500});
    pico_set_dim_log((Pico_Dim){50,50});

    {
        pico_set_anchor_pos((Pico_Anchor){PICO_LEFT, PICO_TOP});

        pico_set_panel(NULL);
        pico_set_color_clear((Pico_Color){0x00, 0x00, 0x00, 0xFF});
        pico_set_color_draw((Pico_Color){0xFF, 0xFF, 0xFF, 0xFF});
        pico_output_clear();
        pico_output_draw_rect((Pico_Rect) { 25,25,5,5 });
        pico_input_event(NULL, PICO_KEYDOWN);

        pico_set_panel("xxx");
        pico_set_pos_phy((Pico_Pos){250,250});
        pico_set_dim_phy((Pico_Dim){200,200});
        pico_set_dim_log((Pico_Dim){10,10});
        pico_set_color_clear((Pico_Color){0xFF, 0xFF, 0x00, 0xFF});
        pico_set_color_draw((Pico_Color){0x00, 0xFF, 0x00, 0xFF});
        pico_output_clear();
        pico_output_draw_rect((Pico_Rect) { 5,5,2,2 });
        pico_input_event(NULL, PICO_KEYDOWN);
    }

    {
        pico_set_anchor_pos((Pico_Anchor){PICO_CENTER, PICO_MIDDLE});

        pico_set_panel(NULL);
        pico_set_color_clear((Pico_Color){0x00, 0x00, 0x00, 0xFF});
        pico_set_color_draw((Pico_Color){0xFF, 0xFF, 0xFF, 0xFF});
        pico_output_clear();
        pico_output_draw_rect((Pico_Rect) { 25,25,5,5 });
        pico_input_event(NULL, PICO_KEYDOWN);

        pico_set_panel("xxx");
        pico_set_pos_phy((Pico_Pos){250,250});
        pico_set_color_clear((Pico_Color){0xFF, 0xFF, 0x00, 0xFF});
        pico_set_color_draw((Pico_Color){0x00, 0xFF, 0x00, 0xFF});
        pico_output_clear();
        pico_output_draw_rect((Pico_Rect) { 5,5,2,2 });
        pico_input_event(NULL, PICO_KEYDOWN);
    }

    pico_init(0);
    return 0;
}
