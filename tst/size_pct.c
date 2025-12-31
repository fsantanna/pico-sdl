#include "pico.h"

int main (void) {
    pico_init(1);
    pico_set_title("Size - Fullscreen");

    Pico_Dim phy = PICO_DIM_WINDOW;
    Pico_Dim log = PICO_DIM_WORLD;

    Pico_Pos pt = pico_pos((Pico_Pct){50, 50});
    Pico_Rect r = {pt.x, pt.y, 32, 18};

    // phy: normal -> double -> half -> normal
    puts("WINDOW");
    {
        puts("normal");
        pico_set_dim_window((Pico_Dim){phy.x,phy.y});
        pico_output_clear();
        pico_output_draw_rect(r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("double");
        pico_set_dim_window((Pico_Dim){phy.x*2,phy.y*2});
        pico_output_clear();
        pico_output_draw_rect(r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("half");
        pico_set_dim_window((Pico_Dim){phy.x/2,phy.y/2});
        pico_output_clear();
        pico_output_draw_rect(r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("normal");
        pico_set_dim_window((Pico_Dim){phy.x,phy.y});
        pico_output_clear();
        pico_output_draw_rect(r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }

    // phy: normal -> full -> normal
    {
        puts("full");
        pico_set_fullscreen(1);
        pico_output_clear();
        pico_output_draw_rect(r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("normal");
        pico_set_fullscreen(0);
        pico_output_clear();
        pico_output_draw_rect(r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }

    // log: normal -> double -> half -> normal
    puts("WORLD");
    {
        puts("normal");
        pico_set_dim_world((Pico_Dim){log.x,log.y});
        Pico_Pos pt = pico_pos((Pico_Pct){50, 50});
        Pico_Rect r = {pt.x, pt.y, 32, 18};
        pico_output_clear();
        pico_output_draw_rect(r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("double");
        pico_set_dim_world((Pico_Dim){log.x*2,log.y*2});
        Pico_Pos pt = pico_pos((Pico_Pct){50, 50});
        Pico_Rect r = {pt.x, pt.y, 32, 18};
        pico_output_clear();
        pico_output_draw_rect(r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("half");
        pico_set_dim_world((Pico_Dim){log.x/2,log.y/2});
        Pico_Pos pt = pico_pos((Pico_Pct){50, 50});
        Pico_Rect r = {pt.x, pt.y, 32, 18};
        pico_output_clear();
        pico_output_draw_rect(r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }
    {
        puts("normal");
        pico_set_dim_world((Pico_Dim){log.x,log.y});
        Pico_Pos pt = pico_pos((Pico_Pct){50, 50});
        Pico_Rect r = {pt.x, pt.y, 32, 18};
        pico_output_clear();
        pico_output_draw_rect(r);
        pico_input_event(NULL, PICO_KEYDOWN);
    }

    pico_init(0);
    return 0;
}
