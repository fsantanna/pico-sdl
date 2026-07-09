// A clip set on the WINDOW (parent) layer must scissor the world -> window
// composite, not just direct draws. Currently the composite ignores it, so
// the world overwrites the whole window (full red instead of a red box).
//
// Two blocks pin the same behaviour in both modes:
//   01 PLAIN  -- green comes from the window effect (clear) colour, since a
//                plain present clears the window every frame.
//   02 EXPERT -- green is drawn onto the window layer and persists, since an
//                expert present does not clear the window.
//
// Build/run:  make T=window-clip gen   (inspect tst/out/window-clip-*.png)

#include "pico.h"
#include "../check.h"

int main (void) {
    Pico_Rel_Rect full = { '%', {0.5,0.5, 1.0,1.0}, PICO_ANCHOR_C };
    Pico_Rel_Rect box  = { '%', {0.5,0.5, 0.5,0.5}, PICO_ANCHOR_C };
    Pico_Color green = { 0x00,0xFF,0x00,0xFF };
    Pico_Color red   = { 0xFF,0x00,0x00,0xFF };

    pico_init(1);
    pico_set_window_title("Window Clip");
    pico_set_pencil_style(PICO_STYLE_FILL);

    ///////////////////////////////////////////////////////////////////////////
    // PLAIN: green backdrop via the window effect (clear) colour
    ///////////////////////////////////////////////////////////////////////////

    // GREEN backdrop: the plain present clears the window to this each frame
    pico_set_layer("window");
    pico_set_effect_color(green);

    // clip the WINDOW to a centred box; the composite should honour it
    pico_set_scene_clip(box);

    // WORLD: full red -> should reach the window only inside the box
    pico_set_layer("world");
    pico_set_pencil_color(red);
    pico_output_draw_rect(full);

    _pico_check("window-clip-01");
    // want: GREEN field, RED centred box
    // now:  full RED (window clip ignored by the composite)

    ///////////////////////////////////////////////////////////////////////////
    // EXPERT: green backdrop drawn onto the window layer (persists)
    ///////////////////////////////////////////////////////////////////////////

    pico_set_expert(1, 0);

    // GREEN background painted on the WINDOW layer (full)
    pico_set_layer("window");
    pico_set_scene_clip(full);
    pico_set_pencil_color(green);
    pico_output_draw_rect(full);

    // clip the WINDOW to a centred box; the composite should honour it
    pico_set_scene_clip(box);

    // WORLD: full red -> should reach the window only inside the box
    pico_set_layer("world");
    pico_output_clear();
    pico_set_pencil_color(red);
    pico_output_draw_rect(full);

    pico_output_present(1);
    _pico_check("window-clip-02");
    // want: GREEN field, RED centred box
    // now:  full RED (window clip ignored by the composite)

    pico_init(0);
    return 0;
}
