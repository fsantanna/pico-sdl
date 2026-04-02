#include "pico.h"

int main (void) {
    pico_init(1);

    Pico_Rel_Rect r = { '%', {0.7, 0.3, 0.6, 0.4}, PICO_ANCHOR_C, NULL };

    /* Draw the rect */
    pico_set_style(PICO_STYLE_STROKE);
    pico_output_draw_rect(&r);

    /* Event loop on mouse motion */
    Pico_Event evt;
    while (pico_input_event_timeout(&evt, PICO_EVENT_MOUSE_BUTTON_DN, -1)) {
        if (evt.type == PICO_EVENT_QUIT) {
            break;
        }
        Pico_Mouse win = pico_get_mouse('w', NULL);
        Pico_Mouse raw = pico_get_mouse('!', &r);
        Pico_Mouse pct = pico_get_mouse('%', &r);
        printf (
            ">>> w %4.0f %4.0f | ! %6.1f %6.1f | %% %5.3f %5.3f\n",
            win.x, win.y, raw.x, raw.y, pct.x, pct.y
        );
    }

    pico_init(0);
    return 0;
}
