#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_window("View Target", -1, NULL);

    // 01: target = bottom-right quadrant, draw centered rect
    {
        puts("target: bottom-right quadrant");
        pico_set_view(-1, NULL, NULL,
            &(Pico_Rel_Rect){'%', {1, 1, 0.5, 0.5}, PICO_ANCHOR_SE, NULL},
            NULL, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect(
            &(Pico_Rel_Rect){'%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, NULL}
        );
        _pico_check("view-target-01");
    }

    // 02: same target, r2 with up=r1
    {
        puts("target: up chain");
        pico_output_clear();
        Pico_Rel_Rect r1 = {'%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, NULL};
        pico_output_draw_rect(&r1);
        Pico_Rel_Rect r2 = {'%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, &r1};
        pico_set_color_draw((Pico_Color){0xFF, 0x00, 0x00, 0xFF});
        pico_output_draw_rect(&r2);
        _pico_check("view-target-02");
    }

    // 03: reset target, draw same — should fill full window
    {
        puts("target: reset to full window");
        pico_set_view(-1, NULL, NULL,
            &(Pico_Rel_Rect){'%', {0.5, 0.5, 1, 1}, PICO_ANCHOR_C, NULL},
            NULL, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_set_color_draw(PICO_COLOR_WHITE);
        pico_output_draw_rect(
            &(Pico_Rel_Rect){'%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, NULL}
        );
        _pico_check("view-target-03");
    }

    // 04: explicit layer with view target, drawn with rect=NULL
    {
        puts("target: explicit layer");
        pico_layer_empty(NULL, "bg", (Pico_Abs_Dim){32, 32}, NULL);
        pico_set_layer("bg");
        pico_set_view(-1, NULL, NULL,
            &(Pico_Rel_Rect){'%', {1, 1, 0.5, 0.5}, PICO_ANCHOR_SE, NULL},
            NULL, NULL, NULL, NULL, NULL);
        pico_set_color_clear((Pico_Color){0x80, 0x00, 0x00, 0xFF});
        pico_output_clear();
        pico_set_color_draw(PICO_COLOR_WHITE);
        pico_output_draw_rect(
            &(Pico_Rel_Rect){'%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, NULL}
        );
        pico_set_layer(NULL);
        pico_set_color_clear((Pico_Color){0x00, 0x00, 0x00, 0xFF});
        pico_output_clear();
        pico_output_draw_layer("bg", NULL);
        _pico_check("view-target-04");
    }

    pico_init(0);
    return 0;
}
