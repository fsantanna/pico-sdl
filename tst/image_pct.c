#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_title("Image - Size - Crop");
    pico_set_color_clear((Pico_Color){0xFF,0xFF,0xFF});

    // pico_get_image_pct: NULL ref (world 100x100, image 48x48)
    {
        {
            Pico_Pct p = { .w=0, .h=0.24 };
            pico_get_image_pct("open.png", &p, NULL);
            assert(p.w==0.24f && p.h==0.24f);
        }
        {
            Pico_Pct p = { .w=0.48, .h=0 };
            pico_get_image_pct("open.png", &p, NULL);
            assert(p.w==0.48f && p.h==0.48f);
        }
        {
            Pico_Pct p = { .w=0, .h=0 };
            pico_get_image_pct("open.png", &p, NULL);
            assert(p.w==0.48f && p.h==0.48f);
        }
    }
    // pico_get_image_pct: with ref (ref 50x50, image 48x48 -> 0.96x0.96)
    {
        Pico_Rect_Pct ref = { 0, 0, 0.5, 0.5, PICO_ANCHOR_NW, NULL };
        {
            Pico_Pct p = { .w=0, .h=0 };
            pico_get_image_pct("open.png", &p, &ref);
            assert(p.w==0.96f && p.h==0.96f);
        }
        {
            Pico_Pct p = { .w=0, .h=0.48 };
            pico_get_image_pct("open.png", &p, &ref);
            assert(p.w==0.48f && p.h==0.48f);
        }
    }

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
