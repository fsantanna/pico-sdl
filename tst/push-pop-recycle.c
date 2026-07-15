// REPRO : re-created scene shows textures from old (popped) layers
//
// _pico_tex_create never clears the fresh SDL_TEXTUREACCESS_TARGET
// texture and SDL documents its contents as undefined.
// After pico_pop destroys the old scene's textures, accelerated
// drivers recycle that GPU memory into the textures of the re-created
// scene, so new layers show the old layers' pixels.
//
// NOTE : with PICO_TESTS=1 (software renderer) SDL happens to
// zero-fill texture surfaces, so this test may PASS under
// `make test` / xvfb and still fail on real hardware.
// Run it with the accelerated renderer to reproduce:
//     ./pico-sdl tst/push-pop-recycle.c
//
// The test passes deterministically once _pico_tex_create clears the
// new texture to transparent.

#include <assert.h>
#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);

    // reference: a layer wiped by the scene.clear cascade
    // (SDL_RenderClear with the default transparent effect.color)
    pico_layer_empty("world", "ref", 1, (Pico_Rel_Dim){'!', {64, 36}}, NULL);
    pico_output_clear();
    pico_output_screenshot("ref", "out/push-pop-recycle-ref.png", NULL);

    // scene 1: layer with content, inside push/pop
    pico_push();
    pico_layer_empty("world", "old", 0, (Pico_Rel_Dim){'!', {64, 36}}, NULL);
    pico_set_layer("old");
    pico_set_effect_color((Pico_Color){0xFF, 0x00, 0x00, 0xFF});
    pico_output_clear();
    pico_set_layer("world");
    pico_pop();

    // scene 2: fresh layer with the same dims must start transparent
    pico_layer_empty("world", "new", 0, (Pico_Rel_Dim){'!', {64, 36}}, NULL);
    pico_output_screenshot("new", "out/push-pop-recycle-01.png", NULL);

    assert(_pico_cmp_files("out/push-pop-recycle-ref.png", "out/push-pop-recycle-01.png") && "fresh layer must start transparent");

    pico_init(0);
    return 0;
}
