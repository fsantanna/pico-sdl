#include "pico.h"
#include "../check.h"
#include <string.h>

int main (void) {
    pico_init(1);

    // TITLE
    puts("title: set and get");
    pico_set_window("Test Title", -1, NULL);
    const char* title;
    pico_get_window(&title, NULL, NULL);
    assert(strcmp(title, "Test Title") == 0);
    pico_set_window("View Raw", -1, NULL);

    Pico_Abs_Dim window, world;
    pico_get_window(NULL, NULL, &window);
    pico_get_view(NULL, &world, NULL, NULL, NULL, NULL, NULL, NULL);
    assert(window.w==500 && window.h==500);
    assert(world.w==100 && world.h==100);

    // SIZE (using set_dim for both window and world)
    puts("set_dim: set both equal");
    Pico_Rel_Dim dim = { '!', {window.w, window.h}, NULL };
    pico_set_dim(&dim);
    Pico_Abs_Dim world2;
    pico_get_view(NULL, &world2, NULL, NULL, NULL, NULL, NULL, NULL);
    assert(world2.w==window.w && world2.h==window.h);
    pico_set_view(-1, &(Pico_Rel_Dim){ '!', {world.w, world.h}, NULL },
        NULL, NULL, NULL, NULL, NULL, NULL);  // fallback

    _pico_check("view_raw-0a");
    pico_set_view(0, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    _pico_check("view_raw-0b");
    pico_set_view(1, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    // WORLD - bigger
    puts("shows lower-left X, center rect, center/up-right line");
    for (int i=0; i<50; i++) {
        world.w += 1;
        world.h += 1;
        Pico_Rel_Dim dim = { '!', {world.w, world.h}, NULL };
        pico_set_view(-1, &dim, NULL, NULL, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_set_color_draw(PICO_COLOR_WHITE);
        pico_output_draw_rect (
            &(Pico_Rel_Rect){ '!', {world.w/2-5, world.h/2-5, 10, 10}, PICO_ANCHOR_NW, NULL }
        );
        pico_set_color_draw(PICO_COLOR_RED);
        pico_output_draw_line (
            &(Pico_Rel_Pos){ '%', {0.5, 0.5}, PICO_ANCHOR_C, NULL },
            &(Pico_Rel_Pos){ '%', {1.0, 0}, PICO_ANCHOR_C, NULL }
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
        pico_set_view(-1, NULL, NULL, NULL,
            &(Pico_Rel_Rect){ '!', {i, i, 100, 100}, PICO_ANCHOR_NW, NULL },
            NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_text("Uma frase bem grande...",
            &(Pico_Rel_Rect){ '!', {10, 50, 0, 10}, PICO_ANCHOR_NW, NULL });
        pico_input_delay(10);
        //pico_input_event(NULL, PICO_EVENT_KEY_DOWN);
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
