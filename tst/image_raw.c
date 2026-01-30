#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_view("Image - Size - Crop", -1, -1, NULL, NULL, NULL, NULL, NULL, NULL);
    pico_set_color_clear((Pico_Color){0xFF,0xFF,0xFF});

    // pico_get_image
    {
        {
            Pico_Abs_Dim dim = pico_get_image("open.png", NULL);
            assert(dim.w==48 && dim.h==48);
        }
        {
            Pico_Rel_Dim d = { '!', {0, 24}, NULL };
            Pico_Abs_Dim r = pico_get_image("open.png", &d);
            assert(r.w==24 && r.h==24);
        }
        {
            Pico_Rel_Dim d = { '!', {48, 0}, NULL };
            Pico_Abs_Dim r = pico_get_image("open.png", &d);
            assert(r.w==48 && r.h==48);
        }
        {
            Pico_Rel_Dim d = { '!', {0, 0}, NULL };
            Pico_Abs_Dim r = pico_get_image("open.png", &d);
            assert(r.w==48 && r.h==48);
        }
    }

    {
        puts("show top-left from center");
        Pico_Rel_Rect r = { '!', {50,50, 0,0}, PICO_ANCHOR_NW, NULL };
        pico_output_clear();
        pico_output_draw_image("open.png", &r);
        _pico_check("image_raw-01");
    }
    {
        puts("show small centered");
        Pico_Rel_Rect r = { '!', {50-5,50-5, 10,10}, PICO_ANCHOR_NW, NULL };
        pico_output_clear();
        pico_output_draw_image("open.png", &r);
        _pico_check("image_raw-02");
    }
    {
        puts("show small/medium distorted");
        Pico_Rel_Rect r = { '!', {50-5,50-10, 10,20}, PICO_ANCHOR_NW, NULL };
        pico_output_clear();
        pico_output_draw_image("open.png", &r);
        _pico_check("image_raw-03");
    }
    {
        puts("show medium normal");
        pico_output_clear();
        Pico_Rel_Rect r = { '!', {50-10,50-10, 20,0}, PICO_ANCHOR_NW, NULL };
        pico_output_draw_image("open.png", &r);
        _pico_check("image_raw-04");
    }
    {
        puts("show small normal");
        pico_output_clear();
        Pico_Rel_Rect r = { '!', {50-5,50-5, 0,10}, PICO_ANCHOR_NW, NULL };
        pico_output_draw_image("open.png", &r);
        _pico_check("image_raw-05");
    }
    {
        puts("show big centered");
        pico_output_clear();
        Pico_Rel_Rect r = { '!', {50-24,50-24, 0,0}, PICO_ANCHOR_NW, NULL };
        pico_output_draw_image("open.png", &r);
        _pico_check("image_raw-06");
    }

    // CROP
    {
        puts("show big croped");
        pico_output_clear();
        pico_set_crop((Pico_Abs_Rect){9,9,30,30});
        Pico_Rel_Rect r1 = { '!', {50-24,50-24, 0,0}, PICO_ANCHOR_NW, NULL };
        pico_output_draw_image("open.png", &r1);
        _pico_check("image_raw-07");

        puts("show medium normal");
        pico_output_clear();
        pico_set_crop((Pico_Abs_Rect){0,0,0,0});
        Pico_Rel_Rect r2 = { '!', {50-10,50-10, 20,0}, PICO_ANCHOR_NW, NULL };
        pico_output_draw_image("open.png", &r2);
        _pico_check("image_raw-08");
    }

    pico_init(0);
    return 0;
}
