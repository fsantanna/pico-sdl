#include "pico.h"

int main (void) {
    pico_init(1);
    pico_set_size((Pico_Dim){500,500}, (Pico_Dim){50,50});

    void draw () {
        pico_set_color_draw((Pico_Color) { 0xFF,0x00,0x00,0xFF });
        pico_output_draw_pixel((Pico_Pos) { 0,0 });

        pico_set_color_draw((Pico_Color) { 0xFF,0xFF,0xFF,0xFF });
        pico_output_draw_pixel((Pico_Pos) { 12,12 });

        pico_set_color_draw((Pico_Color) { 0x00,0xFF,0x00,0xFF });
        pico_output_draw_pixel((Pico_Pos) { 25,25 });

        pico_set_color_draw((Pico_Color) { 0xAA,0xAA,0xAA,0xAA });
        pico_output_draw_pixel((Pico_Pos) { 37,37 });

        pico_set_color_draw((Pico_Color) { 0x00,0x00,0xFF,0xFF });
        pico_output_draw_pixel((Pico_Pos) { 49,49 });
    }

    pico_output_clear();
    draw();
    {
        Pico_Event e;
        pico_input_event(&e, PICO_MOUSEBUTTONDOWN);
        printf(">>> (%d,%d)\n", e.button.x, e.button.y);
    }

    pico_set_zoom((Pico_Pct){200, 200});
    pico_output_clear();
    draw();
    {
        Pico_Event e;
        pico_input_event(&e, PICO_MOUSEBUTTONDOWN);
        printf(">>> (%d,%d)\n", e.button.x, e.button.y);
    }

    pico_set_zoom((Pico_Pct){50, 50});
    pico_output_clear();
    draw();
    {
        Pico_Event e;
        pico_input_event(&e, PICO_MOUSEBUTTONDOWN);
        printf(">>> (%d,%d)\n", e.button.x, e.button.y);
    }

    pico_init(0);
    return 0;
}
