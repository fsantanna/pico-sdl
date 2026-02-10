#include "pico.h"

int main (void) {
    pico_init(1);

    puts("shows dark screen");
    {
        Pico_Dim phy, log;
        pico_get_window(NULL, NULL, &phy);
        pico_get_view(NULL, &log, NULL, NULL, NULL, NULL, NULL, NULL);
        assert(phy.w==500 && phy.h==500);
        assert(log.w==100 && log.h==100);
    }

    puts("waits any key press");
    {
        Pico_Event e;
        pico_input_event(&e, PICO_EVENT_KEY_UP);
    }

    puts("changes window title to \"Testing...\"");
    {
        pico_set_window("Testing...", -1, NULL);
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
        pico_output_draw_image_pct("open.png", &r);
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

    puts("shows centered \"Hello!\" (on top of shapes)");
    {
        int w = pico_get_text(20, "Hello!");
        Pico_Rect r = {50-w/2, 50-5, 0, 20};
        pico_output_draw_text_raw("Hello!", r);
        pico_input_delay(2000);
    }

    puts("waits mouse click then");
    puts("shows pixel over mouse");
    {
        Pico_Event e;
        pico_input_event(&e, PICO_EVENT_MOUSE_BUTTON_DOWN);
        pico_output_draw_pixel_raw((Pico_Pos){e.button.x,e.button.y});
        pico_input_delay(2000);
    }

    puts("waits more 2 seconds");
    {
        Pico_Event e;
        pico_input_event_timeout(&e, PICO_EVENT_ANY, 2000);
    }

    puts("disables grid");
    {
        pico_set_view_raw(0, -1, NULL, NULL, NULL, NULL, NULL);
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
#endif

#if TODO
    // DRAW_RECT
    puts("shows lower-left X, center rect, center/up-right line");
    puts("increases zoom");
    for (int i=1; i<=20; i++) {
        log.w -= 1;
        log.h -= 1;
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
        log.w += 1;
        log.h += 1;
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
