#include "pico.h"

int main (void) {
    pico_init(1);
    pico_output_clear();

    puts("entire screen");
    pico_output_draw_rect_raw((Pico_Rect){10, 10, 20, 20});
    assert(pico_output_screenshot(NULL) != NULL);

    puts("entire screen - shot-01.png");
    pico_set_color_draw((Pico_Color){200, 0, 0});
    pico_output_draw_rect_raw((Pico_Rect){30, 30, 10, 10});
    assert(!strcmp(pico_output_screenshot("gen/shot-01.png"), "gen/shot-01.png"));

    puts("part of screen (raw)");
    pico_set_color_draw((Pico_Color){0, 200, 0});
    pico_output_draw_rect_raw((Pico_Rect){40, 5, 10, 10});
    assert(pico_output_screenshot_raw(NULL, (Pico_Rect){0, 0, 250, 150}) != NULL);

    puts("part of screen (pct)");
    pico_set_color_draw((Pico_Color){0, 0, 200});
    pico_output_draw_rect_raw((Pico_Rect){50, 50, 10, 10});
    assert(pico_output_screenshot_pct(NULL,
        &(Pico_Rect_Pct){0, 0, 0.5, 0.3, {0, 0}, NULL}) != NULL);

    pico_init(0);
    puts("All tests passed!");
    return 0;
}
