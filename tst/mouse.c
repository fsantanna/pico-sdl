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
        pico_set_window("Mouse", -1, &(Pico_Rel_Dim){ '!', {500, 500}, NULL });
        pico_set_view(-1, &(Pico_Rel_Dim){ '!', {50, 50}, NULL }, NULL, NULL, NULL, NULL, NULL, NULL);

        // phy (0,0) -> log (0,0)
        {
            SDL_WarpMouseInWindow(pico_win, 0, 3);
            SDL_PumpEvents();
            Pico_Rel_Pos pos = { '!' };
            pico_get_mouse(&pos, PICO_EVENT_MOUSE_BUTTON_NONE);
            assert(pos.x==0 && pos.y==0.3f);
        }

        // phy (250,250) -> log (25,25)
        {
            SDL_WarpMouseInWindow(pico_win, 250, 251);
            SDL_PumpEvents();
            Pico_Rel_Pos pos = { '!' };
            pico_get_mouse(&pos, PICO_EVENT_MOUSE_BUTTON_NONE);
            assert(pos.x==25 && pos.y==25.099998f);
        }

        // phy (490,490) -> log (49,49)
        {
            SDL_WarpMouseInWindow(pico_win, 499, 490);
            SDL_PumpEvents();
            Pico_Rel_Pos pos = { '!' };
            pico_get_mouse(&pos, PICO_EVENT_MOUSE_BUTTON_NONE);
            assert(pos.x==49.900002f && pos.y==49);
        }
    }

    // Zoom out: src = {-25, -25, 100, 100}
    // centered 100x100 logical
    puts("zoom out 2x");
    {
        pico_set_view(-1, NULL, NULL, NULL,
            &(Pico_Rel_Rect){ '!', {-25, -25, 100, 100}, PICO_ANCHOR_NW, NULL },
            NULL, NULL, NULL);

        // phy (250, 250) -> log (25,25)
        {
            SDL_WarpMouseInWindow(pico_win, 250, 253);
            SDL_PumpEvents();
            Pico_Rel_Pos pos = { '!' };
            pico_get_mouse(&pos, PICO_EVENT_MOUSE_BUTTON_NONE);
            assert(pos.x==25 && pos.y==25.599998f);
        }

        // phy (0,0) -> log (-25,-25)
        {
            SDL_WarpMouseInWindow(pico_win, 0, 0);
            SDL_PumpEvents();
            Pico_Rel_Pos pos = { '!' };
            pico_get_mouse(&pos, PICO_EVENT_MOUSE_BUTTON_NONE);
            assert(pos.x==-25 && pos.y==-25);
        }

        // phy (500,500) -> log (75,75)
        {
            SDL_WarpMouseInWindow(pico_win, 495, 499);
            SDL_PumpEvents();
            Pico_Rel_Pos pos = { '!' };
            pico_get_mouse(&pos, PICO_EVENT_MOUSE_BUTTON_NONE);
            assert(pos.x==74 && pos.y==74.800003f);
        }
    }

    // Zoom in: src = {20, 20, 10, 10}
    // centered 10x10 logical
    puts("zoom in 5x");
    {
        pico_set_view(-1, NULL, NULL, NULL,
            &(Pico_Rel_Rect){ '!', {20, 20, 10, 10}, PICO_ANCHOR_NW, NULL },
            NULL, NULL, NULL);

        // phy (0,0) -> log (20,20)
        {
            SDL_WarpMouseInWindow(pico_win, 1, 2);
            SDL_PumpEvents();
            Pico_Rel_Pos pos = { '!' };
            pico_get_mouse(&pos, PICO_EVENT_MOUSE_BUTTON_NONE);
            assert(pos.x==20.02f && pos.y==20.040001f);
        }

        // phy center (250,250) -> log (25,25)
        {
            SDL_WarpMouseInWindow(pico_win, 254, 251);
            SDL_PumpEvents();
            Pico_Rel_Pos pos = { '!' };
            pico_get_mouse(&pos, PICO_EVENT_MOUSE_BUTTON_NONE);
            assert(pos.x==25.08f && pos.y==25.02f);
        }

        // phy (500,500) -> log (30,30)
        {
            SDL_WarpMouseInWindow(pico_win, 497, 498);
            SDL_PumpEvents();
            Pico_Rel_Pos pos = { '!' };
            pico_get_mouse(&pos, PICO_EVENT_MOUSE_BUTTON_NONE);
            assert(pos.x==29.940001f && pos.y==29.959999f);
        }
    }

    // Scroll/Zoom in: src = {25, 25, 25, 25}
    // bottom-right (SE) half
    puts("scroll/zoom SE");
    {
        pico_set_view(-1, NULL, NULL, NULL,
            &(Pico_Rel_Rect){ '!', {25, 25, 25, 25}, PICO_ANCHOR_NW, NULL },
            NULL, NULL, NULL);

        // phy (0,0) -> log (25,25)
        {
            SDL_WarpMouseInWindow(pico_win, 2, 1);
            SDL_PumpEvents();
            Pico_Rel_Pos pos = { '!' };
            pico_get_mouse(&pos, PICO_EVENT_MOUSE_BUTTON_NONE);
            assert(pos.x==25.1f && pos.y==25.049999f);
        }

        // phy (250,250) -> log (37,37)
        {
            SDL_WarpMouseInWindow(pico_win, 253, 250);
            SDL_PumpEvents();
            Pico_Rel_Pos pos = { '!' };
            pico_get_mouse(&pos, PICO_EVENT_MOUSE_BUTTON_NONE);
            assert(pos.x==37.650002f && pos.y==37.5f);
        }
    }

    puts("normal PCT");
    {
        pico_set_view(-1, NULL, NULL, NULL,
            &(Pico_Rel_Rect){ '!', {0, 0, 50, 50}, PICO_ANCHOR_NW, NULL },
            NULL, NULL, NULL);

        // phy (250,250) -> pct (0.5,0.5)
        {
            SDL_WarpMouseInWindow(pico_win, 250, 250);
            SDL_PumpEvents();
            Pico_Rel_Pos pos = { '%', {0, 0}, PICO_ANCHOR_NW, NULL };
            pico_get_mouse(&pos, PICO_EVENT_MOUSE_BUTTON_NONE);
            assert(pos.x==0.5 && pos.y==0.5);
        }

        // phy (0,0) -> pct (0,0)
        {
            SDL_WarpMouseInWindow(pico_win, 0, 0);
            SDL_PumpEvents();
            Pico_Rel_Pos pos = { '%', {0, 0}, PICO_ANCHOR_C, NULL };
            pico_get_mouse(&pos, PICO_EVENT_MOUSE_BUTTON_NONE);
            assert(pos.x==0.0 && pos.y==0.0);
        }
    }

    puts("zoom PCT");
    {
        // Zoom with src = {20, 20, 10, 10}, but pct should still be 0-1 relative
        // to full logical world
        pico_set_view(-1, NULL, NULL, NULL,
            &(Pico_Rel_Rect){ '!', {20, 20, 10, 10}, PICO_ANCHOR_NW, NULL },
            NULL, NULL, NULL);

        // phy (0,0) -> raw (20,20) -> pct (0.4,0.4)
        {
            SDL_WarpMouseInWindow(pico_win, 0, 0);
            SDL_PumpEvents();
            Pico_Rel_Pos pos = { '%', {0, 0}, PICO_ANCHOR_NW, NULL };
            pico_get_mouse(&pos, PICO_EVENT_MOUSE_BUTTON_NONE);
            assert(pos.x>0.39 && pos.x<0.41);
            assert(pos.y>0.39 && pos.y<0.41);
        }

        // phy (250,250) -> raw (25,25) -> pct (0.5,0.5)
        {
            SDL_WarpMouseInWindow(pico_win, 250, 250);
            SDL_PumpEvents();
            Pico_Rel_Pos pos = { '%', {0, 0}, PICO_ANCHOR_C, NULL };
            pico_get_mouse(&pos, PICO_EVENT_MOUSE_BUTTON_NONE);
            assert(pos.x>0.49 && pos.x<0.51);
            assert(pos.y>0.49 && pos.y<0.51);
        }
    }

    pico_init(0);
    return 0;
}
