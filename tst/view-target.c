#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_window_title("View Target");

    // 01: target = bottom-right quadrant, draw centered rect
    {
        puts("target: bottom-right quadrant");
        pico_set_scene_dst((Pico_Rel_Rect){'%', {1, 1, 0.5, 0.5}, PICO_ANCHOR_SE});
        pico_output_clear();
        pico_output_draw_rect(
            (Pico_Rel_Rect){'%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C}
        );
        _pico_check("view-target-01");
    }

    // 02: same target, r2 nested inside r1
    {
        puts("target: r2 inside r1");
        pico_output_clear();
        Pico_Rel_Rect r1 = {'%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C};
        pico_output_draw_rect(r1);
        Pico_Rel_Rect r2 = pico_in_rect(r1, (Pico_Rel_Rect){'%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C});
        pico_set_pencil_color((Pico_Color){0xFF, 0x00, 0x00, 0xFF});
        pico_output_draw_rect(r2);
        _pico_check("view-target-02");
    }

    // 03: reset target, draw same — should fill full window
    {
        puts("target: reset to full window");
        pico_set_scene_dst((Pico_Rel_Rect){'%', {0.5, 0.5, 1, 1}, PICO_ANCHOR_C});
        pico_output_clear();
        pico_set_pencil_color(PICO_COLOR_WHITE);
        pico_output_draw_rect(
            (Pico_Rel_Rect){'%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C}
        );
        _pico_check("view-target-03");
    }

    // 04: explicit layer with view target, drawn with rect=NULL
    {
        puts("target: explicit layer");
        pico_layer_empty(NULL, "bg", 1, (Pico_Rel_Dim){'!', {32, 32}}, NULL);
        const char* old = pico_set_layer("bg");
        pico_set_scene_dst((Pico_Rel_Rect){'%', {1, 1, 0.5, 0.5}, PICO_ANCHOR_SE});
        pico_set_effect_color((Pico_Color){0x80, 0x00, 0x00, 0xFF});
        pico_output_clear();
        pico_set_pencil_color(PICO_COLOR_WHITE);
        pico_output_draw_rect(
            (Pico_Rel_Rect){'%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C}
        );
        pico_set_layer(old);
        pico_set_effect_color((Pico_Color){0x00, 0x00, 0x00, 0xFF});
        pico_output_clear();
        pico_output_draw_layer("bg", NULL);
        _pico_check("view-target-04");
    }

    // 05: target h only, w=0 -> w inferred from 2:1 layer aspect
    {
        puts("target: h only, w inferred");
        pico_layer_empty("world", "bg2", 1, (Pico_Rel_Dim){'!', {80, 40}}, NULL);
        const char* old = pico_set_layer("bg2");
        pico_set_scene_dst(
            (Pico_Rel_Rect){'%', {0.5, 0.5, 0, 0.4}, PICO_ANCHOR_C});
        pico_set_effect_color((Pico_Color){0x80, 0x00, 0x00, 0xFF});
        pico_output_clear();
        pico_set_layer(old);
        pico_set_effect_color((Pico_Color){0x00, 0x00, 0x00, 0xFF});
        pico_output_clear();
        pico_output_draw_layer("bg2", NULL);
        _pico_check("view-target-05");
    }

    // 06: target w only, h=0 -> h inferred from 2:1 layer aspect
    {
        puts("target: w only, h inferred");
        const char* old = pico_set_layer("bg2");
        pico_set_scene_dst(
            (Pico_Rel_Rect){'%', {0.5, 0.5, 0.4, 0}, PICO_ANCHOR_C});
        pico_set_layer(old);
        pico_output_clear();
        pico_output_draw_layer("bg2", NULL);
        _pico_check("view-target-06");
    }

    // 07: target w=h=0 -> full layer dim
    {
        puts("target: w=h=0, full layer dim");
        const char* old = pico_set_layer("bg2");
        pico_set_scene_dst(
            (Pico_Rel_Rect){'%', {0, 0, 0, 0}, PICO_ANCHOR_NW});
        pico_set_layer(old);
        pico_output_clear();
        pico_output_draw_layer("bg2", NULL);
        _pico_check("view-target-07");
    }

    pico_init(0);
    return 0;
}
