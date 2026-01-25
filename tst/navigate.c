#include <SDL2/SDL.h>

#include "pico.h"
#include "../check.h"

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
    pico_set_view(-1, -1, NULL, NULL,
        &(Pico_Rel_Dim){ '!', {1000, 1000}, NULL }, NULL, NULL);

    // draw
    {
        // X at center
        pico_output_draw_line(
            &(Pico_Rel_Pos){ '%', {0.45, 0.45}, PICO_ANCHOR_C, NULL },
            &(Pico_Rel_Pos){ '%', {0.55, 0.55}, PICO_ANCHOR_C, NULL }
        );
        pico_output_draw_line(
            &(Pico_Rel_Pos){ '%', {0.55, 0.45}, PICO_ANCHOR_C, NULL },
            &(Pico_Rel_Pos){ '%', {0.45, 0.55}, PICO_ANCHOR_C, NULL }
        );

        // RECT at center
        pico_set_style(PICO_STYLE_STROKE);
        pico_output_draw_rect(
            &(Pico_Rel_Rect){ '%', {0.5, 0.5, 0.25, 0.25}, PICO_ANCHOR_C, NULL }
        );
    }

    // navigate: 3 up, 3 left, 3 minus, 2 right, 4 down, 2 plus
    _pico_check("navigate-01");
    for (int i=0; i<3; i++) { push_key(SDLK_UP);    pico_input_event(NULL, PICO_ANY); }
    _pico_check("navigate-02");
    for (int i=0; i<3; i++) { push_key(SDLK_LEFT);  pico_input_event(NULL, PICO_ANY); }
    _pico_check("navigate-03");
    for (int i=0; i<3; i++) { push_key(SDLK_MINUS); pico_input_event(NULL, PICO_ANY); }
    _pico_check("navigate-04");
    for (int i=0; i<2; i++) { push_key(SDLK_RIGHT); pico_input_event(NULL, PICO_ANY); }
    _pico_check("navigate-05");
    for (int i=0; i<4; i++) { push_key(SDLK_DOWN);  pico_input_event(NULL, PICO_ANY); }
    _pico_check("navigate-06");
    for (int i=0; i<2; i++) { push_key(SDLK_EQUALS); pico_input_event(NULL, PICO_ANY); }
    _pico_check("navigate-07");

    pico_init(0);
    return 0;
}
