// REPRO : re-created scene shows textures from old (freed) layers
//
// _pico_tex_create never clears the fresh SDL_TEXTUREACCESS_TARGET
// texture and SDL documents its contents as undefined.
// After pico_layer_free destroys the old scene's textures, accelerated
// drivers recycle that GPU memory into the textures of the re-created
// scene, so new layers show the old layers' pixels.
//
// NOTE : with PICO_TESTS=1 (software renderer) SDL happens to
// zero-fill texture surfaces, so this test may PASS under
// `make test` / xvfb and still fail on real hardware.
// Run it with the accelerated renderer to reproduce:
//     ./pico-sdl tst/free-recycle.c
//
// The test passes deterministically once _pico_tex_create clears the
// new texture to transparent.

#include <assert.h>
#include "pico.h"
#include "../check.h"

int main (void) {
    pico_open(NULL);

    // reference: a layer wiped by the scene.clear cascade
    // (SDL_RenderClear with the default transparent effect.color)
    Pico_Layer* ref = pico_layer_empty(pico_layer_world(), 1, (Pico_Rel_Dim){'!', {64, 36}}, NULL);
    pico_output_clear();
    pico_output_screenshot(ref, "out/free-recycle-ref.png", NULL);

    // scene 1: layer with content, created and freed
    Pico_Layer* old = pico_layer_empty(pico_layer_world(), 0, (Pico_Rel_Dim){'!', {64, 36}}, NULL);
    pico_set_layer(old);
    pico_set_effect_color((Pico_Color){0xFF, 0x00, 0x00, 0xFF});
    pico_output_clear();
    pico_set_layer(pico_layer_world());
    pico_layer_free(old);

    // scene 2: fresh layer with the same dims must start transparent
    Pico_Layer* new = pico_layer_empty(pico_layer_world(), 0, (Pico_Rel_Dim){'!', {64, 36}}, NULL);
    pico_output_screenshot(new, "out/free-recycle-01.png", NULL);

    assert(_pico_cmp_files("out/free-recycle-ref.png", "out/free-recycle-01.png") && "fresh layer must start transparent");

    pico_layer_free(new);
    pico_layer_free(ref);
    pico_close();
    return 0;
}
