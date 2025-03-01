#include "../src/pico.h"

int main (void) {
    pico_init(1);
    pico_set_title("Blend");
    pico_set_font("tiny.ttf", 8);
    pico_set_grid(0);
    pico_set_anchor(PICO_LEFT, PICO_TOP);
    
    pico_output_clear();
    pico_set_color_draw((Pico_Color){255,255,255,255});
    pico_output_draw_text((Pico_Pos){0, 0}, "opaque");

    pico_input_event(NULL, SDL_KEYDOWN);

    pico_output_clear();
    pico_set_color_draw((Pico_Color){255,255,255,122});
    pico_output_draw_text((Pico_Pos){0, 0}, "dimmed");

    pico_input_event(NULL, SDL_KEYDOWN);

    pico_output_clear();
    pico_set_color_draw((Pico_Color){255,255,255,255});
    pico_output_draw_text((Pico_Pos){0, 0}, "dimmed by rect");
    pico_set_color_draw((Pico_Color){0,0,0,122});
    pico_output_draw_rect((Pico_Rect){0, 0, 112, 8});

    pico_input_event(NULL, SDL_KEYDOWN);

    pico_init(0);
    return 0;
}
