#include "pico.h"

int main (void) {
    pico_init(1);
    pico_set_view(-1,
        &(Pico_Rel_Dim){'!', {10, 10}, NULL},
        NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    pico_output_clear();
    pico_set_color_draw(pico_color_alpha(pico_get_color_draw(), 0x80));

    // Each position drawn 3 times with NW/C/SE anchors
#if 1
    float px[] = { 100, 250, 400 };
    float py[] = { 100, 250, 400 };
    for (int i = 0; i < 3; i++) {
        float x = px[i];
        float y = py[i];

        // Red — NW anchor
        pico_set_color_draw(PICO_COLOR_RED);
        pico_output_draw_pixel(&(Pico_Rel_Pos){'w', {x, y}, PICO_ANCHOR_NW, NULL});

        // Green — C anchor
        pico_set_color_draw(PICO_COLOR_GREEN);
        pico_output_draw_pixel(&(Pico_Rel_Pos){'w', {x, y}, PICO_ANCHOR_C, NULL});

        // Blue — SE anchor
        pico_set_color_draw(PICO_COLOR_BLUE);
        pico_output_draw_pixel(&(Pico_Rel_Pos){'w', {x, y}, PICO_ANCHOR_SE, NULL});
    }
#endif

    // Same positions in world coordinates ('!')
#if 0
    float wx[] = { 2, 5, 8 };
    float wy[] = { 2, 5, 8 };
    for (int i = 0; i < 3; i++) {
        float x = wx[i];
        float y = wy[i];

        // Red — NW anchor
        pico_set_color_draw(PICO_COLOR_RED);
        pico_output_draw_pixel(&(Pico_Rel_Pos){'!', {x, y}, PICO_ANCHOR_NW, NULL});

        // Green — C anchor
        pico_set_color_draw(PICO_COLOR_GREEN);
        pico_output_draw_pixel(&(Pico_Rel_Pos){'!', {x, y}, PICO_ANCHOR_C, NULL});

        // Blue — SE anchor
        pico_set_color_draw(PICO_COLOR_BLUE);
        pico_output_draw_pixel(&(Pico_Rel_Pos){'!', {x, y}, PICO_ANCHOR_SE, NULL});
    }
#endif

    pico_input_event(NULL, PICO_EVENT_QUIT);
    pico_init(0);
    return 0;
}
