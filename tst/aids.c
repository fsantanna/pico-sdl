#include "pico.h"

// Injects a ctrl+g key press, as pico_quit does for quit.
static void _push_ctrl_g (void) {
    SDL_Event e = {0};
    e.type = SDL_KEYDOWN;
    e.key.keysym.sym = SDLK_g;
    e.key.keysym.mod = KMOD_LCTRL;
    SDL_PushEvent(&e);
}

int main (void) {
    pico_init(1);
    assert(pico_get_aids() == 1);

    // aids on: ctrl+g is consumed and toggles the grid
    {
        int grid = pico_get_effect_grid();
        _push_ctrl_g();
        Pico_Event e;
        pico_input_event_timeout(&e, PICO_EVENT_KEY_DN, 100);
        assert(e.type == PICO_EVENT_NONE);
        assert(pico_get_effect_grid() == !grid);
    }

    // aids off: ctrl+g passes through to the app
    {
        pico_set_aids(0);
        assert(pico_get_aids() == 0);
        int grid = pico_get_effect_grid();
        _push_ctrl_g();
        Pico_Event e;
        pico_input_event_timeout(&e, PICO_EVENT_KEY_DN, 100);
        assert(e.type == PICO_EVENT_KEY_DN);
        assert(e.keyboard.key == SDLK_g);
        assert(e.keyboard.ctrl);
        assert(pico_get_effect_grid() == grid);
    }

    // aids off: quit is not auto-handled even in non-expert mode
    {
        pico_quit();
        Pico_Event e;
        pico_input_event_timeout(&e, PICO_EVENT_QUIT, 100);
        assert(e.type == PICO_EVENT_QUIT);
    }

    pico_init(0);
    return 0;
}
