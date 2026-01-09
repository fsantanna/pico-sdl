#include "pico.h"
#include "../src/colors.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_title("Colors Test");
    pico_set_view_raw(
        -1,
        &(Pico_Dim){640, 480},
        NULL,
        &(Pico_Dim){64, 48},
        NULL,
        NULL
    );

    {
        puts("predefined colors");
        pico_output_clear();

        // Draw rectangles with predefined colors
        float y = 0.1;
        float h = 0.05;

        pico_set_color_draw(PICO_COLOR_RED);
        pico_output_draw_rect_pct(&(Pico_Rect_Pct){0.5, y, 0.8, h, PICO_ANCHOR_C, NULL});
        y += h + 0.02;

        pico_set_color_draw(PICO_COLOR_GREEN);
        pico_output_draw_rect_pct(&(Pico_Rect_Pct){0.5, y, 0.8, h, PICO_ANCHOR_C, NULL});
        y += h + 0.02;

        pico_set_color_draw(PICO_COLOR_BLUE);
        pico_output_draw_rect_pct(&(Pico_Rect_Pct){0.5, y, 0.8, h, PICO_ANCHOR_C, NULL});
        y += h + 0.02;

        pico_set_color_draw(PICO_COLOR_YELLOW);
        pico_output_draw_rect_pct(&(Pico_Rect_Pct){0.5, y, 0.8, h, PICO_ANCHOR_C, NULL});
        y += h + 0.02;

        pico_set_color_draw(PICO_COLOR_CYAN);
        pico_output_draw_rect_pct(&(Pico_Rect_Pct){0.5, y, 0.8, h, PICO_ANCHOR_C, NULL});
        y += h + 0.02;

        pico_set_color_draw(PICO_COLOR_MAGENTA);
        pico_output_draw_rect_pct(&(Pico_Rect_Pct){0.5, y, 0.8, h, PICO_ANCHOR_C, NULL});
        y += h + 0.02;

        pico_set_color_draw(PICO_COLOR_ORANGE);
        pico_output_draw_rect_pct(&(Pico_Rect_Pct){0.5, y, 0.8, h, PICO_ANCHOR_C, NULL});
        y += h + 0.02;

        pico_set_color_draw(PICO_COLOR_PURPLE);
        pico_output_draw_rect_pct(&(Pico_Rect_Pct){0.5, y, 0.8, h, PICO_ANCHOR_C, NULL});
        y += h + 0.02;

        pico_set_color_draw(PICO_COLOR_PINK);
        pico_output_draw_rect_pct(&(Pico_Rect_Pct){0.5, y, 0.8, h, PICO_ANCHOR_C, NULL});

        _pico_check("colors-01");
    }

    {
        puts("darker function");
        pico_output_clear();

        Pico_Color base = PICO_COLOR_RED;
        float y = 0.1;
        float h = 0.08;

        // Original color
        pico_set_color_draw(base);
        pico_output_draw_rect_pct(&(Pico_Rect_Pct){0.5, y, 0.8, h, PICO_ANCHOR_C, NULL});
        y += h + 0.02;

        // Progressively darker
        pico_set_color_draw(pico_color_darker(base, 25));
        pico_output_draw_rect_pct(&(Pico_Rect_Pct){0.5, y, 0.8, h, PICO_ANCHOR_C, NULL});
        y += h + 0.02;

        pico_set_color_draw(pico_color_darker(base, 50));
        pico_output_draw_rect_pct(&(Pico_Rect_Pct){0.5, y, 0.8, h, PICO_ANCHOR_C, NULL});
        y += h + 0.02;

        pico_set_color_draw(pico_color_darker(base, 75));
        pico_output_draw_rect_pct(&(Pico_Rect_Pct){0.5, y, 0.8, h, PICO_ANCHOR_C, NULL});

        _pico_check("colors-02");
    }

    {
        puts("lighter function");
        pico_output_clear();

        Pico_Color base = PICO_COLOR_BLUE;
        float y = 0.1;
        float h = 0.08;

        // Original color
        pico_set_color_draw(base);
        pico_output_draw_rect_pct(&(Pico_Rect_Pct){0.5, y, 0.8, h, PICO_ANCHOR_C, NULL});
        y += h + 0.02;

        // Progressively lighter
        pico_set_color_draw(pico_color_lighter(base, 25));
        pico_output_draw_rect_pct(&(Pico_Rect_Pct){0.5, y, 0.8, h, PICO_ANCHOR_C, NULL});
        y += h + 0.02;

        pico_set_color_draw(pico_color_lighter(base, 50));
        pico_output_draw_rect_pct(&(Pico_Rect_Pct){0.5, y, 0.8, h, PICO_ANCHOR_C, NULL});
        y += h + 0.02;

        pico_set_color_draw(pico_color_lighter(base, 75));
        pico_output_draw_rect_pct(&(Pico_Rect_Pct){0.5, y, 0.8, h, PICO_ANCHOR_C, NULL});

        _pico_check("colors-03");
    }

    {
        puts("negative percentages");
        pico_output_clear();

        Pico_Color base = PICO_COLOR_GREEN;
        float y = 0.15;
        float h = 0.12;

        // Using darker with negative (should lighten)
        pico_set_color_draw(pico_color_darker(base, -50));
        pico_output_draw_rect_pct(&(Pico_Rect_Pct){0.3, y, 0.35, h, PICO_ANCHOR_C, NULL});

        // Using lighter with negative (should darken)
        pico_set_color_draw(pico_color_lighter(base, -50));
        pico_output_draw_rect_pct(&(Pico_Rect_Pct){0.7, y, 0.35, h, PICO_ANCHOR_C, NULL});

        _pico_check("colors-04");
    }

    {
        puts("color gradients");
        pico_output_clear();

        Pico_Color base = PICO_COLOR_ORANGE;
        float x_start = 0.1;
        float w = 0.05;
        float h = 0.6;

        // Gradient from dark to light
        for (int i = 0; i <= 10; i++) {
            int darkness = 90 - (i * 18);  // 90, 72, 54, 36, 18, 0, -18, -36, -54, -72, -90
            pico_set_color_draw(pico_color_darker(base, darkness));
            pico_output_draw_rect_pct(&(Pico_Rect_Pct){
                x_start + (i * w),
                0.5,
                w,
                h,
                PICO_ANCHOR_C,
                NULL
            });
        }

        _pico_check("colors-05");
    }

    pico_init(0);
    return 0;
}
