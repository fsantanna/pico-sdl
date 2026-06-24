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
        float y = 0.01f;
        for (int i = 1; i <= 12; i++) {
            float h = 0.01f * i;
            Pico_Rel_Rect r = { '%', {0.01f, y, 0, h}, PICO_ANCHOR_NW };
            pico_output_draw_text("Hello World!", r);
            y += h + 0.008f;
        }
        _pico_check("text-sizes-01");
    }

    pico_init(0);
    return 0;
}
