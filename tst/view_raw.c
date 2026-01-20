#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);

    Pico_Dim window, world;
    pico_get_view(NULL, NULL, &window, NULL, &world, NULL, NULL);
    assert(window.w==500 && window.h==500);
    assert(world.w==100 && world.h==100);

    _pico_check("view_raw-0a");
    pico_set_view_raw(0, -1, NULL, NULL, NULL, NULL, NULL);
    _pico_check("view_raw-0b");
    pico_set_view_raw(1, -1, NULL, NULL, NULL, NULL, NULL);

    // WORLD - bigger
    puts("shows lower-left X, center rect, center/up-right line");
    for (int i=0; i<50; i++) {
        world.w += 1;
        world.h += 1;
        pico_set_view_raw(-1, -1, NULL, NULL, &world, NULL, NULL);
        pico_output_clear();
        pico_set_color_draw(PICO_COLOR_WHITE);
        pico_output_draw_rect_raw (
            (Pico_Rect){world.w/2-5, world.h/2-5, 10, 10}
        );
        pico_set_color_draw(PICO_COLOR_RED);
        pico_output_draw_line_pct (
            &(Pico_Pos_Pct){0.5,0.5,PICO_ANCHOR_C,NULL},
            &(Pico_Pos_Pct){1.0,0,PICO_ANCHOR_C,NULL}
        );
        pico_input_delay(10);
        switch (i) {
            case 0:
                _pico_check("view_raw-01");
                break;
            //case 49:
            case 48:
                _pico_check("view_raw-02");
                break;
        }
    }

    // SCROLL - left/up
    puts("scrolls left/up");
    for (int i=0; i<50; i++) {
        pico_set_view_raw(-1, -1, NULL, NULL, NULL,
            &(SDL_Rect){i, i, 100,100},
            NULL);
        pico_output_clear();
        pico_output_draw_text_raw("Uma frase bem grande...", (Pico_Rect){10,50,0,10});
        pico_input_delay(10);
        switch (i) {
            case 0:
                _pico_check("view_raw-03");
                break;
            case 49:
                _pico_check("view_raw-04");
                break;
        }
    }

    pico_init(0);
    return 0;
}
