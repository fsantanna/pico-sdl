#include "pico.h"

int main (void) {
    pico_init(1);
    pico_set_view_raw(-1, &(Pico_Dim){500, 500}, NULL, &(Pico_Dim){50, 50}, NULL, NULL);

    void draw () {
        pico_set_color_draw((Pico_Color) { 0xFF,0x00,0x00 });
        pico_output_draw_pixel_raw((Pico_Pos) { 0,0 });

        pico_set_color_draw((Pico_Color) { 0xFF,0xFF,0xFF });
        pico_output_draw_pixel_raw((Pico_Pos) { 12,12 });

        pico_set_color_draw((Pico_Color) { 0x00,0xFF,0x00 });
        pico_output_draw_pixel_raw((Pico_Pos) { 25,25 });

        pico_set_color_draw((Pico_Color) { 0xAA,0xAA,0xAA });
        pico_set_alpha(0xAA);
        pico_output_draw_pixel_raw((Pico_Pos) { 37,37 });
        pico_set_alpha(0xFF);

        pico_set_color_draw((Pico_Color) { 0x00,0x00,0xFF });
        pico_output_draw_pixel_raw((Pico_Pos) { 49,49 });
    }

    pico_output_clear();
    draw();
    {
        Pico_Event e;
        pico_input_event(&e, PICO_MOUSEBUTTONDOWN);
        Pico_Pos m;
        pico_get_mouse(&m, PICO_MOUSE_BUTTON_NONE);
        printf(">>> (%d,%d) / (%d,%d)\n", e.button.x, e.button.y, m.x, m.y);
        printf(">>> button = %d\n", e.button.button);
        assert(e.button.x==m.x && e.button.y==m.y);
    }

    // Zoom 200% - view becomes 10x10 centered at (20, 20)
    pico_set_view_raw(-1, NULL, NULL, NULL, &(SDL_Rect){20, 20, 10, 10}, NULL);
    pico_output_clear();
    draw();
    {
        Pico_Event e;
        pico_input_event(&e, PICO_MOUSEBUTTONDOWN);
        Pico_Pos m;
        pico_get_mouse(&m, PICO_MOUSE_BUTTON_NONE);
        printf(">>> (%d,%d) / (%d,%d)\n", e.button.x, e.button.y, m.x, m.y);
    }

    // Zoom 50% - view becomes entire world
    pico_set_view_raw(-1, NULL, NULL, NULL, &(SDL_Rect){0, 0, 50, 50}, NULL);
    pico_output_clear();
    draw();
    {
        Pico_Event e;
        pico_input_event(&e, PICO_MOUSEBUTTONDOWN);
        Pico_Pos m;
        pico_get_mouse(&m, PICO_MOUSE_BUTTON_NONE);
        printf(">>> (%d,%d) / (%d,%d)\n", e.button.x, e.button.y, m.x, m.y);
    }

    pico_init(0);
    return 0;
}
