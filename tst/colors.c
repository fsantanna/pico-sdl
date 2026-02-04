#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_window("Colors Test", -1, &(Pico_Rel_Dim){ '!', {640, 480}, NULL });
    pico_set_view(-1, &(Pico_Rel_Dim){ '!', {64, 48}, NULL }, NULL, NULL, NULL, NULL);

    Pico_Anchor C = PICO_ANCHOR_C;

    puts("predefined colors");
    {
        pico_output_clear();

        pico_set_color_draw(PICO_COLOR_RED);
        pico_output_draw_rect(&(Pico_Rel_Rect){ '%', {0.5, 0.1, 0.8, 0.05}, C, NULL });

        pico_set_color_draw(PICO_COLOR_GREEN);
        pico_output_draw_rect(&(Pico_Rel_Rect){ '%', {0.5, 0.2, 0.8, 0.05}, C, NULL });

        pico_set_color_draw(PICO_COLOR_BLUE);
        pico_output_draw_rect(&(Pico_Rel_Rect){ '%', {0.5, 0.3, 0.8, 0.05}, C, NULL });

        pico_set_color_draw(PICO_COLOR_YELLOW);
        pico_output_draw_rect(&(Pico_Rel_Rect){ '%', {0.5, 0.4, 0.8, 0.05}, C, NULL });

        pico_set_color_draw(PICO_COLOR_CYAN);
        pico_output_draw_rect(&(Pico_Rel_Rect){ '%', {0.5, 0.5, 0.8, 0.05}, C, NULL });

        pico_set_color_draw(PICO_COLOR_MAGENTA);
        pico_output_draw_rect(&(Pico_Rel_Rect){ '%', {0.5, 0.6, 0.8, 0.05}, C, NULL });

        pico_set_color_draw(PICO_COLOR_ORANGE);
        pico_output_draw_rect(&(Pico_Rel_Rect){ '%', {0.5, 0.7, 0.8, 0.05}, C, NULL });

        pico_set_color_draw(PICO_COLOR_PURPLE);
        pico_output_draw_rect(&(Pico_Rel_Rect){ '%', {0.5, 0.8, 0.8, 0.05}, C, NULL });

        pico_set_color_draw(PICO_COLOR_PINK);
        pico_output_draw_rect(&(Pico_Rel_Rect){ '%', {0.5, 0.9, 0.8, 0.05}, C, NULL });

        _pico_check("colors-01");
    }

    puts("darker function");
    {
        pico_output_clear();
        for (int i=0; i<9; i++) {
            Pico_Color c = pico_color_darker(PICO_COLOR_RED, i*0.1);
            pico_set_color_draw(c);
            pico_output_draw_rect(
                &(Pico_Rel_Rect){ '%', {0.5, 0.1+i*0.1, 0.8, 0.05}, PICO_ANCHOR_C, NULL }
            );
        }
        _pico_check("colors-02");
    }

    puts("ligher function");
    {
        pico_output_clear();
        for (int i=0; i<9; i++) {
            Pico_Color c = pico_color_lighter(PICO_COLOR_BLUE, i*0.1);
            pico_set_color_draw(c);
            pico_output_draw_rect(
                &(Pico_Rel_Rect){ '%', {0.5, 0.1+i*0.1, 0.8, 0.05}, PICO_ANCHOR_C, NULL }
            );
        }
        _pico_check("colors-03");
    }

    {
        puts("negative percentages");
        pico_output_clear();

        // -dark -> light
        pico_set_color_draw(pico_color_darker(PICO_COLOR_GREEN, -0.50));
        pico_output_draw_rect(
            &(Pico_Rel_Rect){ '%', {0.5, 0.25, 0.5, 0.15}, PICO_ANCHOR_C, NULL }
        );

        // green
        pico_set_color_draw(PICO_COLOR_GREEN);
        pico_output_draw_rect(
            &(Pico_Rel_Rect){ '%', {0.5, 0.50, 0.5, 0.15}, PICO_ANCHOR_C, NULL }
        );

        // -light -> dark
        pico_set_color_draw(pico_color_lighter(PICO_COLOR_GREEN, -0.50));
        pico_output_draw_rect(
            &(Pico_Rel_Rect){ '%', {0.5, 0.75, 0.5, 0.15}, PICO_ANCHOR_C, NULL }
        );

        _pico_check("colors-04");
    }

    // mirrors Lua test for '%' and '!' color table formats
    {
        puts("percentage color format");
        pico_output_clear();

        // {'%', r=1, g=0, b=0} -> (255, 0, 0)
        pico_set_color_draw((Pico_Color){255, 0, 0});
        pico_output_draw_rect(
            &(Pico_Rel_Rect){ '%', {0.5, 0.15, 0.8, 0.1}, C, NULL }
        );

        // {'%', r=0, g=1, b=0} -> (0, 255, 0)
        pico_set_color_draw((Pico_Color){0, 255, 0});
        pico_output_draw_rect(
            &(Pico_Rel_Rect){ '%', {0.5, 0.30, 0.8, 0.1}, C, NULL }
        );

        // {'%', r=0, g=0, b=1} -> (0, 0, 255)
        pico_set_color_draw((Pico_Color){0, 0, 255});
        pico_output_draw_rect(
            &(Pico_Rel_Rect){ '%', {0.5, 0.45, 0.8, 0.1}, C, NULL }
        );

        // {'%', r=0.5, g=0.5, b=0.5} -> (127, 127, 127)
        pico_set_color_draw((Pico_Color){127, 127, 127});
        pico_output_draw_rect(
            &(Pico_Rel_Rect){ '%', {0.5, 0.60, 0.8, 0.1}, C, NULL }
        );

        // {'!', r=255, g=255, b=0} -> (255, 255, 0)
        pico_set_color_draw((Pico_Color){255, 255, 0});
        pico_output_draw_rect(
            &(Pico_Rel_Rect){ '%', {0.5, 0.75, 0.8, 0.1}, C, NULL }
        );

        // {r=0, g=255, b=255} -> (0, 255, 255)
        pico_set_color_draw((Pico_Color){0, 255, 255});
        pico_output_draw_rect(
            &(Pico_Rel_Rect){ '%', {0.5, 0.90, 0.8, 0.1}, C, NULL }
        );

        _pico_check("colors-05");
    }

    pico_init(0);
    return 0;
}
