#include "../src/pico.h"

int main (void) {
    pico_init(1);
    pico_set_font("tiny.ttf", 10);
    Pico_Pos pt = pico_pct_to_pos(0.5, 0.5);
    puts("shows dark screen");

    SDL_Event e1;
    puts("waits any key press");
    pico_input_event(&e1, SDL_KEYUP);

    // TITLE
    puts("changes window title to \"Testing...\"");
    pico_set_title("Testing...");
    pico_input_delay(2000);

    // SOUND
    puts("plays sound");
    //pico_output_sound("start.wav");
    pico_input_delay(2000);

    // CLEAR
    pico_set_color_clear((SDL_Color){0xFF,0xFF,0xFF,0xFF});
    pico_output_clear();

    puts("shows white screen");
    pico_input_delay(2000);

    // DRAW_IMAGE
    pico_set_anchor(PICO_CENTER, PICO_MIDDLE);
    pico_output_draw_image(pt,"open.png");

    puts("shows centered image");
    pico_input_delay(2000);

    // DRAW_PIXEL/RECT/OVAL
    pico_set_color_clear((SDL_Color){0x00,0x00,0x00,0xFF});
    pico_set_color_draw((SDL_Color){0xFF,0xFF,0xFF,0xFF});
    pico_output_clear();
    pico_output_draw_pixel(pt);
    Pico_Pos rct = pico_pct_to_pos(3/4.0, 1/4.0);
    pico_output_draw_rect((Pico_Rect){ rct.x,rct.y, 10,5});
    Pico_Pos ova = pico_pct_to_pos(1/4.0, 3/4.0);
    pico_output_draw_oval((Pico_Rect){ova.x,ova.y, 5,10});

    puts("shows oval -> pixel -> rect");
    pico_input_delay(2000);

    // DRAW_TEXT
    pico_output_draw_text(pt, "Hello!");

    puts("shows centered \"Hello!\" (on top of shapes)");
    pico_input_delay(2000);
    pico_output_clear();

    // WRITE
    Pico_Pos up = pico_pct_to_pos(1/10.0, 1/10.0);
    pico_set_cursor(up);
    pico_output_write("1 ");
    pico_input_delay(200);
    pico_output_write("2 ");
    pico_input_delay(200);
    pico_output_writeln("3");
    pico_input_delay(200);
    pico_output_writeln("");
    pico_input_delay(200);
    pico_output_writeln("pico");

    puts("shows 1 2 3 \\n \\n pico");
    pico_input_delay(2000);

    // MOUSE
    SDL_Event e2;
    puts("waits mouse click");
    pico_input_event(&e2, SDL_MOUSEBUTTONDOWN);
    pico_output_draw_pixel((Pico_Pos){e2.button.x,e2.button.y});

    puts("shows pixel over mouse");
    pico_input_delay(2000);

    // EVENT
    SDL_Event e3;
    puts("waits more 2 seconds");
    pico_input_event_timeout(&e3, SDL_ANY, 2000);

    // GRID=0
    pico_set_grid(0);
    puts("disables grid");
    pico_input_delay(2000);

    // EXPERT

    pico_output_clear();                    // TODO: should restart cursor?
    pico_set_expert(1);
    pico_set_cursor(up);
    pico_output_writeln("expert");
    puts("shows expert");
    pico_output_present();
    pico_input_delay(2000);
    pico_set_expert(0);
    pico_output_clear();

    // GET SIZE
    Pico_Dim log = pico_get_size().log;

    // DRAW_RECT
    puts("shows lower-left X, center rect, center/up-right line");
    puts("increases zoom");
    for (int i=1; i<=20; i++) {
        log.x -= 1;
        log.y -= 1;
        pico_set_size(PICO_SIZE_KEEP, log);
        Pico_Pos ct = pico_pct_to_pos(0.5, 0.5);
        pico_output_clear();
        pico_set_color_draw((SDL_Color){0xFF,0xFF,0xFF,0xFF});
        pico_output_draw_rect((Pico_Rect){ct.x,ct.y,10,10});
        pico_set_color_draw((SDL_Color){0xFF,0x00,0x00,0xFF});
        pico_output_draw_text(pico_pct_to_pos(1/4.0,3/4.0), "X");
        pico_output_draw_line(ct, pico_pct_to_pos(1,0));
        pico_input_delay(250);
    }
    puts("decreases zoom");
    for (int i=1; i<=20; i++) {
        log.x += 1;
        log.y += 1;
        pico_set_size(PICO_SIZE_KEEP, log);
        Pico_Pos ct = pico_pct_to_pos(0.5, 0.5);
        pico_output_clear();
        pico_set_color_draw((SDL_Color){0xFF,0xFF,0xFF,0xFF});
        pico_output_draw_rect((Pico_Rect){ct.x,ct.y,10,10});
        pico_set_color_draw((SDL_Color){0xFF,0x00,0x00,0xFF});
        pico_output_draw_text(pico_pct_to_pos(1/4.0,3/4.0), "X");
        pico_output_draw_line(ct, pico_pct_to_pos(1,0));
        pico_input_delay(250);
    }
    pico_set_color_draw((SDL_Color){0xFF,0xFF,0xFF,0xFF});

    // PAN

    puts("pans left");
    for (int i=0; i<10; i++) {
        pico_set_pan((Pico_Pos){i,0});
        pico_output_clear();
        pico_output_draw_text(pt, "Uma frase bem grande");
        pico_input_delay(500);
    }

    puts("pans right/down (TODO)");
    for (int i=0; i<20; i++) {
        pico_set_pan((Pico_Pos){10-i,-i});
        pico_output_clear();
        pico_output_draw_text(pt, "Uma frase bem grande");
        pico_output_present();
        pico_input_delay(500);
    }

    pico_init(0);
    return 0;
}
