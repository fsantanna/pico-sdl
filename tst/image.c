#include "../src/pico.h"

int main (void) {
    pico_init(1);
    pico_set_title("Image - Size - Crop");
    Pico_Pos cnt = pico_pct_to_pos(50, 50);
    pico_set_color_clear((Pico_Color){0xFF,0xFF,0xFF,0xFF});

    pico_output_clear();
    pico_output_draw_image(cnt,"open.png");
    puts("show big centered");
    pico_input_event(NULL, SDL_KEYDOWN);

    pico_output_clear();
    pico_set_image_size((Pico_Pos){10,10});
    pico_output_draw_image(cnt,"open.png");
    puts("show small centered");
    pico_input_event(NULL, SDL_KEYDOWN);

    pico_output_clear();
    pico_set_image_size((Pico_Pos){10,15});
    pico_output_draw_image(cnt,"open.png");
    puts("show small/medium distorted");
    pico_input_event(NULL, SDL_KEYDOWN);

    pico_output_clear();
    pico_set_image_size((Pico_Pos){15,0});
    pico_output_draw_image(cnt,"open.png");
    puts("show medium normal");
    pico_input_event(NULL, SDL_KEYDOWN);

    pico_output_clear();
    pico_set_image_size((Pico_Pos){0,10});
    pico_output_draw_image(cnt,"open.png");
    puts("show small normal");
    pico_input_event(NULL, SDL_KEYDOWN);

    pico_output_clear();
    pico_set_image_size((Pico_Dim){0,0});
    pico_output_draw_image(cnt,"open.png");
    puts("show big centered");
    pico_input_event(NULL, SDL_KEYDOWN);

    pico_output_clear();
    pico_set_image_size((Pico_Dim){30,30});
    pico_set_image_crop((Pico_Rect){9,9,30,30});
    pico_output_draw_image(cnt,"open.png");
    puts("show big croped");
    pico_input_event(NULL, SDL_KEYDOWN);

    pico_output_clear();
    pico_set_image_size((Pico_Dim){0,15});
    pico_set_image_crop((Pico_Rect){0,0,0,0});
    pico_output_draw_image(cnt,"open.png");
    puts("show medium normal");
    pico_input_event(NULL, SDL_KEYDOWN);

    pico_init(0);
    return 0;
}
