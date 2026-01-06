#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_title("Image - Size - Crop");
    pico_set_color_clear((Pico_Color){0xFF,0xFF,0xFF});

    {
        puts("show top-left from center");
        Pico_Rect r = { 50,50, 0,0 };
        pico_output_clear();
        pico_output_draw_image_raw(r, "open.png");
        //_pico_check("img_big_center");
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("show small centered");
        Pico_Rect r = { 50-5,50-5, 10,10 };
        pico_output_clear();
        pico_output_draw_image_raw(r, "open.png");
        //_pico_check("img_small_center");
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("show small/medium distorted");
        Pico_Rect r = { 50-5,50-10, 10,20 };
        pico_output_clear();
        pico_output_draw_image_raw(r, "open.png");
        //_pico_check("img_smallmedium_distorted");
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("show medium normal");
        pico_output_clear();
        Pico_Rect r = { 50-10,50-10, 20,0 };
        pico_output_draw_image_raw(r, "open.png");
        //_pico_check("img_medium_normal");
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("show small normal");
        pico_output_clear();
        Pico_Rect r = { 50-5,50-5, 0,10 };
        pico_output_draw_image_raw(r, "open.png");
        //_pico_check("img_small_normal");
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("show big centered");
        pico_output_clear();
        Pico_Rect r = { 50-24,50-24, 0,0 };
        pico_output_draw_image_raw(r, "open.png");
        _pico_check("image_raw-01");
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
