#include "pico.h"
#include <SDL2/SDL.h>

// Push a Ctrl+key event to simulate navigation
void push_key (SDL_Keycode key) {
    SDL_Event e = {0};
    e.type = SDL_KEYDOWN;
    e.key.keysym.sym = key;
    e.key.keysym.mod = KMOD_LCTRL;
    SDL_PushEvent(&e);
}

int main() {
    pico_init(1);
    pico_set_view_raw(-1, NULL, NULL, &(Pico_Dim){1000, 1000}, NULL, NULL);

    // draw
    {
        // Draw X at center of world
        pico_output_draw_line_pct(
            &(Pico_Pos_Pct){0.45, 0.45, PICO_ANCHOR_C, NULL},
            &(Pico_Pos_Pct){0.55, 0.55, PICO_ANCHOR_C, NULL}
        );
        pico_output_draw_line_pct(
            &(Pico_Pos_Pct){0.55, 0.45, PICO_ANCHOR_C, NULL},
            &(Pico_Pos_Pct){0.45, 0.55, PICO_ANCHOR_C, NULL}
        );

        // Draw rectangle at center of world (1/4 size)
        pico_set_style(PICO_STROKE);
        pico_output_draw_rect_pct(
            &(Pico_Rect_Pct){0.5, 0.5, 0.25, 0.25, PICO_ANCHOR_C, NULL}
        );
    }

    // Generate navigation key presses: 5 up, 3 left, 2 right, 4 down
    _pico_check("navigate-01");
    for (int i=0; i<5; i++) push_key(SDLK_UP);
    _pico_check("navigate-02");
    for (int i=0; i<3; i++) push_key(SDLK_LEFT);
    _pico_check("navigate-03");
    for (int i=0; i<2; i++) push_key(SDLK_RIGHT);
    _pico_check("navigate-04");
    for (int i=0; i<4; i++) push_key(SDLK_DOWN);
    _pico_check("navigate-05");

    pico_init(0);
    return 0;
}
