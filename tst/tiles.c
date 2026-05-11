#include "pico.h"
#include "../check.h"

// helper: window-pixel mouse set (NW anchor)
static void mouse_w (int x, int y) {
    const char* old = pico_set_layer("window");
    pico_set_mouse(&(Pico_Rel_Pos){'!', {x, y}, PICO_ANCHOR_NW});
    pico_set_layer(old);
}

int main (void) {
    pico_init(1);

    // 4x4 grid of 4x4 pixel tiles = 16x16 logical world
    Pico_Rel_Dim log  = { '#', {4, 4} };
    Pico_Abs_Dim tile = { 4, 4 };
    pico_set_window((Pico_Window){ .fs=0, .show=1, .title="Tile" });

    pico_set_layer("window");
    pico_set_scene_dim(&(Pico_Rel_Dim){ '!', {160, 160} });
    pico_set_layer("world");
    pico_set_scene_tile(tile); pico_set_scene_dim(&log);

    // 4x4 pixel white tile
    Pico_Color white[16];
    for (int i = 0; i < 16; i++) {
        white[i] = (Pico_Color){ 0xFF, 0xFF, 0xFF, 0xFF };
    }

    // Test 1: tile (1,1) with NW anchor - top-left corner
    {
        puts("tile (1,1) NW anchor");
        pico_output_clear();
        Pico_Rel_Rect r = { '#', {1, 1, 1, 1}, PICO_ANCHOR_NW };
        pico_output_draw_pixmap("tile", (Pico_Abs_Dim){4,4}, white, &r);
        _pico_check("tiles-01");
    }

    // Test 2: tile (2,2) with C anchor - centered in tile
    {
        puts("tile (2,2) C anchor");
        pico_output_clear();
        Pico_Rel_Rect r = { '#', {2, 2, 1, 1}, PICO_ANCHOR_C };
        pico_output_draw_pixmap("tile", (Pico_Abs_Dim){4,4}, white, &r);
        _pico_check("tiles-02");
    }

    // Test 3: 2x2 tiles at (1,1) with NW anchor
    {
        puts("2x2 tiles (1,1) NW anchor");
        pico_output_clear();
        Pico_Rel_Rect r = { '#', {1, 1, 2, 2}, PICO_ANCHOR_NW };
        pico_output_draw_pixmap("tile", (Pico_Abs_Dim){4,4}, white, &r);
        _pico_check("tiles-03");
    }

    // Test 4: 2x2 tiles centered at (2.5, 2.5)
    {
        puts("2x2 tiles (2.5,2.5) C anchor");
        pico_output_clear();
        Pico_Rel_Rect r = { '#', {2.5, 2.5, 2, 2}, PICO_ANCHOR_C };
        pico_output_draw_pixmap("tile", (Pico_Abs_Dim){4,4}, white, &r);
        _pico_check("tiles-04");
    }

    // Test 5: mouse position in tile mode
    // phy (0,0) -> log (0,0) -> tile (1,1)
    {
        puts("mouse tile (1,1)");
        mouse_w(0, 0);
        Pico_Mouse pos = pico_get_mouse('#', NULL);
        assert(pos.x==1 && pos.y==1);
    }

    // phy (40,40) -> log (4,4) -> tile (2,2)
    // 160 phy / 16 log = 10x scale, so phy 40 = log 4
    {
        puts("mouse tile (2,2)");
        mouse_w(40, 40);
        Pico_Mouse pos = pico_get_mouse('#', NULL);
        assert(pos.x==2 && pos.y==2);
    }

    // phy (80,120) -> log (8,12) -> tile (3,4)
    {
        puts("mouse tile (3,4)");
        mouse_w(80, 120);
        Pico_Mouse pos = pico_get_mouse('#', NULL);
        assert(pos.x==3 && pos.y==4);
    }

    pico_init(0);
    return 0;
}
