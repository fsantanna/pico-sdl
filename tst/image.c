#include "../src/pico.h"

int main (void) {
    pico_init(1);
    pico_set_title("Image - Size - Crop");
    SDL_Point cnt = pico_pct_to_pos(0.5, 0.5);
    pico_set_color_clear((SDL_Color){0xFF,0xFF,0xFF,0xFF});
    pico_output_present();                  // show centered

    pico_output_draw_image(cnt,"open.png");
    pico_output_present();
    puts("show big centered");
    pico_input_event(NULL, SDL_KEYDOWN);

    pico_set_image_size((SDL_Point){10,10});
    pico_output_draw_image(cnt,"open.png");
    pico_output_present();
    puts("show small centered");
    pico_input_event(NULL, SDL_KEYDOWN);

    pico_set_image_size((SDL_Point){10,15});
    pico_output_draw_image(cnt,"open.png");
    puts("show small/medium distorted");
    pico_output_present();
    pico_input_event(NULL, SDL_KEYDOWN);

    pico_set_image_size((SDL_Point){15,0});
    pico_output_draw_image(cnt,"open.png");
    puts("show medium normal");
    pico_output_present();
    pico_input_event(NULL, SDL_KEYDOWN);

    pico_set_image_size((SDL_Point){0,10});
    pico_output_draw_image(cnt,"open.png");
    puts("show small normal");
    pico_output_present();
    pico_input_event(NULL, SDL_KEYDOWN);

    pico_set_image_size((SDL_Point){0,0});
    pico_output_draw_image(cnt,"open.png");
    puts("show big centered");
    pico_output_present();
    pico_input_event(NULL, SDL_KEYDOWN);

    pico_set_image_size((SDL_Point){30,30});
    pico_set_image_crop((SDL_Rect){9,9,30,30});
    pico_output_draw_image(cnt,"open.png");
    puts("show big croped");
    pico_output_present();
    pico_input_event(NULL, SDL_KEYDOWN);

    pico_set_image_size((SDL_Point){0,15});
    pico_set_image_crop((SDL_Rect){0,0,0,0});
    pico_output_draw_image(cnt,"open.png");
    puts("show medium normal");
    pico_output_present();
    pico_input_event(NULL, SDL_KEYDOWN);

    pico_init(0);
    return 0;
}
