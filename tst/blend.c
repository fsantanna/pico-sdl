#include "pico.h"

int main (void) {
    pico_init(1);
    pico_set_title("Blend");
    pico_set_grid(0);
    Pico_Pos pos = pico_pos(50, 50);

    // TEXT
    
    pico_output_clear();
    pico_set_color_draw((Pico_Color){255,255,255,255});
    pico_output_draw_text(pos, "SOME TEXT");
    pico_input_delay(2000);

    pico_output_clear();
    pico_set_color_draw((Pico_Color){255,255,255,120});
    pico_output_draw_text(pos, "SOME TEXT");
    pico_input_delay(2000);

    // RECT

    pico_output_clear();
    pico_set_color_draw((Pico_Color){255,255,255,255});
    pico_output_draw_rect((Pico_Rect){pos.x, pos.y, 60, 12});
    pico_input_delay(2000);

    pico_output_clear();
    pico_set_color_draw((Pico_Color){255,255,255,120});
    pico_output_draw_rect((Pico_Rect){pos.x, pos.y, 60, 12});
    pico_input_delay(2000);

    // OVAL
    pico_set_color_clear((Pico_Color){255,120,120,255});

    pico_output_clear();
    pico_set_color_draw((Pico_Color){255,255,255,255});
    pico_output_draw_oval((Pico_Rect){pos.x, pos.y, 60, 12});
    pico_input_delay(2000);

    pico_output_clear();
    pico_set_color_draw((Pico_Color){255,255,255,120});
    pico_output_draw_oval((Pico_Rect){pos.x, pos.y, 60, 12});
    pico_input_delay(2000);

    // TRI

    pico_output_clear();
    pico_set_color_draw((Pico_Color){255,255,255,255});
    pico_output_draw_tri((Pico_Rect){pos.x, pos.y, 60, 12});
    pico_input_delay(2000);

    pico_output_clear();
    pico_set_color_draw((Pico_Color){255,255,255,120});
    pico_output_draw_tri((Pico_Rect){pos.x, pos.y, 60, 12});
    pico_input_delay(2000);

    // LINE

    pico_output_clear();
    pico_set_color_draw((Pico_Color){255,255,255,255});
    pico_output_draw_line((Pico_Pos){pos.x-20, pos.y-6}, (Pico_Pos){pos.x+20,pos.y+6});
    pico_input_delay(2000);

    pico_output_clear();
    pico_set_color_draw((Pico_Color){255,255,255,120});
    pico_output_draw_line((Pico_Pos){pos.x-20, pos.y-6}, (Pico_Pos){pos.x+20,pos.y+6});
    pico_input_delay(2000);

    pico_init(0);
    return 0;
}
