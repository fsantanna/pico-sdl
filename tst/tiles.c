#include "pico.h"
#include "../check.h"
#include <SDL2/SDL.h>

extern SDL_Window* pico_win;

int main (void) {
    pico_init(1);

    // 4x4 grid of 4x4 pixel tiles = 16x16 logical world
    Pico_Rel_Dim phy  = { '!', {160, 160}, NULL };
    Pico_Rel_Dim log  = { '#', {4, 4}, NULL };
    Pico_Abs_Dim tile = { 4, 4 };
    pico_set_window("Tile", -1, &phy);
    pico_set_view(-1, &log, &tile, NULL, NULL, NULL, NULL, NULL);

    // 4x4 pixel white tile
    Pico_Color_A white[16];
    for (int i = 0; i < 16; i++) {
        white[i] = (Pico_Color_A){ 0xFF, 0xFF, 0xFF, 0xFF };
    }

    // Test 1: tile (1,1) with NW anchor - top-left corner
    {
        puts("tile (1,1) NW anchor");
        pico_output_clear();
        Pico_Rel_Rect r = { '#', {1, 1, 1, 1}, PICO_ANCHOR_NW, NULL };
        pico_output_draw_buffer("tile", (Pico_Abs_Dim){4,4}, white, &r);
        _pico_check("tiles-01");
    }

    // Test 2: tile (2,2) with C anchor - centered in tile
    {
        puts("tile (2,2) C anchor");
        pico_output_clear();
        Pico_Rel_Rect r = { '#', {2, 2, 1, 1}, PICO_ANCHOR_C, NULL };
        pico_output_draw_buffer("tile", (Pico_Abs_Dim){4,4}, white, &r);
        _pico_check("tiles-02");
    }

    // Test 3: 2x2 tiles at (1,1) with NW anchor
    {
        puts("2x2 tiles (1,1) NW anchor");
        pico_output_clear();
        Pico_Rel_Rect r = { '#', {1, 1, 2, 2}, PICO_ANCHOR_NW, NULL };
        pico_output_draw_buffer("tile", (Pico_Abs_Dim){4,4}, white, &r);
        _pico_check("tiles-03");
    }

    // Test 4: 2x2 tiles centered at (2.5, 2.5)
    {
        puts("2x2 tiles (2.5,2.5) C anchor");
        pico_output_clear();
        Pico_Rel_Rect r = { '#', {2.5, 2.5, 2, 2}, PICO_ANCHOR_C, NULL };
        pico_output_draw_buffer("tile", (Pico_Abs_Dim){4,4}, white, &r);
        _pico_check("tiles-04");
    }

    // Test 5: mouse position in tile mode
    // phy (0,0) -> log (0,0) -> tile (1,1)
    {
        puts("mouse tile (1,1)");
        SDL_WarpMouseInWindow(pico_win, 0, 0);
        SDL_PumpEvents();
        Pico_Rel_Pos pos = { '#' };
        pico_get_mouse(&pos, PICO_EVENT_MOUSE_BUTTON_NONE);
        assert(pos.x==1 && pos.y==1);
    }

    // phy (40,40) -> log (4,4) -> tile (2,2)
    // 160 phy / 16 log = 10x scale, so phy 40 = log 4
    {
        puts("mouse tile (2,2)");
        SDL_WarpMouseInWindow(pico_win, 40, 40);
        SDL_PumpEvents();
        Pico_Rel_Pos pos = { '#' };
        pico_get_mouse(&pos, PICO_EVENT_MOUSE_BUTTON_NONE);
        assert(pos.x==2 && pos.y==2);
    }

    // phy (80,120) -> log (8,12) -> tile (3,4)
    {
        puts("mouse tile (3,4)");
        SDL_WarpMouseInWindow(pico_win, 80, 120);
        SDL_PumpEvents();
        Pico_Rel_Pos pos = { '#' };
        pico_get_mouse(&pos, PICO_EVENT_MOUSE_BUTTON_NONE);
        assert(pos.x==3 && pos.y==4);
    }

    pico_init(0);
    return 0;
}
