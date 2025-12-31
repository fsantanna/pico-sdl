#include "pico.h"

int main (void) {
    pico_init(1);
    pico_set_title("Size - Fullscreen");

    Pico_Dim phy = PICO_DIM_WINDOW;
    Pico_Dim log = PICO_DIM_WORLD;
    Pico_Rect r = {50-25, 50-25, 50, 50};

    // phy: normal -> double -> half -> normal
    puts("WINDOW");
    {
        puts("normal");
        pico_set_view(-1, &phy, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_raw(r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("double");
        Pico_Dim dim = (Pico_Dim){phy.x*2,phy.y*2};
        pico_set_view(-1, &dim, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_raw(r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("half");
        Pico_Dim dim = (Pico_Dim){phy.x/2,phy.y/2};
        pico_set_view(-1, &dim, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_raw(r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("normal");
        pico_set_view(-1, &phy, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_raw(r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }

    // phy: normal -> full -> normal
    {
        puts("full");
        pico_set_view(1, NULL, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_raw(r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("normal");
        pico_set_view(0, NULL, NULL, NULL, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_raw(r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }

    // log: normal -> double -> half -> normal
    puts("WORLD");
    {
        puts("normal");
        pico_set_view(-1, NULL, NULL, &log, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_raw(r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("double");
        Pico_Dim dim = (Pico_Dim){log.x*2,log.y*2};
        //Pico_Pos pos = { 500-log.x, 500-log.y };
        pico_set_view(-1, NULL, NULL, &dim, NULL, NULL);
        pico_output_clear();
        Pico_Rect r = {log.x, log.x, 50, 50};
        pico_output_draw_rect_raw(r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("half");
        Pico_Dim dim = (Pico_Dim){log.x/2,log.y/2};
        pico_set_view(-1, NULL, NULL, &dim, NULL, NULL);
        pico_output_clear();
        Pico_Rect r = {log.x/4-25, log.x/4-25, 50, 50};
        pico_output_draw_rect_raw(r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("normal");
        pico_set_view(-1, NULL, NULL, &log, NULL, NULL);
        pico_output_clear();
        pico_output_draw_rect_raw(r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }

    pico_init(0);
    return 0;
}
