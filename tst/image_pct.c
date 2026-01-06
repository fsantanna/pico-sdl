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
        pico_output_draw_image_pct(&r, "open.png");
        //_pico_check("img_big_center");
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("show big centered");
        pico_output_clear();
        Pico_Rect_Pct r = { 0.5, 0.5, 1.0, 1.0, PICO_ANCHOR_C, NULL };
        pico_output_draw_image_pct(&r, "open.png");
        //_pico_check("img_big_center");
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("show small centered");
        pico_output_clear();
        Pico_Rect_Pct r = { 0.5, 0.5, 0.2, 0.2, PICO_ANCHOR_C, NULL };
        pico_output_draw_image_pct(&r, "open.png");
        //_pico_check("img_big_center");
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("show w-half proportional");
        pico_output_clear();
        Pico_Rect_Pct r = { 0.5, 0.5, 0.5, 0, PICO_ANCHOR_C, NULL };
        pico_output_draw_image_pct(&r, "open.png");
        //_pico_check("img_big_center");
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("show w-half h-quart distorted");
        pico_output_clear();
        Pico_Rect_Pct r = { 0.5, 0.5, 0.5, 0.25, PICO_ANCHOR_C, NULL };
        pico_output_draw_image_pct(&r, "open.png");
        //_pico_check("img_big_center");
        pico_input_event(NULL, PICO_KEYDOWN);
    }

    pico_init(0);
    return 0;
}
