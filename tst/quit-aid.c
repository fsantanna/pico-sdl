#include "pico.h"
#include "_pico.h"

// Auto-exit aid: a quit event (window close) calls pico_close and
// exits the process from inside the input call. pico_close releases
// every handle still alive: under valgrind this test must exit with no
// leaks, although nothing is freed here.

int main (void) {
    pico_open(NULL);

    Pico_Layer* W = pico_layer_world();
    Pico_Rel_Dim d = { '!', {8, 8} };

    // hierarchy, sub-layer, text, image, video, font, sound: all alive
    Pico_Layer* up  = pico_layer_empty(W, 1, d, NULL);
    Pico_Layer* dn  = pico_layer_empty(up, 1, d, NULL);
    Pico_Layer* sub = pico_layer_sub (
        NULL, up, &(Pico_Rel_Rect){'%', {.5, .5, .5, .5}, PICO_ANCHOR_C}
    );
    Pico_Layer* txt = pico_layer_text(NULL, (Pico_Rel_Dim){'!', {0, 10}}, "x");
    Pico_Layer* img = pico_layer_image(NULL, "../res/open.png");
    Pico_Layer* vid = pico_layer_video(NULL, "../res/video.y4m");
    Pico_Font*  fnt = pico_font_load("../res/DejaVuSans.ttf");
    Pico_Sound* snd = pico_sound_load("../res/start.wav");
    (void)dn; (void)sub; (void)txt; (void)img; (void)vid; (void)snd;
    assert(G.live == 8);

    // the current layer and a pencil font are user-owned as well
    pico_set_layer(up);
    pico_set_pencil_font(fnt);
    pico_output_draw_text("y", (Pico_Rel_Rect){'!', {4, 4, 0, 8}, PICO_ANCHOR_C});

    // easy-path resources too
    pico_set_layer(W);
    pico_output_draw_image (
        "../res/open.png", (Pico_Rel_Rect){'%', {.5, .5, 1, 1}, PICO_ANCHOR_C}
    );

    pico_quit();
    pico_input_event(NULL, PICO_EVENT_ANY);     // closes pico and exits

    assert(0 && "unreachable");
    return 1;
}
