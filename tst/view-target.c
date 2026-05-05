#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_window_title("View Target");

    // 01: target = bottom-right quadrant, draw centered rect
    {
        puts("target: bottom-right quadrant");
        pico_set_scene_dst("root", (Pico_Rel_Rect){'%', {1, 1, 0.5, 0.5}, PICO_ANCHOR_SE, NULL});
        pico_output_clear("root");
        pico_output_draw_rect("root", 
            &(Pico_Rel_Rect){'%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, NULL}
        );
        _pico_check("view-target-01");
    }

    // 02: same target, r2 with up=r1
    {
        puts("target: up chain");
        pico_output_clear("root");
        Pico_Rel_Rect r1 = {'%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, NULL};
        pico_output_draw_rect("root", &r1);
        Pico_Rel_Rect r2 = {'%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, &r1};
        pico_set_pencil_color("root", (Pico_Color){0xFF, 0x00, 0x00, 0xFF});
        pico_output_draw_rect("root", &r2);
        _pico_check("view-target-02");
    }

    // 03: reset target, draw same — should fill full window
    {
        puts("target: reset to full window");
        pico_set_scene_dst("root", (Pico_Rel_Rect){'%', {0.5, 0.5, 1, 1}, PICO_ANCHOR_C, NULL});
        pico_output_clear("root");
        pico_set_pencil_color("root", PICO_COLOR_WHITE);
        pico_output_draw_rect("root", 
            &(Pico_Rel_Rect){'%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, NULL}
        );
        _pico_check("view-target-03");
    }

    // 04: explicit layer with view target, drawn with rect=NULL
    {
        puts("target: explicit layer");
        pico_layer_empty(NULL, "bg", (Pico_Abs_Dim){32, 32}, NULL);
        pico_set_scene_dst("bg", (Pico_Rel_Rect){'%', {1, 1, 0.5, 0.5}, PICO_ANCHOR_SE, NULL});
        pico_set_effect_color("bg", (Pico_Color){0x80, 0x00, 0x00, 0xFF});
        pico_output_clear("bg");
        pico_set_pencil_color("bg", PICO_COLOR_WHITE);
        pico_output_draw_rect("bg",
            &(Pico_Rel_Rect){'%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, NULL}
        );
        pico_set_effect_color("root", (Pico_Color){0x00, 0x00, 0x00, 0xFF});
        pico_output_clear("root");
        pico_output_draw_layer("root", "bg", NULL);
        _pico_check("view-target-04");
    }

    // 05: target h only, w=0 -> w inferred from 2:1 layer aspect
    {
        puts("target: h only, w inferred");
        pico_layer_empty(NULL, "bg2", (Pico_Abs_Dim){80, 40}, NULL);
        pico_set_scene_dst("bg2",
            (Pico_Rel_Rect){'%', {0.5, 0.5, 0, 0.4}, PICO_ANCHOR_C, NULL});
        pico_set_effect_color("bg2", (Pico_Color){0x80, 0x00, 0x00, 0xFF});
        pico_output_clear("bg2");
        pico_set_effect_color("root", (Pico_Color){0x00, 0x00, 0x00, 0xFF});
        pico_output_clear("root");
        pico_output_draw_layer("root", "bg2", NULL);
        _pico_check("view-target-05");
    }

    // 06: target w only, h=0 -> h inferred from 2:1 layer aspect
    {
        puts("target: w only, h inferred");
        pico_set_scene_dst("bg2",
            (Pico_Rel_Rect){'%', {0.5, 0.5, 0.4, 0}, PICO_ANCHOR_C, NULL});
        pico_output_clear("root");
        pico_output_draw_layer("root", "bg2", NULL);
        _pico_check("view-target-06");
    }

    // 07: target w=h=0 -> full layer dim
    {
        puts("target: w=h=0, full layer dim");
        pico_set_scene_dst("bg2",
            (Pico_Rel_Rect){'%', {0, 0, 0, 0}, PICO_ANCHOR_NW, NULL});
        pico_output_clear("root");
        pico_output_draw_layer("root", "bg2", NULL);
        _pico_check("view-target-07");
    }

    pico_init(0);
    return 0;
}
