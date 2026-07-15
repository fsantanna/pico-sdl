// FAILING TEST : transparent clear must wipe the layer
//
// pico_output_clear fills the current layer with effect.color via
// SDL_RenderFillRect, but the renderer draw blend mode is
// SDL_BLENDMODE_BLEND (set once in pico_init), so a fill with the
// default PICO_COLOR_TRANSPARENT is a no-op and the old content
// survives.
// The other two clear paths (_pico_output_clear_pre cascade and
// pico_set_effect_color repaint) use SDL_RenderClear, which ignores
// the blend mode and overwrites — the direct path is inconsistent
// with them.
//
// The test passes once pico_output_clear overwrites instead of
// blending (e.g. switching to SDL_BLENDMODE_NONE around the fill).

#include <assert.h>
#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);

    // reference: plain green world
    pico_set_effect_color((Pico_Color){0x00, 0xFF, 0x00, 0xFF});
    pico_output_clear();
    pico_output_screenshot("world", "out/clear-transp-ref.png", NULL);

    // detached layer painted solid red
    pico_layer_empty(NULL, "a", 0, (Pico_Rel_Dim){'!', {64, 36}}, NULL);
    pico_set_layer("a");
    pico_set_effect_color((Pico_Color){0xFF, 0x00, 0x00, 0xFF});
    pico_output_clear();

    // wipe back to the default transparent background
    pico_set_effect_color(PICO_COLOR_TRANSPARENT);
    pico_output_clear();

    // composite over the green world: world must remain plain green
    pico_set_layer("world");
    pico_output_draw_layer("a", NULL);
    pico_output_screenshot("world", "out/clear-transp-01.png", NULL);

    assert(_pico_cmp_files("out/clear-transp-ref.png", "out/clear-transp-01.png") && "transparent clear must wipe the layer");

    pico_init(0);
    return 0;
}
