#include "pico.h"

int main (void) {
    pico_init(1);
    pico_output_clear();

    puts("entire screen");
    pico_output_draw_rect_raw((Pico_Rect){10, 10, 20, 20});
    assert(pico_output_screenshot(NULL) != NULL);
    pico_input_event(NULL, PICO_KEYDOWN);

    puts("entire screen - second.png");
    pico_set_color_draw((Pico_Color){200,0,0});
    pico_output_draw_rect_raw((Pico_Rect){30, 30, 10, 10});
    assert(!strcmp(pico_output_screenshot("gen/shot-01.png"),"asr/shot-01.png"));
    pico_input_event(NULL, PICO_KEYDOWN);

    puts("part of screen");
    pico_set_color_draw((Pico_Color){0,200,0});
    pico_output_draw_rect_raw((Pico_Rect){40, 5, 10, 10});
    assert(pico_output_screenshot_ext(NULL, (Pico_Rect){0, 0, 50, 30}) != NULL);
    pico_input_event(NULL, PICO_KEYDOWN);

    pico_set_zoom((Pico_Dim){200, 200});

    puts("zoomed screen");
    pico_set_color_draw((Pico_Color){0,200,0});
    pico_output_draw_rect_raw((Pico_Rect){40, 5, 10, 10});
    assert(pico_output_screenshot(NULL) != NULL);
    pico_input_event(NULL, PICO_KEYDOWN);

    puts("part of zoomed screen");
    pico_set_color_draw((Pico_Color){0,200,0});
    pico_output_draw_rect_raw((Pico_Rect){0, 0, 10, 10});
    assert(pico_output_screenshot_ext(NULL, (Pico_Rect){0, 0, 10, 10}) != NULL);
    pico_input_event(NULL, PICO_KEYDOWN);

    pico_init(0);
    return 0;
}
