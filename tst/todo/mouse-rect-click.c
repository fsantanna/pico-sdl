#include "pico.h"

int main (void) {
    pico_init(1);

    /* Layer 120x50 with 3 centered buttons equally spaced */
    Pico_Rel_Rect btn1 = { '%', {0.25, 0.50, 0.15, 0.30}, PICO_ANCHOR_C, NULL };
    Pico_Rel_Rect btn2 = { '%', {0.50, 0.50, 0.15, 0.30}, PICO_ANCHOR_C, NULL };
    Pico_Rel_Rect btn3 = { '%', {0.75, 0.50, 0.15, 0.30}, PICO_ANCHOR_C, NULL };

    pico_layer_empty(NULL, "A", (Pico_Abs_Dim){120, 50}, NULL);
    pico_set_layer("A");
    pico_set_color_clear(PICO_COLOR_NAVY);
    pico_output_clear();
    pico_set_draw_color(PICO_COLOR_WHITE);
    pico_output_draw_rect(&btn1);
    pico_output_draw_rect(&btn2);
    pico_output_draw_rect(&btn3);
    pico_set_layer(NULL);

    /* Draw layer at bottom-right, 35%x35% of screen (distorted) */
    Pico_Rel_Rect r = { '%', {0.99, 0.99, 0.35, 0.35}, PICO_ANCHOR_SE, NULL };
    pico_set_color_clear(PICO_COLOR_BLACK);
    pico_output_clear();
    pico_output_draw_layer("A", &r);

    /* Event loop */
    Pico_Event evt;
    while (pico_input_event_timeout(&evt, PICO_EVENT_MOUSE_BUTTON_DN, -1)) {
        if (evt.type == PICO_EVENT_QUIT) {
            break;
        }

        Pico_Mouse win = pico_get_mouse('w', NULL);
        Pico_Mouse pct = pico_get_mouse('%', &r);
        printf(">>> w %4.0f %4.0f | %% %5.3f %5.3f", win.x, win.y, pct.x, pct.y);

        Pico_Rel_Pos pos = { '%', {pct.x, pct.y}, PICO_ANCHOR_NW, NULL };
        if (pico_vs_pos_rect(&pos, &btn1)) {
            printf(" | click 1");
        } else if (pico_vs_pos_rect(&pos, &btn2)) {
            printf(" | click 2");
        } else if (pico_vs_pos_rect(&pos, &btn3)) {
            printf(" | click 3");
        }
        printf("\n");
    }

    pico_init(0);
    return 0;
}
