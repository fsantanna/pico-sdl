#include "pico.h"
#include "../check.h"

// SDL2_gfx `_ellipseRGBA` sets the blend mode from its alpha argument
// (NONE when a==255) and never restores it.
// After an opaque oval, later semi-transparent draws must still blend.
// Expected: both halves show a translucent white overlay over red.
int main (void) {
    pico_init(1);
    pico_set_window((Pico_Window){ .fs=0, .show=1, .title="gfx-alpha" });
    pico_set_layer("window");
    pico_set_scene_dim((Pico_Rel_Dim){ '!', {640, 360} });
    pico_set_layer("world");
    pico_set_scene_dim((Pico_Rel_Dim){ '!', {64, 36} });
    pico_set_pencil_style(PICO_STYLE_FILL);
    pico_output_clear();

    // red background rects on both halves
    pico_set_pencil_color((Pico_Color){255,0,0,255});
    pico_output_draw_rect((Pico_Rel_Rect){ '!', { 0, 0,32,36}, PICO_ANCHOR_NW });
    pico_output_draw_rect((Pico_Rel_Rect){ '!', {32, 0,32,36}, PICO_ANCHOR_NW });

    // left: translucent overlay right after a rect
    pico_set_pencil_color((Pico_Color){255,255,255,128});
    pico_output_draw_rect((Pico_Rel_Rect){ '!', { 4, 4,24,28}, PICO_ANCHOR_NW });

    // right: an opaque oval in between, then the same overlay
    pico_set_pencil_color((Pico_Color){0,0,255,255});
    pico_output_draw_oval((Pico_Rel_Rect){ '!', {40,20,16,12}, PICO_ANCHOR_NW });
    pico_set_pencil_color((Pico_Color){255,255,255,128});
    pico_output_draw_rect((Pico_Rel_Rect){ '!', {36, 4,24,28}, PICO_ANCHOR_NW });

    _pico_check("bug-gfx-alpha-01");

    pico_init(0);
    return 0;
}
