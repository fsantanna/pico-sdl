#include "pico.h"

// Draw a ladder of text sizes and screenshot, in two configs:
//   A) default world 100x100 upscaled 5x -> 500x500 window
//   B) world 500x500 == window 500x500 (1:1, no scaling)
// Heights use '%' mode (fraction of world), so the SAME fraction yields the
// SAME on-screen size in both configs -- only the rasterization path differs:
//   A rasterizes the glyph at (frac*100) px then upscales 5x  -> blocky
//   B rasterizes the glyph at (frac*500) px at 1:1            -> crisp

static const float FR[] = { 0.016f, 0.02f, 0.024f, 0.032f, 0.04f, 0.056f, 0.08f, 0.12f };
// -> on-screen px: 8, 10, 12, 16, 20, 28, 40, 60

static void ladder (const char* tag) {
    pico_output_clear();
    pico_set_pencil_color((Pico_Color){255,255,255,255});
    float y = 0.01f;
    for (int i = 0; i < 8; i++) {
        float h = FR[i];
        Pico_Rel_Rect r = { '%', {0.012f, y, 0, h}, PICO_ANCHOR_NW };
        pico_output_draw_text("Reading 0123 mq", r);
        y += h + 0.008f;
    }
    char path[128];
    sprintf(path, "out/font_sizes-%s.png", tag);
    pico_output_screenshot("window", path, NULL);
}

int main (void) {
    pico_init(1);
    pico_set_pencil_font("../res/DejaVuSans.ttf");

    ladder("A_world100");                            // default low-res world
    pico_set_dim((Pico_Rel_Dim){'!',{500,500}});     // world == window 500x500
    ladder("B_world500");

    pico_init(0);
    return 0;
}
