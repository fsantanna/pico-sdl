#include "pico.h"

int main() {
    pico_init(1);
    pico_set_view_raw(-1, NULL, NULL, &(Pico_Dim){1000, 1000}, NULL, NULL);

    while (1) {
        // Draw X at center of world
        pico_output_draw_line_pct(
            &(Pico_Pos_Pct){0.45, 0.45, PICO_ANCHOR_C, NULL},
            &(Pico_Pos_Pct){0.55, 0.55, PICO_ANCHOR_C, NULL}
        );
        pico_output_draw_line_pct(
            &(Pico_Pos_Pct){0.55, 0.45, PICO_ANCHOR_C, NULL},
            &(Pico_Pos_Pct){0.45, 0.55, PICO_ANCHOR_C, NULL}
        );

        // Draw rectangle at center of world (1/4 size)
        pico_set_style(PICO_STROKE);
        pico_output_draw_rect_pct(
            &(Pico_Rect_Pct){0.5, 0.5, 0.25, 0.25, PICO_ANCHOR_C, NULL}
        );

        pico_input_event(NULL, PICO_ANY);
    }

    pico_init(0);
    return 0;
}
