#include "pico.h"

int main (void) {
    pico_init(1);

    pico_set_view_raw (
        -1,
        &(Pico_Dim){500, 500},
        NULL,
        &(Pico_Dim){50, 50},
        NULL,
        NULL
    );

    void draw () {
        pico_set_color_draw((Pico_Color) { 0xFF,0x00,0x00 });
        pico_output_draw_pixel_raw((Pico_Pos) { 0,0 });

        pico_set_color_draw((Pico_Color) { 0xFF,0xFF,0xFF });
        pico_output_draw_pixel_raw((Pico_Pos) { 12,12 });

        pico_set_color_draw((Pico_Color) { 0x00,0xFF,0x00 });
        pico_output_draw_pixel_raw((Pico_Pos) { 25,25 });

        pico_set_color_draw((Pico_Color) { 0xAA,0xAA,0xAA });
        pico_set_alpha(0xAA);
        pico_output_draw_pixel_raw((Pico_Pos) { 37,37 });
        pico_set_alpha(0xFF);

        pico_set_color_draw((Pico_Color) { 0x00,0x00,0xFF });
        pico_output_draw_pixel_raw((Pico_Pos) { 49,49 });
    }

    void mouse () {
        Pico_Event e;
        pico_input_event(&e, PICO_MOUSEBUTTONDOWN);
        Pico_Pos m1;
        Pico_Pos_Pct m2 = {0, 0, PICO_ANCHOR_NW, NULL};
        Pico_Pos_Pct m3 = {0, 0, PICO_ANCHOR_C,  NULL};
        pico_get_mouse_raw(&m1, PICO_MOUSE_BUTTON_NONE);
        pico_get_mouse_pct(&m2, PICO_MOUSE_BUTTON_NONE);
        pico_get_mouse_pct(&m3, PICO_MOUSE_BUTTON_NONE);
        printf (
            ">>> (%d,%d) / (%d,%d) / (%f,%f) / (%f,%f)\n",
            e.button.x, e.button.y,
            m1.x, m1.y,
            m2.x, m2.y,
            m3.x, m3.y
        );
        assert(e.button.x==m1.x && e.button.y==m1.y);
    }

    {
        pico_output_clear();
        draw();
        mouse();
    }

    // zoom out
    {
        pico_set_view_pct(-1, NULL, NULL, NULL,
            &(Pico_Rect_Pct){0.5, 0.5, 2, 2, PICO_ANCHOR_C, NULL},
            NULL
        );
        pico_output_clear();
        draw();
        mouse();
    }

    // zoom in
    {
        pico_set_view_pct(-1, NULL, NULL, NULL,
            &(Pico_Rect_Pct){0.5, 0.5, 0.25, 0.25, PICO_ANCHOR_C, NULL},
            NULL
        );
        pico_output_clear();
        draw();
        mouse();
    }

    pico_init(0);
    return 0;
}
