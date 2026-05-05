#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);

    Pico_Rel_Dim phy = { '!', {480, 480}, NULL };
    pico_set_dim(&phy);
    pico_set_effect_grid("root", 0);

    // Layer "map": 20x15 cols/rows of 16x16 tiles -> 320x240 px texture.
    // view.tile must be set at birth so '#'-mode works without pico_set_scene.
    {
        puts("layer map: 20x15 tiles of 16x16");
        Pico_Abs_Dim tile = { 16, 16 };
        pico_layer_empty(NULL, "map", (Pico_Abs_Dim){20, 15}, &tile);
        pico_set_effect_grid("map", 1);
        pico_output_clear("map");
        pico_set_pencil_color("map", PICO_COLOR_RED);
        pico_output_draw_rect("map", &(Pico_Rel_Rect){ '#', {1, 1, 1, 1}, PICO_ANCHOR_NW, NULL });
        pico_set_pencil_color("map", PICO_COLOR_GREEN);
        pico_output_draw_rect("map", &(Pico_Rel_Rect){ '#', {6, 4, 4, 2}, PICO_ANCHOR_NW, NULL });
        pico_set_pencil_color("map", PICO_COLOR_BLUE);
        pico_output_draw_rect("map", &(Pico_Rel_Rect){ '#', {20, 15, 1, 1}, PICO_ANCHOR_NW, NULL });
    }

    // Test 1: draw layer "map" 1:1 on main, confirm tile-aligned content
    {
        puts("draw map 1:1");
        pico_output_clear("root");
        pico_output_draw_layer("root", "map", &(Pico_Rel_Rect){ '!', {0, 0, 320, 240}, PICO_ANCHOR_NW, NULL });
        _pico_check("layer-empty-tile-01");
    }

    // Test 2: draw layer "map" scaled up, grid still aligned visually
    {
        puts("draw map scaled");
        pico_output_clear("root");
        pico_output_draw_layer("root", "map", &(Pico_Rel_Rect){ '!', {0, 0, 480, 360}, PICO_ANCHOR_NW, NULL });
        _pico_check("layer-empty-tile-02");
    }

    // Layer "fx": tile == NULL -> dim is pixels, view.tile stays {0,0}
    {
        puts("layer fx: plain pixel layer (tile=NULL)");
        pico_layer_empty(NULL, "fx", (Pico_Abs_Dim){64, 64}, NULL);
        pico_output_clear("fx");
        pico_set_pencil_color("fx", PICO_COLOR_YELLOW);
        pico_output_draw_rect("fx", &(Pico_Rel_Rect){ '!', {8, 8, 48, 48}, PICO_ANCHOR_NW, NULL });
    }

    // Test 3: draw both layers, confirm tile-mode and pixel-mode coexist
    {
        puts("draw map + fx");
        pico_output_clear("root");
        pico_output_draw_layer("root", "map", &(Pico_Rel_Rect){ '!', {0,   0, 320, 240}, PICO_ANCHOR_NW, NULL });
        pico_output_draw_layer("root", "fx",  &(Pico_Rel_Rect){ '!', {340, 0, 128, 128}, PICO_ANCHOR_NW, NULL });
        _pico_check("layer-empty-tile-03");
    }

    pico_init(0);
    return 0;
}
