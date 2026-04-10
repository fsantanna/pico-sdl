#include "pico.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

int main(void) {
    pico_init(1);

    puts("phy (500,500) -> log (50,50)");
    {
        pico_set_window("Mouse", -1, &(Pico_Rel_Dim){ '!', {500, 500}, NULL });
        pico_set_view(-1, &(Pico_Rel_Dim){ '!', {50, 50}, NULL }, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

        // phy (0,0) -> log (0,0)
        {
            pico_set_mouse(&(Pico_Rel_Pos){ 'w', {0, 3}, PICO_ANCHOR_NW, NULL });
            Pico_Mouse pos = pico_get_mouse('!', NULL);
            assert(pos.x==0 && pos.y==0.3f);
        }

        // phy (250,250) -> log (25,25)
        {
            pico_set_mouse(&(Pico_Rel_Pos){ 'w', {250, 251}, PICO_ANCHOR_NW, NULL });
            Pico_Mouse pos = pico_get_mouse('!', NULL);
            assert(pos.x==25 && pos.y==25.099998f);
        }

        // phy (490,490) -> log (49,49)
        {
            pico_set_mouse(&(Pico_Rel_Pos){ 'w', {499, 490}, PICO_ANCHOR_NW, NULL });
            Pico_Mouse pos = pico_get_mouse('!', NULL);
            assert(pos.x==49.900002f && pos.y==49);
        }
    }

    // Zoom out: src = {-25, -25, 100, 100}
    // centered 100x100 logical
    puts("zoom out 2x");
    {
        pico_set_view(-1, NULL, NULL, NULL,
            &(Pico_Rel_Rect){ '!', {-25, -25, 100, 100}, PICO_ANCHOR_NW, NULL },
            NULL, NULL, NULL, NULL);

        // phy (250, 250) -> log (25,25)
        {
            pico_set_mouse(&(Pico_Rel_Pos){ 'w', {250, 253}, PICO_ANCHOR_NW, NULL });
            Pico_Mouse pos = pico_get_mouse('!', NULL);
            assert(pos.x==25 && pos.y==25.599998f);
        }

        // phy (0,0) -> log (-25,-25)
        {
            pico_set_mouse(&(Pico_Rel_Pos){ 'w', {0, 0}, PICO_ANCHOR_NW, NULL });
            Pico_Mouse pos = pico_get_mouse('!', NULL);
            assert(pos.x==-25 && pos.y==-25);
        }

        // phy (500,500) -> log (75,75)
        {
            pico_set_mouse(&(Pico_Rel_Pos){ 'w', {495, 499}, PICO_ANCHOR_NW, NULL });
            Pico_Mouse pos = pico_get_mouse('!', NULL);
            assert(pos.x==74 && pos.y==74.800003f);
        }
    }

    // Zoom in: src = {20, 20, 10, 10}
    // centered 10x10 logical
    puts("zoom in 5x");
    {
        pico_set_view(-1, NULL, NULL, NULL,
            &(Pico_Rel_Rect){ '!', {20, 20, 10, 10}, PICO_ANCHOR_NW, NULL },
            NULL, NULL, NULL, NULL);

        // phy (0,0) -> log (20,20)
        {
            pico_set_mouse(&(Pico_Rel_Pos){ 'w', {1, 2}, PICO_ANCHOR_NW, NULL });
            Pico_Mouse pos = pico_get_mouse('!', NULL);
            assert(pos.x==20.02f && pos.y==20.040001f);
        }

        // phy center (250,250) -> log (25,25)
        {
            pico_set_mouse(&(Pico_Rel_Pos){ 'w', {254, 251}, PICO_ANCHOR_NW, NULL });
            Pico_Mouse pos = pico_get_mouse('!', NULL);
            assert(pos.x==25.08f && pos.y==25.02f);
        }

        // phy (500,500) -> log (30,30)
        {
            pico_set_mouse(&(Pico_Rel_Pos){ 'w', {497, 498}, PICO_ANCHOR_NW, NULL });
            Pico_Mouse pos = pico_get_mouse('!', NULL);
            assert(pos.x==29.940001f && pos.y==29.959999f);
        }
    }

    // Scroll/Zoom in: src = {25, 25, 25, 25}
    // bottom-right (SE) half
    puts("scroll/zoom SE");
    {
        pico_set_view(-1, NULL, NULL, NULL,
            &(Pico_Rel_Rect){ '!', {25, 25, 25, 25}, PICO_ANCHOR_NW, NULL },
            NULL, NULL, NULL, NULL);

        // phy (0,0) -> log (25,25)
        {
            pico_set_mouse(&(Pico_Rel_Pos){ 'w', {2, 1}, PICO_ANCHOR_NW, NULL });
            Pico_Mouse pos = pico_get_mouse('!', NULL);
            assert(pos.x==25.1f && pos.y==25.049999f);
        }

        // phy (250,250) -> log (37,37)
        {
            pico_set_mouse(&(Pico_Rel_Pos){ 'w', {253, 250}, PICO_ANCHOR_NW, NULL });
            Pico_Mouse pos = pico_get_mouse('!', NULL);
            assert(pos.x==37.650002f && pos.y==37.5f);
        }
    }

    puts("normal PCT");
    {
        pico_set_view(-1, NULL, NULL, NULL,
            &(Pico_Rel_Rect){ '!', {0, 0, 50, 50}, PICO_ANCHOR_NW, NULL },
            NULL, NULL, NULL, NULL);

        // phy (250,250) -> pct (0.5,0.5)
        {
            pico_set_mouse(&(Pico_Rel_Pos){ 'w', {250, 250}, PICO_ANCHOR_NW, NULL });
            Pico_Mouse pos = pico_get_mouse('%', NULL);
            assert(pos.x==0.5 && pos.y==0.5);
        }

        // phy (0,0) -> pct (0,0)
        {
            pico_set_mouse(&(Pico_Rel_Pos){ 'w', {0, 0}, PICO_ANCHOR_NW, NULL });
            Pico_Mouse pos = pico_get_mouse('%', NULL);
            assert(pos.x==0.0 && pos.y==0.0);
        }
    }

    puts("zoom PCT");
    {
        // Zoom with src = {20, 20, 10, 10}, but pct should still be 0-1 relative
        // to full logical world
        pico_set_view(-1, NULL, NULL, NULL,
            &(Pico_Rel_Rect){ '!', {20, 20, 10, 10}, PICO_ANCHOR_NW, NULL },
            NULL, NULL, NULL, NULL);

        // phy (0,0) -> raw (20,20) -> pct (0.4,0.4)
        {
            pico_set_mouse(&(Pico_Rel_Pos){ 'w', {0, 0}, PICO_ANCHOR_NW, NULL });
            Pico_Mouse pos = pico_get_mouse('%', NULL);
            assert(pos.x>0.39 && pos.x<0.41);
            assert(pos.y>0.39 && pos.y<0.41);
        }

        // phy (250,250) -> raw (25,25) -> pct (0.5,0.5)
        {
            pico_set_mouse(&(Pico_Rel_Pos){ 'w', {250, 250}, PICO_ANCHOR_NW, NULL });
            Pico_Mouse pos = pico_get_mouse('%', NULL);
            assert(pos.x>0.49 && pos.x<0.51);
            assert(pos.y>0.49 && pos.y<0.51);
        }
    }

    puts("window mode (raw phy)");
    {
        pico_set_mouse(&(Pico_Rel_Pos){ 'w', {123, 456}, PICO_ANCHOR_NW, NULL });
        Pico_Mouse pos = pico_get_mouse('w', NULL);
        assert(pos.x==123 && pos.y==456);
    }

    // roundtrip: set(rel) -> get(rel) -> assert equal
    puts("roundtrip '!'");
    {
        pico_set_view (
            -1, NULL, NULL, NULL,
            &(Pico_Rel_Rect){ '!', {0, 0, 50, 50}, PICO_ANCHOR_NW, NULL },
            NULL, NULL, NULL, NULL
        );
        pico_set_mouse (
            &(Pico_Rel_Pos){ '!', {25, 25}, PICO_ANCHOR_NW, NULL }
        );
        Pico_Mouse pos = pico_get_mouse('!', NULL);
        assert(pos.x==25 && pos.y==25);
    }

    puts("roundtrip '%'");
    {
        pico_set_mouse (
            &(Pico_Rel_Pos){ '%', {0.5, 0.5}, PICO_ANCHOR_NW, NULL }
        );
        Pico_Mouse pos = pico_get_mouse('%', NULL);
        assert(pos.x>0.49 && pos.x<0.51);
        assert(pos.y>0.49 && pos.y<0.51);
    }

    puts("roundtrip '!' zoomed");
    {
        pico_set_view (
            -1, NULL, NULL, NULL,
            &(Pico_Rel_Rect){ '!', {20, 20, 10, 10}, PICO_ANCHOR_NW, NULL },
            NULL, NULL, NULL, NULL
        );
        pico_set_mouse (
            &(Pico_Rel_Pos){ '!', {25, 25}, PICO_ANCHOR_NW, NULL }
        );
        Pico_Mouse pos = pico_get_mouse('!', NULL);
        assert(pos.x==25 && pos.y==25);
    }

    pico_init(0);
    return 0;
}
