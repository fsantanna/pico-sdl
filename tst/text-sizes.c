#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_window_title("Text Sizes");
    pico_set_layer("window");
    pico_set_scene_dim((Pico_Rel_Dim){ '!', {500, 500} });
    pico_set_layer("world");
    pico_set_scene_dim((Pico_Rel_Dim){ '!', {500, 500} });

    pico_output_clear();
    pico_set_pencil_font("../res/DejaVuSans.ttf");
    pico_set_pencil_color((Pico_Color){255, 255, 255, 0xFF});

    // 12 lines, small to big, stacked top-to-bottom
    {
        int y = 5;
        for (int i = 1; i <= 12; i++) {
            int h = 5 * i;
            Pico_Rel_Rect r = { '!', {5, y, 0, h}, PICO_ANCHOR_NW };
            pico_output_draw_text("Hello World!", r);
            y += h + 4;
        }
        _pico_check("text-sizes-01");
    }

    // repro of intro.atm flicker: small text, W anchor, redrawn
    // (cleared) each frame as the prefix grows -> left edge shimmers
    // ~1px because auto-width re-quantises per char.
    // r1 '!' vs r2 '%' at the same pixel height (12 ~= 0.025*500):
    // isolates whether the flicker is mode-specific or size-driven.
    {
        const char* msg = "The quick brown fox jumps over the lazy dog.";
        Pico_Rel_Rect r1 = { '!', {75, 200, 0, 12}, PICO_ANCHOR_W };
        Pico_Rel_Rect r2 = { '%', {0.15, 0.5, 0, 0.025}, PICO_ANCHOR_W };
        char buf[128];
        int n = strlen(msg);
        for (int i = 1; i <= n; i++) {
            memcpy(buf, msg, i);
            buf[i] = '\0';
            pico_output_clear();
            pico_output_draw_text(buf, r1);
            pico_output_draw_text(buf, r2);
            pico_input_delay(50);
        }
        _pico_check("text-sizes-02");
    }

    pico_init(0);
    return 0;
}
