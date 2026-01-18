#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_title("Image - Size - Crop");
    pico_set_color_clear((Pico_Color){0xFF,0xFF,0xFF});

    {
        puts("show original centered");
        pico_output_clear();
        Pico_Rect_Pct r = { 0.5, 0.5, 0, 0, PICO_ANCHOR_C, NULL };
        pico_output_draw_image_pct("open.png", &r);
        _pico_check("image_pct-01");
    }
    {
        puts("show big centered");
        pico_output_clear();
        Pico_Rect_Pct r = { 0.5, 0.5, 1.0, 1.0, PICO_ANCHOR_C, NULL };
        pico_output_draw_image_pct("open.png", &r);
        _pico_check("image_pct-02");
    }
    {
        puts("show small centered");
        pico_output_clear();
        Pico_Rect_Pct r = { 0.5, 0.5, 0.2, 0.2, PICO_ANCHOR_C, NULL };
        pico_output_draw_image_pct("open.png", &r);
        _pico_check("image_pct-03");
    }
    {
        puts("show w-half proportional");
        pico_output_clear();
        Pico_Rect_Pct r = { 0.5, 0.5, 0.5, 0, PICO_ANCHOR_C, NULL };
        pico_output_draw_image_pct("open.png", &r);
        _pico_check("image_pct-04");
    }
    {
        puts("show w-half h-quart distorted");
        pico_output_clear();
        Pico_Rect_Pct r = { 0.5, 0.5, 0.5, 0.25, PICO_ANCHOR_C, NULL };
        pico_output_draw_image_pct("open.png", &r);
        _pico_check("image_pct-05");
    }

    pico_init(0);
    return 0;
}
