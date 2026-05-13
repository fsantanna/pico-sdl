#include "pico.h"
#include "../check.h"
#include <string.h>

int main (void) {
    pico_init(1);

    // TITLE
    puts("title: set and get");
    pico_set_window_title("Test Title");
    const char* title = pico_get_window_title();
    assert(strcmp(title, "Test Title") == 0);
    pico_set_window_title("View Raw");

    Pico_Abs_Dim window, world;
    pico_set_layer("window");
    window = pico_get_scene_dim();
    pico_set_layer("world");
    world = pico_get_scene_dim();
    assert(window.w==500 && window.h==500);
    assert(world.w==100 && world.h==100);

    // SIZE (using set_dim for both window and world)
    puts("set_dim: set both equal");
    Pico_Rel_Dim dim = { '!', {window.w, window.h} };
    pico_set_dim(&dim);
    Pico_Abs_Dim world2;
    world2 = pico_get_scene_dim();
    assert(world2.w==window.w && world2.h==window.h);
    pico_set_scene_dim(&(Pico_Rel_Dim){ '!', {world.w, world.h} });

    _pico_check("view_raw-0a");
    pico_set_effect_grid(0);
    _pico_check("view_raw-0b");
    pico_set_effect_grid(1);

    // WORLD - bigger
    puts("shows lower-left X, center rect, center/up-right line");
    for (int i=0; i<50; i++) {
        world.w += 1;
        world.h += 1;
        Pico_Rel_Dim dim = { '!', {world.w, world.h} };
        pico_set_scene_dim(&dim);
        pico_output_clear();
        pico_set_pencil_color(PICO_COLOR_WHITE);
        pico_output_draw_rect (
            (Pico_Rel_Rect){ '!', {world.w/2-5, world.h/2-5, 10, 10}, PICO_ANCHOR_NW }
        );
        pico_set_pencil_color(PICO_COLOR_RED);
        pico_output_draw_line (
            (Pico_Rel_Pos){ '%', {0.5, 0.5}, PICO_ANCHOR_C },
            (Pico_Rel_Pos){ '%', {1.0, 0}, PICO_ANCHOR_C }
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
        pico_set_scene_src((Pico_Rel_Rect){ '!', {i, i, 100, 100}, PICO_ANCHOR_NW });
        pico_output_clear();
        pico_output_draw_text("Uma frase bem grande...",
            (Pico_Rel_Rect){ '!', {10, 50, 0, 10}, PICO_ANCHOR_NW });
        pico_input_delay(10);
        //pico_input_event(NULL, PICO_EVENT_KEY_DN);
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
