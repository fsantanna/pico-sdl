#include "pico.h"

int main (void) {
    pico_init(1);

    /* Create layer 50x50, draw centered circle */
    pico_layer_empty(NULL, "A", (Pico_Abs_Dim){50, 50}, NULL);
    pico_set_layer("A");
    pico_set_show_color(NULL, PICO_COLOR_RED);
    pico_output_clear();
    pico_output_draw_oval(&(Pico_Rel_Rect){ '%', {0.3, 0.6, 0.5, 0.5}, PICO_ANCHOR_C, NULL });
    pico_set_layer(NULL);

    /* Draw layer distorted 2:1 on main */
    Pico_Rel_Rect r = { '%', {0.6, 0.3, 0.8, 0.4}, PICO_ANCHOR_C, NULL };
    pico_set_show_color(NULL, PICO_COLOR_BLACK);
    pico_output_clear();
    pico_output_draw_layer("A", &r);

    /* Event loop */
    Pico_Event evt;
    while (pico_input_event_timeout(&evt, PICO_EVENT_MOUSE_BUTTON_DN, -1)) {
        if (evt.type == PICO_EVENT_QUIT) {
            break;
        }
        Pico_Mouse win = pico_get_mouse('w', NULL);
        Pico_Mouse raw = pico_get_mouse('!', &r);
        Pico_Mouse pct = pico_get_mouse('%', &r);
        printf(
            ">>> w %4.0f %4.0f | ! %6.1f %6.1f | %% %5.3f %5.3f\n",
            win.x, win.y, raw.x, raw.y, pct.x, pct.y
        );
    }

    pico_init(0);
    return 0;
}
