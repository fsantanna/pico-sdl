#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_title("Colors Test");
    pico_set_view(
        -1, -1,
        &(Pico_Rel_Dim){ '!', {640, 480}, NULL },
        NULL,
        &(Pico_Rel_Dim){ '!', {64, 48}, NULL },
        NULL,
        NULL
    );

    Pico_Pct C = PICO_ANCHOR_C;

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

    pico_init(0);
    return 0;
}
