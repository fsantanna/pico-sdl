#include "pico.h"
#include "../check.h"

/* Aux-clip regression: layer dst fully inside vs. over-extending world.
 *
 * Without aux-clipping in _pico_output_draw_layer, an over-extending
 * dst gets clipped by SDL but src is not adjusted - the full image
 * squashes into the clipped dst instead of windowing the visible
 * center.
 */
int main (void) {
    pico_init(1);
    pico_set_window_title("Layer clip");
    pico_set_effect_color(PICO_COLOR_WHITE);
    pico_output_clear();

    pico_layer_image("world", "img", "../res/open.png");

    // case 1: dst fully inside world (aux no-op)
    {
        const char* old = pico_set_layer("img");
        pico_set_scene_dst((Pico_Rel_Rect){'%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C});
        pico_set_layer(old);
        pico_output_clear();
        pico_output_present();
        _pico_check("layer-clip-01");
    }

    // case 2: dst 4-side overflow (aux must window the center)
    {
        const char* old = pico_set_layer("img");
        pico_set_scene_dst((Pico_Rel_Rect){'%', {0.5, 0.5, 2.0, 2.0}, PICO_ANCHOR_C});
        pico_set_layer(old);
        pico_output_clear();
        pico_output_present();
        _pico_check("layer-clip-02");
    }

    // case 3: same layer thrice (50x50) - centered + half off-left + half off-right
    {
        const char* old = pico_set_layer("img");
        pico_set_scene_dst((Pico_Rel_Rect){'%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C});
        pico_set_layer(old);
        pico_output_clear();
        pico_output_draw_layer("img",
            &(Pico_Rel_Rect){'%', {0, 0.5, 0.5, 0.5}, PICO_ANCHOR_C});
        pico_output_draw_layer("img",
            &(Pico_Rel_Rect){'%', {1.0, 0.5, 0.5, 0.5}, PICO_ANCHOR_C});
        pico_output_present();
        _pico_check("layer-clip-03");
    }

    // case 4: src overflow - scene.src past layer.dim (aux clips src)
    {
        const char* old = pico_set_layer("img");
        pico_set_scene_dst((Pico_Rel_Rect){'%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C});
        pico_set_scene_src((Pico_Rel_Rect){'%', {0.5, 0.5, 2.0, 2.0}, PICO_ANCHOR_C});
        pico_set_layer(old);
        pico_output_clear();
        pico_output_present();
        _pico_check("layer-clip-04");
    }

    // case 5: world scene.src y-overflow (mimics navigate-02 after 3 Ctrl+UP)
    // aux: dst shifts down, top empty -> window bg (gray) shows
    // no aux: SDL edge-clamps -> top filled with world.tex top row (white)
    {
        pico_output_clear();
        pico_set_pencil_color(PICO_COLOR_BLACK);
        pico_output_draw_rect(
            &(Pico_Rel_Rect){'%', {0.5, 0.5, 0.25, 0.25}, PICO_ANCHOR_C});
        pico_set_scene_src(
            (Pico_Rel_Rect){'%', {0.5, 0.2, 1.0, 1.0}, PICO_ANCHOR_C});
        pico_output_present();
        _pico_check("layer-clip-05");
    }

    // case 6: child layer src y-overflow (Phase A aux on layer->world blit)
    // L: green bg, black marker. dst fills world. src.y negative.
    // aux: dst shifts down, top of dst empty -> world.tex (white) shows
    // no aux: SDL edge-clamps L.tex top row -> green at top of world.tex
    {
        pico_layer_empty("world", "L", (Pico_Abs_Dim){48, 48}, NULL);
        {
            const char* old = pico_set_layer("L");
            pico_set_effect_color(PICO_COLOR_GREEN);
            pico_output_clear();
            pico_set_pencil_color(PICO_COLOR_BLACK);
            pico_output_draw_rect(
                &(Pico_Rel_Rect){'%', {0.5, 0.5, 0.4, 0.4}, PICO_ANCHOR_C});
            pico_set_scene_dst(
                (Pico_Rel_Rect){'%', {0.5, 0.5, 1.0, 1.0}, PICO_ANCHOR_C});
            pico_set_scene_src(
                (Pico_Rel_Rect){'%', {0.5, 0.2, 1.0, 1.0}, PICO_ANCHOR_C});
            pico_set_layer(old);
        }
        pico_output_clear();
        // reset world.scene.src so case 6 tests Phase A aux only
        pico_set_scene_src(
            (Pico_Rel_Rect){'%', {0.5, 0.5, 1.0, 1.0}, PICO_ANCHOR_C});
        pico_output_present();
        _pico_check("layer-clip-06");
    }

    pico_init(0);
    return 0;
}
