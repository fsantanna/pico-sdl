#include "../src/pico.h"

int main (void) {
    pico_init(1);                           // show dark screen
    pico_set_font("tiny.ttf", 10);

    SDL_Event e1;
    pico_input_event(&e1, SDL_KEYUP);       // wait any key press

    // TITLE
    pico_set_title("Testing...");     // change window title

    // SOUND
    pico_output_sound("start.wav");         // play sound

    // CLEAR
    pico_set_color_clear((SDL_Color){0xFF,0xFF,0xFF,0xFF});
    pico_output_clear();

    pico_output_present();                  // show white screen
    pico_input_delay(2000);

    // DRAW_IMAGE
    pico_set_anchor(Center,Middle);
    pico_output_draw_image((SDL_Point){0,0},"open.png");

    pico_output_present();                  // show centered image
    pico_input_delay(2000);

    // DRAW_PIXEL/RECT/OVAL
    pico_set_color_clear((SDL_Color){0x00,0x00,0x00,0xFF});
    pico_output_clear();
    pico_set_color_draw((SDL_Color){0xFF,0xFF,0xFF,0xFF});
    pico_output_draw_pixel((SDL_Point){0,0});
    pico_output_draw_rect((SDL_Rect){ 20,20, 10,5});
    pico_output_draw_oval((SDL_Rect){-20,-20, 5,10});

    pico_output_present();                  // show oval -> pixel -> rect
    pico_input_delay(2000);

    // DRAW_TEXT
    pico_output_draw_text((SDL_Point){0,0}, "Hello!");

    pico_output_present();                  // show centered Hello World
    pico_input_delay(2000);

    // WRITE
    pico_set_cursor((SDL_Point){-30,30});
    pico_output_write("1 ");
    pico_output_write("2 ");
    pico_output_writeln("3");
    pico_output_writeln("");
    pico_output_writeln("pico");

    pico_output_present();                  // show 1 2 3 \n \n pico
    pico_input_delay(2000);

    // MOUSE
    SDL_Event e2;
    pico_input_event(&e2, SDL_MOUSEBUTTONDOWN);
    pico_output_draw_pixel((SDL_Point){e2.button.x,e2.button.y});

    pico_output_present();                  // show button click
    pico_input_delay(2000);

    // EVENT
    SDL_Event e3;
    pico_input_event_timeout(&e3, SDL_ANY, 5000);

    // GRID=0
    pico_set_grid(0);

    // AUTO=0
    pico_output_clear();                    // TODO: should restart cursor?
    //pico_set_auto(0);
    pico_output_writeln("no auto");

    pico_output_present();                  // show no auto
    pico_input_delay(2000);

    // GET SIZE
    SDL_Point log, phy;
    pico_get_window_size(&log, &phy);

    // DRAW_RECT
    for (int i=1; i<=20; i++) {
        log.x -= 1;
        log.y -= 1;
        pico_set_size_window(log, phy);
        pico_set_color_draw((SDL_Color){0xFF,0xFF,0xFF,0xFF});
        pico_output_draw_rect((SDL_Rect){0,0,30,30});
        pico_set_color_draw((SDL_Color){0xFF,0x00,0x00,0xFF});
        pico_output_draw_text((SDL_Point){-15,15}, "X");
        pico_output_draw_line((SDL_Point){0,0}, (SDL_Point){30,30});
        pico_output_present();
        pico_input_delay(250);
    }
    for (int i=1; i<=20; i++) {
        log.x += 1;
        log.y += 1;
        pico_set_size_window(log, phy);
        pico_set_color_draw((SDL_Color){0xFF,0xFF,0xFF,0xFF});
        pico_output_draw_rect((SDL_Rect){0,0,30,30});
        pico_set_color_draw((SDL_Color){0xFF,0x00,0x00,0xFF});
        pico_output_draw_text((SDL_Point){-15,15}, "X");
        pico_output_draw_line((SDL_Point){0,0}, (SDL_Point){30,30});
        pico_output_present();
        pico_input_delay(250);
    }
    pico_set_color_draw((SDL_Color){0xFF,0xFF,0xFF,0xFF});

    // PAN

    for (int i=0; i<10; i++) {
        pico_set_pan((SDL_Point){i,0});
        pico_output_draw_text((SDL_Point){0,0}, "Uma frase bem grande");
        pico_output_present();
        pico_input_delay(500);
    }

    for (int i=0; i<20; i++) {
        pico_set_pan((SDL_Point){10-i,-i});
        pico_output_draw_text((SDL_Point){0,0}, "Uma frase bem grande");
        pico_output_present();
        pico_input_delay(500);
    }

    pico_init(0);
    return 0;
}
