#include "pico.h"
#include "_pico.h"

// pico_close releases every handle still alive: under valgrind this
// test must end with no leaks, although nothing is freed here.

static void _scene (void) {
    Pico_Layer* W = pico_layer_world();
    Pico_Rel_Dim d = { '!', {8, 8} };

    // hierarchy, sub-layer, text, image, video, font, sound: all alive
    Pico_Layer* up  = pico_layer_empty(W, 1, d, NULL);
    Pico_Layer* dn  = pico_layer_empty(up, 1, d, NULL);
    Pico_Layer* sub = pico_layer_sub (
        dn, up, &(Pico_Rel_Rect){'%', {.5, .5, .5, .5}, PICO_ANCHOR_C}
    );
    Pico_Layer* txt = pico_layer_text(NULL, (Pico_Rel_Dim){'!', {0, 10}}, "x");
    Pico_Layer* img = pico_layer_image(NULL, "../res/open.png");
    Pico_Layer* vid = pico_layer_video(NULL, "../res/video.y4m");
    Pico_Font*  fnt = pico_font_load("../res/DejaVuSans.ttf");
    Pico_Sound* snd = pico_sound_load("../res/start.wav");
    (void)sub; (void)txt; (void)img; (void)vid; (void)snd;
    assert(G.live == 8);

    // some freed by hand, the rest left to pico_close
    pico_layer_free(txt);
    assert(G.live == 7);

    // a user-owned layer is current, with a user-owned pencil font
    pico_set_layer(up);
    pico_set_pencil_font(fnt);
    pico_output_draw_text("y", (Pico_Rel_Rect){'!', {4, 4, 0, 8}, PICO_ANCHOR_C});
}

int main (void) {
    pico_open(NULL);
    _scene();
    pico_close();

    // a fresh open starts with nothing alive
    pico_open(NULL);
    assert(G.live==0 && G.owned==NULL);
    assert(pico_get_layer() == pico_layer_world());
    _scene();
    pico_close();

    return 0;
}
