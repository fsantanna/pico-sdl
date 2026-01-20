#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_title("Image - Size - Crop");
    pico_set_color_clear((Pico_Color){0xFF,0xFF,0xFF});

    // pico_get_image
    {
        {
            Pico_Dim dim = pico_get_image("open.png");
            assert(dim.w==48 && dim.h==48);
        }
        {
            Pico_Dim d = { 0, 24 };
            pico_get_image_raw("open.png", &d);
            assert(d.w==24 && d.h==24);
        }
        {
            Pico_Dim d = { 48, 0 };
            pico_get_image_raw("open.png", &d);
            assert(d.w==48 && d.h==48);
        }
        {
            Pico_Dim d = { 0, 0 };
            pico_get_image_raw("open.png", &d);
            assert(d.w==48 && d.h==48);
        }
    }

    {
        puts("show top-left from center");
        Pico_Rect r = { 50,50, 0,0 };
        pico_output_clear();
        pico_output_draw_image_raw("open.png", r);
        _pico_check("image_raw-01");
    }
    {
        puts("show small centered");
        Pico_Rect r = { 50-5,50-5, 10,10 };
        pico_output_clear();
        pico_output_draw_image_raw("open.png", r);
        _pico_check("image_raw-02");
    }
    {
        puts("show small/medium distorted");
        Pico_Rect r = { 50-5,50-10, 10,20 };
        pico_output_clear();
        pico_output_draw_image_raw("open.png", r);
        _pico_check("image_raw-03");
    }
    {
        puts("show medium normal");
        pico_output_clear();
        Pico_Rect r = { 50-10,50-10, 20,0 };
        pico_output_draw_image_raw("open.png", r);
        _pico_check("image_raw-04");
    }
    {
        puts("show small normal");
        pico_output_clear();
        Pico_Rect r = { 50-5,50-5, 0,10 };
        pico_output_draw_image_raw("open.png", r);
        _pico_check("image_raw-05");
    }
    {
        puts("show big centered");
        pico_output_clear();
        Pico_Rect r = { 50-24,50-24, 0,0 };
        pico_output_draw_image_raw("open.png", r);
        _pico_check("image_raw-06");
    }
#if 0
    {
        pico_output_clear();
        pico_set_crop((Pico_Rect){9,9,30,30});
        pico_output_draw_image_ext(cnt,"open.png",(Pico_Dim){30,30});
        puts("show big croped");
        _pico_check("img_big_cropped");
    }
    {
        pico_output_clear();
        pico_set_crop((Pico_Rect){0,0,0,0});
        pico_output_draw_image_ext(cnt,"open.png",(Pico_Dim){0,15});
        puts("show medium normal");
        _pico_check("img_medium_normal2");
    }
#endif

    pico_init(0);
    return 0;
}
