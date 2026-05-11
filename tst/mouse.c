#include "pico.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

// helper: window-pixel mouse set (NW anchor)
static void mouse_w (int x, int y) {
    const char* old = pico_set_layer("window");
    pico_set_mouse(&(Pico_Rel_Pos){'!', {x, y}, PICO_ANCHOR_NW});
    pico_set_layer(old);
}

int main(void) {
    pico_init(1);

    puts("phy (500,500) -> log (50,50)");
    {
        pico_set_window((Pico_Window){ .fs=0, .show=1, .title="Mouse" });

        pico_set_layer("window");
        pico_set_scene_dim(&(Pico_Rel_Dim){ '!', {500, 500} });
        pico_set_layer("world");
        pico_set_scene_dim(&(Pico_Rel_Dim){ '!', {50, 50} });

        // phy (0,0) -> log (0,0)
        {
            mouse_w(0, 3);
            Pico_Mouse pos = pico_get_mouse('!', NULL);
            assert(pos.x==0 && pos.y==0.3f);
        }

        // phy (250,250) -> log (25,25)
        {
            mouse_w(250, 251);
            Pico_Mouse pos = pico_get_mouse('!', NULL);
            assert(pos.x==25 && pos.y==25.099998f);
        }

        // phy (490,490) -> log (49,49)
        {
            mouse_w(499, 490);
            Pico_Mouse pos = pico_get_mouse('!', NULL);
            assert(pos.x==49.900002f && pos.y==49);
        }
    }

    // Zoom out: src = {-25, -25, 100, 100}
    // centered 100x100 logical
    puts("zoom out 2x");
    {
        pico_set_scene_src((Pico_Rel_Rect){ '!', {-25, -25, 100, 100}, PICO_ANCHOR_NW });

        // phy (250, 250) -> log (25,25)
        {
            mouse_w(250, 253);
            Pico_Mouse pos = pico_get_mouse('!', NULL);
            assert(pos.x==25 && pos.y==25.599998f);
        }

        // phy (0,0) -> log (-25,-25)
        {
            mouse_w(0, 0);
            Pico_Mouse pos = pico_get_mouse('!', NULL);
            assert(pos.x==-25 && pos.y==-25);
        }

        // phy (500,500) -> log (75,75)
        {
            mouse_w(495, 499);
            Pico_Mouse pos = pico_get_mouse('!', NULL);
            assert(pos.x==74 && pos.y==74.800003f);
        }
    }

    // Zoom in: src = {20, 20, 10, 10}
    // centered 10x10 logical
    puts("zoom in 5x");
    {
        pico_set_scene_src((Pico_Rel_Rect){ '!', {20, 20, 10, 10}, PICO_ANCHOR_NW });

        // phy (0,0) -> log (20,20)
        {
            mouse_w(1, 2);
            Pico_Mouse pos = pico_get_mouse('!', NULL);
            assert(pos.x==20.02f && pos.y==20.040001f);
        }

        // phy center (250,250) -> log (25,25)
        {
            mouse_w(254, 251);
            Pico_Mouse pos = pico_get_mouse('!', NULL);
            assert(pos.x==25.08f && pos.y==25.02f);
        }

        // phy (500,500) -> log (30,30)
        {
            mouse_w(497, 498);
            Pico_Mouse pos = pico_get_mouse('!', NULL);
            assert(pos.x==29.940001f && pos.y==29.959999f);
        }
    }

    // Scroll/Zoom in: src = {25, 25, 25, 25}
    // bottom-right (SE) half
    puts("scroll/zoom SE");
    {
        pico_set_scene_src((Pico_Rel_Rect){ '!', {25, 25, 25, 25}, PICO_ANCHOR_NW });

        // phy (0,0) -> log (25,25)
        {
            mouse_w(2, 1);
            Pico_Mouse pos = pico_get_mouse('!', NULL);
            assert(pos.x==25.1f && pos.y==25.049999f);
        }

        // phy (250,250) -> log (37,37)
        {
            mouse_w(253, 250);
            Pico_Mouse pos = pico_get_mouse('!', NULL);
            assert(pos.x==37.650002f && pos.y==37.5f);
        }
    }

    puts("normal PCT");
    {
        pico_set_scene_src((Pico_Rel_Rect){ '!', {0, 0, 50, 50}, PICO_ANCHOR_NW });

        // phy (250,250) -> pct (0.5,0.5)
        {
            mouse_w(250, 250);
            Pico_Mouse pos = pico_get_mouse('%', NULL);
            assert(pos.x==0.5 && pos.y==0.5);
        }

        // phy (0,0) -> pct (0,0)
        {
            mouse_w(0, 0);
            Pico_Mouse pos = pico_get_mouse('%', NULL);
            assert(pos.x==0.0 && pos.y==0.0);
        }
    }

    puts("zoom PCT");
    {
        // Zoom with src = {20, 20, 10, 10}, but pct should still be 0-1 relative
        // to full logical world
        pico_set_scene_src((Pico_Rel_Rect){ '!', {20, 20, 10, 10}, PICO_ANCHOR_NW });

        // phy (0,0) -> raw (20,20) -> pct (0.4,0.4)
        {
            mouse_w(0, 0);
            Pico_Mouse pos = pico_get_mouse('%', NULL);
            assert(pos.x>0.39 && pos.x<0.41);
            assert(pos.y>0.39 && pos.y<0.41);
        }

        // phy (250,250) -> raw (25,25) -> pct (0.5,0.5)
        {
            mouse_w(250, 250);
            Pico_Mouse pos = pico_get_mouse('%', NULL);
            assert(pos.x>0.49 && pos.x<0.51);
            assert(pos.y>0.49 && pos.y<0.51);
        }
    }

    puts("window mode (raw phy)");
    {
        mouse_w(123, 456);
        const char* old = pico_set_layer("window");
        Pico_Mouse pos = pico_get_mouse('!', NULL);
        pico_set_layer(old);
        assert(pos.x==123 && pos.y==456);
    }

    // roundtrip: set(rel) -> get(rel) -> assert equal
    puts("roundtrip '!'");
    {
        pico_set_scene_src((Pico_Rel_Rect){ '!', {0, 0, 50, 50}, PICO_ANCHOR_NW });
        pico_set_mouse (
            &(Pico_Rel_Pos){ '!', {25, 25}, PICO_ANCHOR_NW }
        );
        Pico_Mouse pos = pico_get_mouse('!', NULL);
        assert(pos.x==25 && pos.y==25);
    }

    puts("roundtrip '%'");
    {
        pico_set_mouse (
            &(Pico_Rel_Pos){ '%', {0.5, 0.5}, PICO_ANCHOR_NW }
        );
        Pico_Mouse pos = pico_get_mouse('%', NULL);
        assert(pos.x>0.49 && pos.x<0.51);
        assert(pos.y>0.49 && pos.y<0.51);
    }

    puts("roundtrip '!' zoomed");
    {
        pico_set_scene_src((Pico_Rel_Rect){ '!', {20, 20, 10, 10}, PICO_ANCHOR_NW });
        pico_set_mouse (
            &(Pico_Rel_Pos){ '!', {25, 25}, PICO_ANCHOR_NW }
        );
        Pico_Mouse pos = pico_get_mouse('!', NULL);
        assert(pos.x==25 && pos.y==25);
    }

    pico_init(0);
    return 0;
}
