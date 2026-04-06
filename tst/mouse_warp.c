#include "pico.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

extern SDL_Window* pico_win;

int main(void) {
    pico_init(1);

    puts("phy (500,500) -> log (50,50)");
    {
        pico_set_window("Mouse Warp", -1, &(Pico_Rel_Dim){ '!', {500, 500}, NULL });
        pico_set_view(-1, &(Pico_Rel_Dim){ '!', {50, 50}, NULL }, NULL, NULL, NULL, NULL, NULL, NULL);

        // log (0,0) -> phy (0,0)
        {
            Pico_Rel_Pos p = {'!', {0, 0}, PICO_ANCHOR_NW, NULL};
            pico_warp_mouse(&p);
            SDL_PumpEvents();
            Pico_Mouse pos = pico_get_mouse(0);
            assert(pos.x==0 && pos.y==0);
        }

        // pct (0.5, 0.5) -> log (25,25) -> phy (250,250)
        {
            Pico_Rel_Pos p = {'%', {0.5, 0.5}, PICO_ANCHOR_NW, NULL};
            pico_warp_mouse(&p);
            SDL_PumpEvents();
            Pico_Mouse pos = pico_get_mouse('%');
            assert(pos.x==0.5 && pos.y==0.5);
            Pico_Mouse pos_raw = pico_get_mouse('!');
            assert(pos_raw.x==25 && pos_raw.y==25);
        }
    }

    // Zoom out: src = {-25, -25, 100, 100}
    // centered 100x100 logical
    puts("zoom out 2x");
    {
        pico_set_view(-1, NULL, NULL, NULL,
            &(Pico_Rel_Rect){ '!', {-25, -25, 100, 100}, PICO_ANCHOR_NW, NULL },
            NULL, NULL, NULL);

        // log (25, 25) -> phy (250, 250)
        {
            Pico_Rel_Pos p = {'!', {25, 25}, PICO_ANCHOR_NW, NULL};
            pico_warp_mouse(&p);
            SDL_PumpEvents();
            Pico_Mouse pos = pico_get_mouse(0);
            assert(pos.x==25 && pos.y==25);
        }

        // log (-25, -25) -> phy (0, 0)
        {
            Pico_Rel_Pos p = {'!', {-25, -25}, PICO_ANCHOR_NW, NULL};
            pico_warp_mouse(&p);
            SDL_PumpEvents();
            Pico_Mouse pos = pico_get_mouse(0);
            assert(pos.x==-25 && pos.y==-25);
        }
    }

    pico_init(0);
    return 0;
}
