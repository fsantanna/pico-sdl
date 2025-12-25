#include "pico.h"

int main (void) {
    pico_init(1);

    Pico_Pct anc = { PICO_ANCHOR_CENTER, PICO_ANCHOR_MIDDLE };

    Pico_Dim phy = pico_get_dim_phy();
    Pico_Dim log = pico_get_dim_log();
    assert(phy.w==640 && phy.h==360);
    assert(log.w==64  && log.h==36 );

    Pico_Pos pt = {50, 50, anc, NULL};
    puts("shows dark screen");

    Pico_Event e1;
    puts("waits any key press");
    pico_input_event(&e1, PICO_KEYUP);

    // TITLE
    puts("changes window title to \"Testing...\"");
    pico_set_title("Testing...");
    pico_input_delay(2000);

    // SOUND
    puts("plays sound");
    pico_output_sound("start.wav");
    pico_input_delay(2000);

    // CLEAR
    pico_set_color_clear((Pico_Color){0xFF,0xFF,0xFF});
    pico_output_clear();

    puts("shows white screen");
    pico_input_delay(2000);

    // DRAW_IMAGE
    pico_output_draw_image(&pt, NULL, "open.png");

    puts("shows centered image");
    pico_input_delay(2000);

    // DRAW_PIXEL/RECT/OVAL
    pico_set_color_clear((Pico_Color){0x00,0x00,0x00});
    pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF});
    pico_output_clear();
    pico_output_draw_pixel(&pt);

    Pico_Pos p1 = { 75, 25, anc, NULL};
    Pico_Dim d1 = { 10, 5, NULL };
    pico_output_draw_rect(&p1, &d1);

    Pico_Pos p2 = { 25, 75, anc, NULL};
    Pico_Dim d2 = { 5, 10, NULL };
    pico_output_draw_oval(&p2, &d2);

    puts("shows oval -> pixel -> rect");
    pico_input_delay(2000);

    // DRAW_TEXT
    pico_output_draw_text(&pt, NULL, "Hello!");

    puts("shows centered \"Hello!\" (on top of shapes)");
    pico_input_delay(2000);
    pico_output_clear();

    // WRITE
    Pico_Pos up = { 10, 10, anc, NULL };
    pico_set_cursor(&up);
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
    {
        Pico_Event e2;
        puts("waits mouse click");
        pico_input_event(&e2, PICO_MOUSEBUTTONDOWN);
        puts("shows pixel over mouse");
        Pico_Pos pos; //pico_mouse_to_pos(&e2);
        pico_output_draw_pixel(&pos);
        pico_input_delay(2000);
    }

    // EVENT
    Pico_Event e3;
    puts("waits more 2 seconds");
    pico_input_event_timeout(&e3, PICO_ANY, 2000);

    // GRID=0
    pico_set_grid(0);
    puts("disables grid");
    pico_input_delay(2000);

    // EXPERT

    pico_output_clear();                    // TODO: should restart cursor?
    pico_set_expert(1);
    pico_set_cursor(&up);
    pico_output_writeln("expert");
    puts("shows expert");
    pico_output_present();
    pico_input_delay(2000);
    pico_set_expert(0);
    pico_output_clear();

    // DRAW_RECT
    Pico_Pos ct = { 50, 50, anc, NULL};
    Pico_Dim d = { 10, 10, NULL };
    Pico_Pos x = { 25, 75, anc, NULL};
    Pico_Pos l = { 100, 0, anc, NULL};
    puts("shows lower-left X, center rect, center/up-right line");
    puts("increases zoom");
    for (int i=1; i<=20; i++) {
        log.w -= 1;
        log.h -= 1;
        pico_set_dim_log(log);
        pico_output_clear();
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF});
        pico_output_draw_rect(&ct, &d);
        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00});
        pico_output_draw_text(&x, NULL, "X");
        pico_output_draw_line(&ct, &l);
        pico_input_delay(250);
    }
    puts("decreases zoom");
    for (int i=1; i<=20; i++) {
        log.w += 1;
        log.h += 1;
        pico_set_dim_log(log);
        pico_output_clear();
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF});
        pico_output_draw_rect(&ct, &d);
        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00});
        pico_output_draw_text(&x, NULL, "X");
        pico_output_draw_line(&ct, &l);
        pico_input_delay(250);
    }
    pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF});

    // PAN

    puts("scrolls right/down");
    for (int i=0; i<20; i++) {
assert(0);
        //pico_set_scroll((Pico_Pos){10-i,10-i});
        pico_output_clear();
        pico_output_draw_text(&pt, NULL, "Uma frase bem grande...");
        pico_input_delay(250);
    }

    pico_init(0);
    return 0;
}
