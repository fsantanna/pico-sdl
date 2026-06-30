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

    // repro of intro.atm flicker: typewriter, redrawn (cleared) each
    // frame as the prefix grows. r1 '!' and r2 '%' at the SAME integer
    // height (22px = 0.044*500). 22 still jitters: ptsize lands the
    // glyph surface at H0 != 22, so box_h != H0 and the auto-width
    // re-quantises per char (20 happens to match H0 -> stays stable).
    // Stacked so the boxes share a border (r1 bottom = r2 top = y211).
    {
        const char* msg = "The quick brown fox jumps over the lazy dog.";
        Pico_Rel_Rect r1 = { '!', {20, 200, 0, 22}, PICO_ANCHOR_W };
        Pico_Rel_Rect r2 = { '%', {0.04, 0.444, 0, 0.044}, PICO_ANCHOR_W };
        char buf[128];
        int n = strlen(msg);
        for (int i = 1; i <= n; i++) {
            memcpy(buf, msg, i);
            buf[i] = '\0';
            pico_output_clear();

            // texts (white)
            pico_set_pencil_color((Pico_Color){255, 255, 255, 0xFF});
            pico_output_draw_text(buf, r1);
            pico_output_draw_text(buf, r2);

            // enclosing boxes (red stroke), sized to the measured text
            pico_set_pencil_color((Pico_Color){255, 0, 0, 0xFF});
            pico_set_pencil_style(PICO_STYLE_STROKE);
            Pico_Rel_Dim m1 = { '!', {0, 22} };
            pico_get_text(&m1, buf);
            pico_output_draw_rect((Pico_Rel_Rect){ '!', {20, 200, m1.w, 22}, PICO_ANCHOR_W });
            Pico_Rel_Dim m2 = { '%', {0, 0.044} };
            pico_get_text(&m2, buf);
            pico_output_draw_rect((Pico_Rel_Rect){ '%', {0.04, 0.444, m2.w, 0.044}, PICO_ANCHOR_W });
            pico_set_pencil_style(PICO_STYLE_FILL);

            pico_input_delay(50);
        }
        _pico_check("text-sizes-02");
    }

    pico_init(0);
    return 0;
}
