#include "../src/pico.h"

int main (void) {
    pico_init(1);

    SDL_Event e1;
    pico_input_event(&e1, SDL_KEYUP);

    // TITLE
    pico_state_set_title("Testing...");

    // SOUND
    pico_output_sound("start.wav");

    // CLEAR
    pico_state_set_color_clear((SDL_Color){0xFF,0xFF,0xFF,0xFF});
    pico_output_clear();

    pico_input_delay(2000);

    // DRAW_IMAGE
    pico_state_set_anchor(Center,Middle);
    pico_output_draw_image((SDL_Point){0,0},"open.png");

    pico_input_delay(2000);

    // DRAW_PIXEL/RECT/OVAL
    pico_state_set_color_clear((SDL_Color){0x00,0x00,0x00,0xFF});
    pico_state_set_color_draw((SDL_Color){0xFF,0xFF,0xFF,0xFF});
    pico_output_clear();
    pico_output_draw_pixel((SDL_Point){0,0});
    pico_output_draw_rect((SDL_Rect){ 25, 25,10,5});
    pico_output_draw_oval((SDL_Rect){0,0,5,10});

    pico_input_delay(2000);

    // DRAW_TEXT
    pico_state_set_font("tiny.ttf", 10);
    pico_output_draw_text((SDL_Point){0,0}, "Hello!");

    pico_input_delay(2000);
    pico_output_clear();

    // WRITE
    pico_state_set_cursor((SDL_Point){-30,30});
    pico_output_write("1 ");
    pico_output_write("2 ");
    pico_output_writeln("3");
    pico_output_writeln("");
    pico_output_writeln("pico");

    pico_input_delay(2000);

    // MOUSE
    SDL_Event e2;
    pico_input_event(&e2, SDL_MOUSEBUTTONDOWN);
    pico_output_draw_pixel((SDL_Point){e2.button.x,e2.button.y});

    pico_input_delay(2000);

    // EVENT
    SDL_Event e3;
    pico_input_event_timeout(&e3, SDL_ANY, 5000);

    // GRID=0
    pico_state_set_grid(0);

    // AUTO=0
    pico_output_clear();
    pico_state_set_auto(0);
    pico_output_writeln("no auto");
    pico_output_present();

    pico_input_delay(2000);

    // AUTO=1
    pico_state_set_auto(1);
    pico_output_clear();

    // GET SIZE
    SDL_Point log, phy;
    pico_state_get_size_window(&log, &phy);

    // DRAW_RECT
    for (int i=1; i<=20; i++) {
        log.x *= 0.9;
        log.y *= 0.9;
        pico_state_set_size_window(log, phy);
        pico_output_clear();
        pico_output_draw_text((SDL_Point){0,0}, "X");
        pico_output_draw_rect((SDL_Rect){0,0,30,30});
        pico_state_set_color_draw((SDL_Color){0xFF,0x00,0x00,0xFF});
        pico_output_draw_line((SDL_Point){0,0}, (SDL_Point){30,30});
        pico_state_set_color_draw((SDL_Color){0xFF,0xFF,0xFF,0xFF});
        pico_input_delay(200);
    }

    // PAN

    for (int i=0; i<10; i++) {
        pico_state_set_pan((SDL_Point){i,0});
        pico_output_clear();
        pico_output_draw_text((SDL_Point){0,0}, "Uma frase bem grande");
        pico_input_delay(500);
    }

    for (int i=0; i<20; i++) {
        pico_state_set_pan((SDL_Point){10-i,-i});
        pico_output_clear();
        pico_output_draw_text((SDL_Point){0,0}, "Uma frase bem grande");
        pico_input_delay(500);
    }

    pico_init(0);
    return 0;
}
