#include "pico.h"

int main (void) {
    pico_init(1);

    Pico_Dim window = pico_get_dim_window();
    Pico_Dim world  = pico_get_dim_world();
    assert(window.x==640 && window.y==360);
    assert(world.x==64 && world.y==36);

    Pico_Pos ct = pico_pos((Pico_Pct){50, 50});

#if 0
    pico_set_style(PICO_STROKE);

    while (1) {
        pico_output_clear();
        pico_output_draw_pixel(ct);
        pico_output_draw_rect((Pico_Rect){ct.x,ct.y,64,36});
        pico_input_event(NULL, PICO_KEYUP);
    }
#endif

    // DRAW_RECT
    puts("shows lower-left X, center rect, center/up-right line");
    puts("increases zoom");
    for (int i=1; i<=20; i++) {
        world.x -= 1;
        world.y -= 1;
        pico_set_dim_world(world);
        Pico_Pos ct = pico_pos((Pico_Pct){50, 50});
        pico_output_clear();
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF,0xFF});
        pico_output_draw_rect((Pico_Rect){ct.x,ct.y,10,10});
        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00,0xFF});
        pico_output_draw_text(pico_pos((Pico_Pct){25,75}), "X");
        pico_output_draw_line(ct, pico_pos((Pico_Pct){100,0}));
        pico_input_delay(250);
    }
    puts("decreases zoom");
    for (int i=1; i<=20; i++) {
        world.x += 1;
        world.y += 1;
        pico_set_dim_world(world);
        Pico_Pos ct = pico_pos((Pico_Pct){50, 50});
        pico_output_clear();
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF,0xFF});
        pico_output_draw_rect((Pico_Rect){ct.x,ct.y,10,10});
        pico_set_color_draw((Pico_Color){0xFF,0x00,0x00,0xFF});
        pico_output_draw_text(pico_pos((Pico_Pct){25,75}), "X");
        pico_output_draw_line(ct, pico_pos((Pico_Pct){100,0}));
        pico_input_delay(250);
    }
    pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF,0xFF});

    // PAN

    puts("scrolls right/down");
    for (int i=0; i<20; i++) {
        pico_set_scroll((Pico_Pos){10-i,10-i});
        pico_output_clear();
        pico_output_draw_text(ct, "Uma frase bem grande...");
        pico_input_delay(250);
    }

    pico_init(0);
    return 0;
}
