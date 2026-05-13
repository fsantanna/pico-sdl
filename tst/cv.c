#include "pico.h"
#include <stdio.h>

int main (void) {
    pico_init(1);
    pico_set_scene_tile((Pico_Abs_Dim){0, 0});
    Pico_Rel_Dim dim = { '!', {100, 100} };
    pico_set_scene_dim(&dim);

    ///////////////////////////////////////////////////////////////////////////
    // _to / _from across named layers
    ///////////////////////////////////////////////////////////////////////////

    printf("=== pos_to/_from world <-> window ===\n");
    {
        puts("(50,50) world -> window");
        Pico_Rel_Pos w   = { '!', {50, 50}, PICO_ANCHOR_NW };
        Pico_Rel_Pos win = { '!', {0, 0}, PICO_ANCHOR_NW };
        pico_cv_pos_to("window", &w, &win);
        assert((int)win.x == 250 && (int)win.y == 250);
    }
    {
        puts("(250,250) window -> world");
        Pico_Rel_Pos win = { '!', {250, 250}, PICO_ANCHOR_NW };
        Pico_Rel_Pos w   = { '!', {0, 0}, PICO_ANCHOR_NW };
        pico_cv_pos_from("window", &win, &w);
        assert((int)w.x == 50 && (int)w.y == 50);
    }
    {
        puts("round-trip world <-> window");
        Pico_Rel_Pos orig = { '!', {37, 89}, PICO_ANCHOR_NW };
        Pico_Rel_Pos win  = { '!', {0, 0}, PICO_ANCHOR_NW };
        Pico_Rel_Pos back = { '!', {0, 0}, PICO_ANCHOR_NW };
        pico_cv_pos_to("window", &orig, &win);
        pico_cv_pos_from("window", &win, &back);
        assert((int)back.x == 37 && (int)back.y == 89);
    }

    printf("=== sub-layer 2-hop walk ===\n");
    pico_layer_empty("world", "sub_cv", 1, (Pico_Rel_Dim){'!', {50, 50}}, NULL);
    pico_set_layer("sub_cv");

    {
        puts("(10,20) sub -> (20,40) world");
        Pico_Rel_Pos s = { '!', {10, 20}, PICO_ANCHOR_NW };
        Pico_Rel_Pos w = { '!', {0, 0}, PICO_ANCHOR_NW };
        pico_cv_pos_to("world", &s, &w);
        assert((int)w.x == 20 && (int)w.y == 40);
    }
    {
        puts("(20,40) world -> (10,20) sub");
        Pico_Rel_Pos w = { '!', {20, 40}, PICO_ANCHOR_NW };
        Pico_Rel_Pos s = { '!', {0, 0}, PICO_ANCHOR_NW };
        pico_cv_pos_from("world", &w, &s);
        assert((int)s.x == 10 && (int)s.y == 20);
    }
    {
        puts("(10,20) sub -> (100,200) window (2 hops)");
        Pico_Rel_Pos s   = { '!', {10, 20}, PICO_ANCHOR_NW };
        Pico_Rel_Pos win = { '!', {0, 0}, PICO_ANCHOR_NW };
        pico_cv_pos_to("window", &s, &win);
        assert((int)win.x == 100 && (int)win.y == 200);
    }
    {
        puts("(100,200) window -> (10,20) sub (2 hops)");
        Pico_Rel_Pos win = { '!', {100, 200}, PICO_ANCHOR_NW };
        Pico_Rel_Pos s   = { '!', {0, 0}, PICO_ANCHOR_NW };
        pico_cv_pos_from("window", &win, &s);
        assert((int)s.x == 10 && (int)s.y == 20);
    }

    printf("=== rect_to / rect_from ===\n");
    {
        puts("rect (10,20,5,10) sub -> (20,40,10,20) world");
        Pico_Rel_Rect s = { '!', {10, 20, 5, 10}, PICO_ANCHOR_NW };
        Pico_Rel_Rect w = { '!', {0, 0, 0, 0}, PICO_ANCHOR_NW };
        pico_cv_rect_to("world", &s, &w);
        assert((int)w.x==20 && (int)w.y==40);
        assert((int)w.w==10 && (int)w.h==20);
    }
    {
        puts("rect (20,40,10,20) world -> (10,20,5,10) sub");
        Pico_Rel_Rect w = { '!', {20, 40, 10, 20}, PICO_ANCHOR_NW };
        Pico_Rel_Rect s = { '!', {0, 0, 0, 0}, PICO_ANCHOR_NW };
        pico_cv_rect_from("world", &w, &s);
        assert((int)s.x==10 && (int)s.y==20);
        assert((int)s.w==5  && (int)s.h==10);
    }

    printf("=== dim_to / dim_from ===\n");
    {
        puts("dim (5,10) sub -> (10,20) world");
        Pico_Rel_Dim s = { '!', {5, 10} };
        Pico_Rel_Dim w = { '!', {0, 0} };
        pico_cv_dim_to("world", &s, &w);
        assert((int)w.w==10 && (int)w.h==20);
    }
    {
        puts("dim (10,20) world -> (5,10) sub");
        Pico_Rel_Dim w = { '!', {10, 20} };
        Pico_Rel_Dim s = { '!', {0, 0} };
        pico_cv_dim_from("world", &w, &s);
        assert((int)s.w==5 && (int)s.h==10);
    }

    pico_set_layer("world");

    printf("=== descendant target/source (bidirectional) ===\n");
    {
        puts("(20,40) world -> (10,20) sub (target=descendant)");
        Pico_Rel_Pos w = { '!', {20, 40}, PICO_ANCHOR_NW };
        Pico_Rel_Pos s = { '!', {0, 0}, PICO_ANCHOR_NW };
        pico_cv_pos_to("sub_cv", &w, &s);
        assert((int)s.x == 10 && (int)s.y == 20);
    }
    {
        puts("(10,20) sub -> (20,40) world (source=descendant)");
        Pico_Rel_Pos s = { '!', {10, 20}, PICO_ANCHOR_NW };
        Pico_Rel_Pos w = { '!', {0, 0}, PICO_ANCHOR_NW };
        pico_cv_pos_from("sub_cv", &s, &w);
        assert((int)w.x == 20 && (int)w.y == 40);
    }
    {
        puts("rect (20,40,10,20) world -> (10,20,5,10) sub (target=descendant)");
        Pico_Rel_Rect w = { '!', {20, 40, 10, 20}, PICO_ANCHOR_NW };
        Pico_Rel_Rect s = { '!', {0, 0, 0, 0}, PICO_ANCHOR_NW };
        pico_cv_rect_to("sub_cv", &w, &s);
        assert((int)s.x==10 && (int)s.y==20);
        assert((int)s.w==5  && (int)s.h==10);
    }
    {
        puts("dim (10,20) world -> (5,10) sub (target=descendant)");
        Pico_Rel_Dim w = { '!', {10, 20} };
        Pico_Rel_Dim s = { '!', {0, 0} };
        pico_cv_dim_to("sub_cv", &w, &s);
        assert((int)s.w==5 && (int)s.h==10);
    }

    printf("\n=== ALL TESTS PASSED ===\n");

    pico_init(0);
    return 0;
}
