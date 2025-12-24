#include "pico.h"
#include "tst.c"

int main (void) {
    pico_init(1);
    pico_set_title("Panel");

    {
        puts("rect pos=30, dim=50");
        pico_output_clear();

        Pico_Rect r = pico_rect_phy (
            (Pico_Pct) {30, 30},
            (Pico_Pct) {50, 50}
        );

        pico_set_panel("rect");
        pico_set_pos_phy((Pico_Pos){r.x,r.y});
        pico_set_dim_phy((Pico_Dim){r.w,r.h});
        pico_set_dim_log((Pico_Dim){r.w/10,r.h/10});

        pico_set_color_clear((Pico_Color){0xFF, 0x00, 0x00, 0xFF});
        pico_output_clear();
        pico_input_event(NULL, PICO_KEYDOWN);

        pico_set_color_clear((Pico_Color){0xFF, 0xFF, 0xFF, 0xFF});
        pico_output_clear();
        pico_input_event(NULL, PICO_KEYDOWN);
    }

    {
        puts("red centered under white");
        Pico_Rect r = pico_rect_log (
            (Pico_Pct){50, 50},
            (Pico_Pct){50, 50}
        );
        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00,0xFF});
        pico_output_draw_rect(r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }

    {
        pico_set_panel(NULL);
        pico_output_clear();
        pico_input_event(NULL, PICO_KEYDOWN);
        pico_set_alpha(0x88);
        pico_set_panel("rect");
        pico_output_present();
        pico_input_event(NULL, PICO_KEYDOWN);
        //pico_set_alpha(0xFF);
    }

    {
        puts("blue centered under white");
        Pico_Pos pt = pico_pos_log((Pico_Pct){50, 50});
        pico_set_color_draw((Pico_Color){0x00,0x00,0xFF,0xFF});
        pico_output_draw_pixel(pt);
        pico_input_event(NULL, PICO_KEYDOWN);
    }

    {
        puts("yellow clipped under white bottom right");
        Pico_Rect r = pico_rect_log (
            (Pico_Pct){100, 100},
            (Pico_Pct){50, 50}
        );
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0x00,0xFF});
        pico_output_draw_rect(r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }

    pico_init(0);
    return 0;
}
