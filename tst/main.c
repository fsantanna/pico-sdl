#include "pico.h"

int main (void) {
    pico_init(1);

    puts("shows dark screen");
    {
        Pico_Dim phy = pico_get_dim_window();
        Pico_Dim log = pico_get_dim_world();
        assert(phy.x==500 && phy.y==500);
        assert(log.x==100 && log.y==100);
    }

    puts("waits any key press");
    {
        Pico_Event e;
        pico_input_event(&e, PICO_KEYUP);
    }

    puts("changes window title to \"Testing...\"");
    {
        pico_set_title("Testing...");
        pico_input_delay(2000);
    }

    puts("plays sound");
    {
        pico_output_sound("start.wav");
        pico_input_delay(2000);
    }

    puts("shows white screen");
    {
        pico_set_color_clear((Pico_Color){0xFF,0xFF,0xFF});
        pico_output_clear();
        pico_input_delay(2000);
    }

    puts("shows centered image");
    {
        Pico_Rect_Pct r = { 0.5,0.5, 0,0, PICO_ANCHOR_C, NULL };
        pico_output_draw_image_pct(&r, "open.png");
        pico_input_delay(2000);
    }

    puts("shows oval -> pixel -> rect");
    {
        pico_set_color_clear((Pico_Color){0x00,0x00,0x00});
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF});
        pico_output_clear();
        {
            Pico_Pos_Pct p = { 0.5,0.5, PICO_ANCHOR_C, NULL };
            pico_output_draw_pixel_pct(&p);
        }
        {
            Pico_Rect_Pct r = { 0.75,0.25, 0.2,0.2, PICO_ANCHOR_C, NULL };
            pico_output_draw_rect_pct(&r);
        }
        {
            Pico_Rect_Pct r = { 0.25,0.75, 0.2,0.2, PICO_ANCHOR_C, NULL };
            pico_output_draw_oval_pct(&r);
        }
        pico_input_delay(2000);
    }

#if TODO
    // DRAW_TEXT
    pico_output_draw_text(pt, "Hello!");

    puts("shows centered \"Hello!\" (on top of shapes)");
    pico_input_delay(2000);
    pico_output_clear();

    // WRITE
    Pico_Pos up = pico_pos((Pico_Pct){10, 10});
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
#endif

    puts("waits mouse click then");
    puts("shows pixel over mouse");
    {
        Pico_Event e;
        pico_input_event(&e, PICO_MOUSEBUTTONDOWN);
        pico_output_draw_pixel_raw((Pico_Pos){e.button.x,e.button.y});
        pico_input_delay(2000);
    }

    puts("waits more 2 seconds");
    {
        Pico_Event e;
        pico_input_event_timeout(&e, PICO_ANY, 2000);
    }

    puts("disables grid");
    {
        pico_set_grid(0);
        pico_input_delay(2000);
    }

#if TODO
    puts("shows expert");
    {
        pico_output_clear();        // TODO: should restart cursor?
        pico_set_expert(1);
        pico_set_cursor(up);
        pico_output_writeln("expert");
        pico_output_present();
        pico_input_delay(2000);
        pico_set_expert(0);
        pico_output_clear();
    }

    // DRAW_RECT
    puts("shows lower-left X, center rect, center/up-right line");
    puts("increases zoom");
    for (int i=1; i<=20; i++) {
        log.x -= 1;
        log.y -= 1;
        pico_set_dim_world(log);
        Pico_Pos ct = pico_pos((Pico_Pct){50, 50});
        pico_output_clear();
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF});
        pico_output_draw_rect((Pico_Rect){ct.x,ct.y,10,10});
        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00});
        pico_output_draw_text(pico_pos((Pico_Pct){25,75}), "X");
        pico_output_draw_line(ct, pico_pos((Pico_Pct){100,0}));
        pico_input_delay(250);
    }
    puts("decreases zoom");
    for (int i=1; i<=20; i++) {
        log.x += 1;
        log.y += 1;
        pico_set_dim_world(log);
        Pico_Pos ct = pico_pos((Pico_Pct){50, 50});
        pico_output_clear();
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF});
        pico_output_draw_rect((Pico_Rect){ct.x,ct.y,10,10});
        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00});
        pico_output_draw_text(pico_pos((Pico_Pct){25,75}), "X");
        pico_output_draw_line(ct, pico_pos((Pico_Pct){100,0}));
        pico_input_delay(250);
    }
    pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF});

    // PAN

    puts("scrolls right/down");
    for (int i=0; i<20; i++) {
        pico_set_scroll((Pico_Pos){10-i,10-i});
        pico_output_clear();
        pico_output_draw_text(pt, "Uma frase bem grande...");
        pico_input_delay(250);
    }
#endif

    pico_init(0);
    return 0;
}
