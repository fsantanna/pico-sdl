#include <assert.h>
#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_window((Pico_Window){ .fs=0, .show=1, .title="Clear Alpha" });

    pico_set_layer("window");
    pico_set_scene_dim((Pico_Rel_Dim){ '!', {640, 360} });
    pico_set_layer("world");
    pico_set_scene_dim((Pico_Rel_Dim){'!', {64, 36}});

    // red background on main
    pico_set_effect_color((Pico_Color){0xFF, 0x00, 0x00, 0xFF});
    pico_output_clear();

    // overlay layer: blue clear (semi-transparent), small white rect
    pico_layer_empty(NULL, "overlay", 0, (Pico_Rel_Dim){'!', {64, 36}}, NULL);
    pico_set_layer("overlay");
    pico_set_effect_color((Pico_Color){0x00, 0x00, 0xFF, 0x80});
    pico_output_clear();
    pico_set_pencil_color((Pico_Color){0xFF, 0xFF, 0xFF, 0xFF});
    pico_output_draw_rect((Pico_Rel_Rect){'%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C});

    // composite overlay on main
    pico_set_layer("world");
    pico_output_draw_layer("overlay", &(Pico_Rel_Rect){'%', {0.5, 0.5, 0.75, 0.75}, PICO_ANCHOR_C});
    _pico_check("clear_alpha-01");

    // default effect.color is transparent: red shows through
    pico_output_clear();
    pico_layer_empty("world", "defclr", 1, (Pico_Rel_Dim){'!', {64, 36}}, NULL);
    pico_output_clear();
    pico_output_present(1);
    _pico_check("clear_alpha-02");

    // a transparent clear must WIPE the layer (not blend a no-op): paint
    // a detached layer red, clear it back to transparent, then composite
    // over a plain green world -- the world must stay plain green
    pico_set_layer("world");
    pico_set_effect_color((Pico_Color){0x00, 0xFF, 0x00, 0xFF});
    pico_output_clear();
    pico_output_screenshot("world", "out/clear_alpha-transp-ref.png", NULL);

    pico_layer_empty(NULL, "wipe", 0, (Pico_Rel_Dim){'!', {64, 36}}, NULL);
    pico_set_layer("wipe");
    pico_set_effect_color((Pico_Color){0xFF, 0x00, 0x00, 0xFF});
    pico_output_clear();
    pico_set_effect_color(PICO_COLOR_TRANSPARENT);
    pico_output_clear();

    pico_set_layer("world");
    pico_output_draw_layer("wipe", NULL);
    pico_output_screenshot("world", "out/clear_alpha-transp-01.png", NULL);
    assert(_pico_cmp_files("out/clear_alpha-transp-ref.png", "out/clear_alpha-transp-01.png") && "transparent clear must wipe the layer");

    pico_init(0);
    return 0;
}
