#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_view("Image - Size - Crop", -1, -1, NULL, NULL, NULL, NULL, NULL);
    pico_set_color_clear((Pico_Color){0xFF,0xFF,0xFF});

    // pico_get_image: pct mode, NULL ref (world 100x100, image 48x48)
    {
        {
            // h=0.24 -> 24px, aspect 1:1 -> w=24px
            Pico_Rel_Dim d = { '%', {0, 0.24}, NULL };
            Pico_Abs_Dim r = pico_get_image("open.png", &d);
            assert(r.w==24 && r.h==24);
        }
        {
            // w=0.48 -> 48px, aspect 1:1 -> h=48px
            Pico_Rel_Dim d = { '%', {0.48, 0}, NULL };
            Pico_Abs_Dim r = pico_get_image("open.png", &d);
            assert(r.w==48 && r.h==48);
        }
        {
            // w=0, h=0 -> raw dimensions 48x48
            Pico_Rel_Dim d = { '%', {0, 0}, NULL };
            Pico_Abs_Dim r = pico_get_image("open.png", &d);
            assert(r.w==48 && r.h==48);
        }
    }
    // pico_get_image: pct mode with ref (ref 50x50, image 48x48)
    {
        Pico_Rel_Rect ref = { '%', {0, 0, 0.5, 0.5}, PICO_ANCHOR_NW, NULL };
        {
            // w=0, h=0 -> raw dimensions 48x48
            Pico_Rel_Dim d = { '%', {0, 0}, &ref };
            Pico_Abs_Dim r = pico_get_image("open.png", &d);
            assert(r.w==48 && r.h==48);
            assert(d.w==0.96f && d.h==0.96f);
        }
        {
            // h=0.48 of 50 = 24px, aspect 1:1 -> w=24px
            Pico_Rel_Dim d = { '%', {0, 0.48}, &ref };
            Pico_Abs_Dim r = pico_get_image("open.png", &d);
            assert(r.w==24 && r.h==24);
            assert(d.w==0.48f && d.h==0.48f);
        }
    }

    {
        puts("show original centered");
        pico_output_clear();
        Pico_Rel_Rect r = { '%', {0.5, 0.5, 0, 0}, PICO_ANCHOR_C, NULL };
        pico_output_draw_image("open.png", &r);
        _pico_check("image_pct-01");
    }
    {
        puts("show big centered");
        pico_output_clear();
        Pico_Rel_Rect r = { '%', {0.5, 0.5, 1.0, 1.0}, PICO_ANCHOR_C, NULL };
        pico_output_draw_image("open.png", &r);
        _pico_check("image_pct-02");
    }
    {
        puts("show small centered");
        pico_output_clear();
        Pico_Rel_Rect r = { '%', {0.5, 0.5, 0.2, 0.2}, PICO_ANCHOR_C, NULL };
        pico_output_draw_image("open.png", &r);
        _pico_check("image_pct-03");
    }
    {
        puts("show w-half proportional");
        pico_output_clear();
        Pico_Rel_Rect r = { '%', {0.5, 0.5, 0.5, 0}, PICO_ANCHOR_C, NULL };
        pico_output_draw_image("open.png", &r);
        _pico_check("image_pct-04");
    }
    {
        puts("show w-half h-quart distorted");
        pico_output_clear();
        Pico_Rel_Rect r = { '%', {0.5, 0.5, 0.5, 0.25}, PICO_ANCHOR_C, NULL };
        pico_output_draw_image("open.png", &r);
        _pico_check("image_pct-05");
    }

    pico_init(0);
    return 0;
}
