#include "pico.h"

int main (void) {
    pico_init(1);
    pico_set_title("Size - Fullscreen");

    Pico_Rect_Pct r = {0.5, 0.5, 0.5, 0.5, PICO_ANCHOR_C, NULL};

    // phy: normal -> double -> half -> normal
    puts("WINDOW");
    {
        puts("normal");
        pico_set_view_pct(-1, &(Pico_Pct){1,1}, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("double");
        pico_set_view_pct(-1, &(Pico_Pct){2,2}, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("half");
        pico_set_view_pct(-1, &(Pico_Pct){0.25,0.25}, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("normal");
        pico_set_view_pct(-1, &(Pico_Pct){2,2}, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }

    // phy: normal -> full -> normal
    {
        puts("full");
        pico_set_view_pct(1, NULL, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("normal");
        pico_set_view_pct(0, NULL, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }

    // log: normal -> double -> half -> normal
    puts("WORLD");
    {
        puts("normal");
        pico_set_view_pct(-1, NULL, NULL, &(Pico_Pct){1,1}, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("double");
        pico_set_view_pct(-1, NULL, NULL, &(Pico_Pct){2,2}, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("half");
        pico_set_view_pct(-1, NULL, NULL, &(Pico_Pct){0.25,0.25}, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("normal");
        pico_set_view_pct(-1, NULL, NULL, &(Pico_Pct){2,2}, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_pct(&r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }

    pico_init(0);
    return 0;
}
