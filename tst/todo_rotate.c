#include "pico.h"

int main (void) {
    pico_init(1);
    pico_set_view("Rotate", -1, -1, NULL, NULL, NULL, NULL, NULL);
    pico_set_view_raw(0, -1, &(Pico_Dim){500, 500}, NULL, &(Pico_Dim){100, 100}, NULL, NULL);
    pico_set_anchor_pos((Pico_Anchor){PICO_CENTER, PICO_MIDDLE});
    pico_set_font(NULL, 16);

    Pico_Pos pt = {50, 50};  // Center of 100x100 world
    Pico_Rect rect = {pt.x, pt.y, 50, 50};

    Pico_Color buffer[] = {
        { 0x00, 0x00, 0x00 },
        { 0xFF, 0xFF, 0x00 },
        { 0x00, 0x00, 0x00 },
        { 0xFF, 0x00, 0x00 },
        { 0x00, 0xFF, 0x00 },
        { 0x00, 0x00, 0xFF },
        { 0x00, 0x00, 0x00 },
        { 0x00, 0xFF, 0xFF },
        { 0x00, 0x00, 0x00 },
    };

    puts("RECT");
    pico_set_rotate(0);
    pico_output_clear();
    pico_output_draw_rect(rect);
    pico_input_delay(500);
    for (int i = 0; i < 72; i++) {
        pico_set_rotate(pico_get_rotate() + 5);
        pico_output_clear();
        pico_output_draw_rect(rect);
        pico_input_delay(50);
    }
    pico_input_delay(500);

    puts("RECT - anchor rotate 110%110%");
    pico_set_rotate(0);
    pico_output_clear();
    pico_set_anchor_rotate((Pico_Anchor){110,110});
    pico_output_draw_rect(rect);
    pico_input_delay(500);
    for (int i = 0; i < 72; i++) {
        pico_set_rotate(pico_get_rotate() + 5);
        pico_output_clear();
        pico_output_draw_rect(rect);
        pico_input_delay(50);
    }
    pico_input_delay(500);

    puts("RECT - anchor rotate -10%-10%");
    pico_set_rotate(0);
    pico_output_clear();
    pico_set_anchor_rotate((Pico_Anchor){-10,-10});
    pico_output_draw_rect(rect);
    pico_input_delay(500);
    for (int i = 0; i < 72; i++) {
        pico_set_rotate(pico_get_rotate() + 5);
        pico_output_clear();
        pico_output_draw_rect(rect);
        pico_input_delay(50);
    }
    pico_set_anchor_rotate((Pico_Anchor){50,50});
    pico_input_delay(500);

    puts("RECT");
    pico_set_rotate(0);
    pico_output_clear();
    pico_output_draw_rect(rect);
    pico_input_delay(500);
    for (int i = 0; i < 72; i++) {
        pico_set_rotate(pico_get_rotate() + 5);
        pico_output_clear();
        pico_output_draw_rect(rect);
        pico_input_delay(50);
    }
    pico_input_delay(500);
    puts("OVAL");
    pico_set_rotate(0);
    pico_output_clear();
    pico_output_draw_oval(rect);
    pico_input_delay(500);
    for (int i = 0; i < 72; i++) {
        pico_set_rotate(pico_get_rotate() + 5);
        pico_output_clear();
        pico_output_draw_oval(rect);
        pico_input_delay(50);
    }
    pico_input_delay(500);

    puts("TRI");
    pico_set_rotate(0);
    pico_output_clear();
    pico_output_draw_tri(rect);
    pico_input_delay(500);
    for (int i = 0; i < 72; i++) {
        pico_set_rotate(pico_get_rotate() + 5);
        pico_output_clear();
        pico_output_draw_tri(rect);
        pico_input_delay(50);
    }
    pico_input_delay(500);

    puts("LINE");
    pico_set_rotate(0);
    pico_output_clear();
    pico_output_draw_line((Pico_Pos){pt.x-25,pt.y-25}, (Pico_Pos){pt.x+25,pt.y+25});
    pico_input_delay(500);
    for (int i = 0; i < 72; i++) {
        pico_set_rotate(pico_get_rotate() + 5);
        pico_output_clear();
        pico_output_draw_line((Pico_Pos){pt.x-25,pt.y-25}, (Pico_Pos){pt.x+25,pt.y+25});
        pico_input_delay(50);
    }
    pico_input_delay(500);

    puts("TEXT");
    pico_set_rotate(0);
    pico_output_clear();
    pico_output_draw_text(pt, "HEY");
    pico_input_delay(500);
    for (int i = 0; i < 72; i++) {
        pico_set_rotate(pico_get_rotate() + 5);
        pico_output_clear();
        pico_output_draw_text(pt, "HEY");
        pico_input_delay(50);
    }
    pico_input_delay(500);

    puts("IMAGE");
    pico_set_rotate(0);
    pico_output_clear();
    pico_output_draw_image(pt, "open.png");
    pico_input_delay(500);
    for (int i = 0; i < 72; i++) {
        pico_set_rotate(pico_get_rotate() + 5);
        pico_output_clear();
        pico_output_draw_image(pt, "open.png");
        pico_input_delay(50);
    }
    pico_input_delay(500);

    puts("BUFFER");
    pico_set_rotate(0);
    pico_output_clear();
    pico_output_draw_buffer(pt, buffer, (Pico_Dim){3,3});
    pico_input_delay(500);
    for (int i = 0; i < 72; i++) {
        pico_set_rotate(pico_get_rotate() + 5);
        pico_output_clear();
        pico_output_draw_buffer(pt, buffer, (Pico_Dim){3,3});
        pico_input_delay(50);
    }
    pico_input_event(NULL, PICO_KEYDOWN);

    pico_init(0);
    return 0;
}
