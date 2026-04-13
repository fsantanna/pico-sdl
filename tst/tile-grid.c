#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);

    Pico_Rel_Dim phy = { '!', {480, 480}, NULL };
    pico_set_window("Grid", -1, &phy);

    // Test 1: main layer, no tile mode, pixel grid only
    {
        puts("pixel grid only (no tile)");
        Pico_Rel_Dim log = { '!', {16, 16}, NULL };
        pico_set_show_grid(NULL, 1); pico_set_view_dim(NULL, &log);
        pico_output_clear();
        pico_set_draw_color(NULL, PICO_COLOR_RED);
        pico_output_draw_rect(&(Pico_Rel_Rect){ '!', {4, 4, 8, 8}, PICO_ANCHOR_NW, NULL });
        _pico_check("tile-grid-01");
    }

    // Test 2: main layer, 4x4 tiles of 4x4 pixels, pixel + tile grid
    {
        puts("pixel + tile grid (4x4 tiles)");
        Pico_Rel_Dim log = { '#', {4, 4}, NULL };
        Pico_Abs_Dim tile = { 4, 4 };
        pico_set_show_grid(NULL, 1); pico_set_view_tile(NULL, tile); pico_set_view_dim(NULL, &log);
        pico_output_clear();
        pico_set_draw_color(NULL, PICO_COLOR_GREEN);
        pico_output_draw_rect(&(Pico_Rel_Rect){ '#', {1, 1, 2, 2}, PICO_ANCHOR_NW, NULL });
        _pico_check("tile-grid-02");
    }

    // Test 3: main layer, 8x8 tiles of 2x2 pixels, pixel + tile grid
    {
        puts("pixel + tile grid (8x8 tiles)");
        Pico_Rel_Dim log = { '#', {8, 8}, NULL };
        Pico_Abs_Dim tile = { 2, 2 };
        pico_set_show_grid(NULL, 1); pico_set_view_tile(NULL, tile); pico_set_view_dim(NULL, &log);
        pico_output_clear();
        pico_set_draw_color(NULL, PICO_COLOR_BLUE);
        pico_output_draw_rect(&(Pico_Rel_Rect){ '#', {3, 3, 2, 2}, PICO_ANCHOR_NW, NULL });
        _pico_check("tile-grid-03");
    }

    // Test 4: main layer, non-square tiles (4x2), pixel + tile grid
    {
        puts("pixel + tile grid (non-square 4x2 tiles)");
        Pico_Rel_Dim log = { '#', {4, 8}, NULL };
        Pico_Abs_Dim tile = { 4, 2 };
        pico_set_show_grid(NULL, 1); pico_set_view_tile(NULL, tile); pico_set_view_dim(NULL, &log);
        pico_output_clear();
        pico_set_draw_color(NULL, PICO_COLOR_YELLOW);
        pico_output_draw_rect(&(Pico_Rel_Rect){ '#', {2, 4, 1, 2}, PICO_ANCHOR_NW, NULL });
        _pico_check("tile-grid-04");
    }

    // Test 5: main layer, grid disabled, no grid lines
    {
        puts("grid disabled");
        Pico_Rel_Dim log = { '#', {4, 4}, NULL };
        Pico_Abs_Dim tile = { 4, 4 };
        pico_set_show_grid(NULL, 0); pico_set_view_tile(NULL, tile); pico_set_view_dim(NULL, &log);
        pico_output_clear();
        pico_set_draw_color(NULL, PICO_COLOR_RED);
        pico_output_draw_rect(&(Pico_Rel_Rect){ '#', {2, 2, 1, 1}, PICO_ANCHOR_C, NULL });
        _pico_check("tile-grid-05");
    }

    // Reset main to simple view for layer tests, grid off
    {
        Pico_Rel_Dim log = { '!', {480, 480}, NULL };
        pico_set_show_grid(NULL, 0); pico_set_view_dim(NULL, &log);
    }

    // Layer A: 4x4 tiles of 4x4 pixels, grid enabled
    {
        puts("layer A: 4x4 tiles, grid on");
        pico_layer_empty(NULL, "layerA", (Pico_Abs_Dim){16, 16}, NULL);
        pico_set_layer("layerA");
        Pico_Rel_Dim log = { '#', {4, 4}, NULL };
        Pico_Abs_Dim tile = { 4, 4 };
        pico_set_show_grid(NULL, 1); pico_set_view_tile(NULL, tile); pico_set_view_dim(NULL, &log);
        pico_output_clear();
        pico_set_draw_color(NULL, PICO_COLOR_RED);
        pico_output_draw_rect(&(Pico_Rel_Rect){ '#', {1, 1, 2, 2}, PICO_ANCHOR_NW, NULL });
        pico_set_layer(NULL);
    }

    // Layer B: 8x4 tiles of 2x4 pixels, grid enabled
    {
        puts("layer B: 8x4 non-square tiles, grid on");
        pico_layer_empty(NULL, "layerB", (Pico_Abs_Dim){16, 16}, NULL);
        pico_set_layer("layerB");
        Pico_Rel_Dim log = { '#', {8, 4}, NULL };
        Pico_Abs_Dim tile = { 2, 4 };
        pico_set_show_grid(NULL, 1); pico_set_view_tile(NULL, tile); pico_set_view_dim(NULL, &log);
        pico_output_clear();
        pico_set_draw_color(NULL, PICO_COLOR_GREEN);
        pico_output_draw_rect(&(Pico_Rel_Rect){ '#', {5, 2, 2, 1}, PICO_ANCHOR_NW, NULL });
        pico_set_layer(NULL);
    }

    // Layer C: 4x4 tiles of 4x4 pixels, grid disabled
    {
        puts("layer C: 4x4 tiles, grid off");
        pico_layer_empty(NULL, "layerC", (Pico_Abs_Dim){16, 16}, NULL);
        pico_set_layer("layerC");
        Pico_Rel_Dim log = { '#', {4, 4}, NULL };
        Pico_Abs_Dim tile = { 4, 4 };
        pico_set_show_grid(NULL, 0); pico_set_view_tile(NULL, tile); pico_set_view_dim(NULL, &log);
        pico_output_clear();
        pico_set_draw_color(NULL, PICO_COLOR_BLUE);
        pico_output_draw_rect(&(Pico_Rel_Rect){ '#', {3, 3, 1, 1}, PICO_ANCHOR_NW, NULL });
        pico_set_layer(NULL);
    }

    // Test 6: draw all 3 layers side by side on main
    {
        puts("3 layers: A(grid on) B(grid on) C(grid off)");
        pico_output_clear();
        pico_output_draw_layer("layerA", &(Pico_Rel_Rect){ '!', {0,   0, 150, 150}, PICO_ANCHOR_NW, NULL });
        pico_output_draw_layer("layerB", &(Pico_Rel_Rect){ '!', {160, 0, 150, 150}, PICO_ANCHOR_NW, NULL });
        pico_output_draw_layer("layerC", &(Pico_Rel_Rect){ '!', {320, 0, 150, 150}, PICO_ANCHOR_NW, NULL });
        _pico_check("tile-grid-06");
    }

    // Test 7: layers at different scales
    {
        puts("3 layers: different scales");
        pico_output_clear();
        pico_output_draw_layer("layerA", &(Pico_Rel_Rect){ '!', {0,   170, 120, 120}, PICO_ANCHOR_NW, NULL });
        pico_output_draw_layer("layerB", &(Pico_Rel_Rect){ '!', {140, 170, 200, 200}, PICO_ANCHOR_NW, NULL });
        pico_output_draw_layer("layerC", &(Pico_Rel_Rect){ '!', {0,   310, 240, 120}, PICO_ANCHOR_NW, NULL });
        _pico_check("tile-grid-07");
    }

    pico_init(0);
    return 0;
}
