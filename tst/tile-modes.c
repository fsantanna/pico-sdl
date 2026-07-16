#include <assert.h>
#include <stdio.h>

#include "pico.h"
#include "../check.h"

// §7 (dim, tile) joint resolution: assert texture dim (D) and stored
// tile (T). Behavioral test (no image); passes when all asserts hold.

static void check (const char* key, Pico_Abs_Dim D, Pico_Abs_Dim T) {
    const char* old = pico_set_layer(key);
    Pico_Abs_Dim d = pico_get_scene_dim();
    Pico_Abs_Dim t = pico_get_scene_tile();
    printf("%s: dim=%dx%d tile=%dx%d\n", key, d.w, d.h, t.w, t.h);
    assert(d.w==D.w && d.h==D.h && "dim mismatch");
    assert(t.w==T.w && t.h==T.h && "tile mismatch");
    pico_set_layer(old);
}

int main (void) {
    pico_init(1);

    // known parent frame for '%' cases: world = 100x100
    pico_set_dim((Pico_Rel_Dim){ '!', {100, 100} });

    // G1: tile='#' derives T = floor(D / t)
    // dim='!' 100x100, tile='#' 10x10 -> T=10x10
    {
        puts("G1: dim='!' + tile='#'");
        Pico_Rel_Dim tile = { '#', {10, 10} };
        pico_layer_empty("world", "g1", 1, (Pico_Rel_Dim){'!', {100, 100}}, &tile);
        check("g1", (Pico_Abs_Dim){100, 100}, (Pico_Abs_Dim){10, 10});
    }

    // G2: tile='%' derives T = floor(t% * D)  ('%' is 0.0-1.0)
    // dim='!' 100x100, tile='%' .1x.1 -> T=10x10
    {
        puts("G2: dim='!' + tile='%'");
        Pico_Rel_Dim tile = { '%', {0.1, 0.1} };
        pico_layer_empty("world", "g2", 1, (Pico_Rel_Dim){'!', {100, 100}}, &tile);
        check("g2", (Pico_Abs_Dim){100, 100}, (Pico_Abs_Dim){10, 10});
    }

    // G3: dim='%' of parent + tile='!'
    // parent world=100x100; dim='%' .5 -> D=50x50; tile abs 5x5
    {
        puts("G3: dim='%' + tile='!'");
        Pico_Rel_Dim tile = { '!', {5, 5} };
        pico_layer_empty("world", "g3", 1, (Pico_Rel_Dim){'%', {0.5, 0.5}}, &tile);
        check("g3", (Pico_Abs_Dim){50, 50}, (Pico_Abs_Dim){5, 5});
    }

    // G4: non-divisible -> floor + clip (5.5)
    // dim='!' 100x100, tile='#' 3x3 -> T=floor(100/3)=33x33
    {
        puts("G4: floor (dim%tile != 0)");
        Pico_Rel_Dim tile = { '#', {3, 3} };
        pico_layer_empty("world", "g4", 1, (Pico_Rel_Dim){'!', {100, 100}}, &tile);
        check("g4", (Pico_Abs_Dim){100, 100}, (Pico_Abs_Dim){33, 33});
    }

    // G5: canonical tiled layer, dim='#' cells + tile='!'
    // dim='#' 20x15, tile='!' 16x16 -> D=320x240, T=16x16
    {
        puts("G5: dim='#' + tile='!'");
        Pico_Rel_Dim tile = { '!', {16, 16} };
        pico_layer_empty("world", "g5", 1, (Pico_Rel_Dim){'#', {20, 15}}, &tile);
        check("g5", (Pico_Abs_Dim){320, 240}, (Pico_Abs_Dim){16, 16});
    }

    // G6: set.scene joint (dim='#' + tile='!') on an existing layer
    // dim='#' 8x8, tile='!' 4x4 -> D=32x32, T=4x4
    {
        puts("G6: set.scene joint dim+tile");
        pico_layer_empty("world", "g6", 1, (Pico_Rel_Dim){'!', {10, 10}}, NULL);
        pico_set_layer("g6");
        Pico_Rel_Dim dim  = { '#', {8, 8} };
        Pico_Rel_Dim tile = { '!', {4, 4} };
        pico_set_scene_dim_tile(&dim, &tile);
        pico_set_layer("world");
        check("g6", (Pico_Abs_Dim){32, 32}, (Pico_Abs_Dim){4, 4});
    }

    // G7: set.scene tile-only, resolves against current scene.dim
    // from G6 state (dim 32x32); tile='#' 2x2 -> T=floor(32/2)=16x16
    {
        puts("G7: set.scene tile-only vs current dim");
        pico_set_layer("g6");
        Pico_Rel_Dim tile = { '#', {2, 2} };
        pico_set_scene_dim_tile(NULL, &tile);
        pico_set_layer("world");
        check("g6", (Pico_Abs_Dim){32, 32}, (Pico_Abs_Dim){16, 16});
    }

    puts("ALL OK");
    pico_init(0);
    return 0;
}
