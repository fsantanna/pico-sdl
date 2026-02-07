#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_window("Push/Pop", -1,
        &(Pico_Rel_Dim){ '!', {640, 480}, NULL });
    pico_set_view(-1,
        &(Pico_Rel_Dim){ '!', {64, 48}, NULL },
        NULL, NULL, NULL, NULL, NULL, NULL);

    // push/pop restores color
    puts("push/pop color");
    {
        pico_output_clear();
        pico_set_color_draw(PICO_COLOR_RED);

        pico_push();
        pico_set_color_draw(PICO_COLOR_BLUE);
        pico_output_draw_rect(&(Pico_Rel_Rect){
            '%', {0.5, 0.25, 0.8, 0.3},
            PICO_ANCHOR_C, NULL
        });
        pico_pop();

        // should be red again
        pico_output_draw_rect(&(Pico_Rel_Rect){
            '%', {0.5, 0.75, 0.8, 0.3},
            PICO_ANCHOR_C, NULL
        });
        _pico_check("push-01");
    }

    // push/pop restores style
    puts("push/pop style");
    {
        pico_output_clear();
        pico_set_color_draw(PICO_COLOR_WHITE);
        pico_set_style(PICO_STYLE_FILL);

        pico_push();
        pico_set_style(PICO_STYLE_STROKE);
        pico_output_draw_rect(&(Pico_Rel_Rect){
            '%', {0.5, 0.25, 0.8, 0.3},
            PICO_ANCHOR_C, NULL
        });
        pico_pop();

        // should be fill again
        pico_output_draw_rect(&(Pico_Rel_Rect){
            '%', {0.5, 0.75, 0.8, 0.3},
            PICO_ANCHOR_C, NULL
        });
        _pico_check("push-02");
    }

    // push/pop restores alpha
    puts("push/pop alpha");
    {
        pico_output_clear();
        pico_set_color_draw(PICO_COLOR_RED);
        pico_set_alpha(0xFF);

        pico_push();
        pico_set_alpha(0x40);
        pico_output_draw_rect(&(Pico_Rel_Rect){
            '%', {0.5, 0.25, 0.8, 0.3},
            PICO_ANCHOR_C, NULL
        });
        pico_pop();

        // should be fully opaque again
        pico_output_draw_rect(&(Pico_Rel_Rect){
            '%', {0.5, 0.75, 0.8, 0.3},
            PICO_ANCHOR_C, NULL
        });
        _pico_check("push-03");
    }

    // nested push/pop
    puts("nested push/pop");
    {
        pico_output_clear();
        pico_set_color_draw(PICO_COLOR_RED);

        pico_push();
        pico_set_color_draw(PICO_COLOR_GREEN);

        pico_push();
        pico_set_color_draw(PICO_COLOR_BLUE);
        pico_output_draw_rect(&(Pico_Rel_Rect){
            '%', {0.5, 0.17, 0.8, 0.2},
            PICO_ANCHOR_C, NULL
        });
        pico_pop();

        // should be green
        pico_output_draw_rect(&(Pico_Rel_Rect){
            '%', {0.5, 0.50, 0.8, 0.2},
            PICO_ANCHOR_C, NULL
        });
        pico_pop();

        // should be red
        pico_output_draw_rect(&(Pico_Rel_Rect){
            '%', {0.5, 0.83, 0.8, 0.2},
            PICO_ANCHOR_C, NULL
        });
        _pico_check("push-04");
    }

    pico_init(0);
    return 0;
}
