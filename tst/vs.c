#include "pico.h"

int main (void) {
    pico_init(1);

    Pico_Rel_Dim phy = { '!', {200, 200} };
    Pico_Rel_Dim log = { '!', {100, 100} };
    pico_set_layer("window");
    pico_set_scene_dim(&phy);
    pico_set_layer("world");
    pico_set_scene_dim(&log);

    // pos_rect - cur vs cur (abs)
    {
        puts("vs_pos_rect - point inside rect (abs)");
        Pico_Rel_Rect r = { '!', {25, 25, 50, 50}, PICO_ANCHOR_NW };
        Pico_Rel_Pos  p = { '!', {50, 50}, PICO_ANCHOR_NW };
        assert(pico_vs_pos_rect(NULL, &p, NULL, &r) == 1);
    }
    {
        puts("vs_pos_rect - point outside rect (abs)");
        Pico_Rel_Rect r = { '!', {25, 25, 50, 50}, PICO_ANCHOR_NW };
        Pico_Rel_Pos  p = { '!', {10, 10}, PICO_ANCHOR_NW };
        assert(pico_vs_pos_rect(NULL, &p, NULL, &r) == 0);
    }

    // pos_rect - cur vs cur (pct)
    {
        puts("vs_pos_rect - point inside rect (pct)");
        Pico_Rel_Rect r = { '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C };
        Pico_Rel_Pos  p = { '%', {0.5, 0.5}, PICO_ANCHOR_C };
        assert(pico_vs_pos_rect(NULL, &p, NULL, &r) == 1);
    }
    {
        puts("vs_pos_rect - point outside rect (pct)");
        Pico_Rel_Rect r = { '%', {0.5, 0.5, 0.3, 0.3}, PICO_ANCHOR_C };
        Pico_Rel_Pos  p = { '%', {0.1, 0.1}, PICO_ANCHOR_C };
        assert(pico_vs_pos_rect(NULL, &p, NULL, &r) == 0);
    }

    // rect_rect - cur vs cur (abs)
    {
        puts("vs_rect_rect - overlapping (abs)");
        Pico_Rel_Rect r1 = { '!', {20, 20, 40, 40}, PICO_ANCHOR_NW };
        Pico_Rel_Rect r2 = { '!', {30, 30, 40, 40}, PICO_ANCHOR_NW };
        assert(pico_vs_rect_rect(NULL, &r1, NULL, &r2) == 1);
    }
    {
        puts("vs_rect_rect - non-overlapping (abs)");
        Pico_Rel_Rect r1 = { '!', {10, 10, 30, 30}, PICO_ANCHOR_NW };
        Pico_Rel_Rect r2 = { '!', {50, 50, 30, 30}, PICO_ANCHOR_NW };
        assert(pico_vs_rect_rect(NULL, &r1, NULL, &r2) == 0);
    }

    // rect_rect - cur vs cur (pct)
    {
        puts("vs_rect_rect - overlapping (pct)");
        Pico_Rel_Rect r1 = { '%', {0.3, 0.3, 0.4, 0.4}, PICO_ANCHOR_C };
        Pico_Rel_Rect r2 = { '%', {0.5, 0.5, 0.4, 0.4}, PICO_ANCHOR_C };
        assert(pico_vs_rect_rect(NULL, &r1, NULL, &r2) == 1);
    }
    {
        puts("vs_rect_rect - non-overlapping (pct)");
        Pico_Rel_Rect r1 = { '%', {0.2, 0.2, 0.2, 0.2}, PICO_ANCHOR_C };
        Pico_Rel_Rect r2 = { '%', {0.8, 0.8, 0.2, 0.2}, PICO_ANCHOR_C };
        assert(pico_vs_rect_rect(NULL, &r1, NULL, &r2) == 0);
    }

    // pos_pos - cur vs cur
    {
        puts("vs_pos_pos - same pixel (cur)");
        Pico_Rel_Pos p1 = { '!', {50, 50}, PICO_ANCHOR_NW };
        Pico_Rel_Pos p2 = { '%', {0.5, 0.5}, PICO_ANCHOR_NW };
        assert(pico_vs_pos_pos(NULL, &p1, NULL, &p2) == 1);
    }
    {
        puts("vs_pos_pos - different pixels");
        Pico_Rel_Pos p1 = { '!', {50, 50}, PICO_ANCHOR_NW };
        Pico_Rel_Pos p2 = { '!', {51, 50}, PICO_ANCHOR_NW };
        assert(pico_vs_pos_pos(NULL, &p1, NULL, &p2) == 0);
    }

    // direct-child layer (sub) attached to world; project value cur<-sub
    pico_layer_empty("world", "sub_vs", 1, (Pico_Rel_Dim){'!', {50, 50}}, NULL);

    // pos_rect - p1 in sub, r2 in cur
    {
        puts("vs_pos_rect - p1 in sub (scale 2x) hits r2 in cur");
        Pico_Rel_Pos  p1 = { '!', {10, 20}, PICO_ANCHOR_NW };  // sub abs (10,20) -> cur (20,40)
        Pico_Rel_Rect r2 = { '!', {15, 35, 20, 20}, PICO_ANCHOR_NW };
        assert(pico_vs_pos_rect("sub_vs", &p1, NULL, &r2) == 1);
    }

    // pos_rect - r2=NULL uses sub's bounds (scene.dst)
    {
        puts("vs_pos_rect - r2=NULL uses sub's bounds");
        // sub's scene.dst default is {'%', {.5,.5,1,1}, C} -> fills cur entirely
        Pico_Rel_Pos p1 = { '%', {0.5, 0.5}, PICO_ANCHOR_NW };
        assert(pico_vs_pos_rect(NULL, &p1, "sub_vs", NULL) == 1);
    }

    // pos_pos - p1 in sub vs p2 in cur, both land on same pixel
    {
        puts("vs_pos_pos - p1 in sub maps to p2 in cur");
        Pico_Rel_Pos p1 = { '!', {10, 20}, PICO_ANCHOR_NW };  // sub -> cur (20,40)
        Pico_Rel_Pos p2 = { '!', {20, 40}, PICO_ANCHOR_NW };
        assert(pico_vs_pos_pos("sub_vs", &p1, NULL, &p2) == 1);
    }

    // pos_rect - r2=NULL, L2=NULL: defaults to cur's bounds
    {
        puts("vs_pos_rect - r2=NULL+L2=NULL uses cur's bounds");
        Pico_Rel_Pos p1 = { '%', {0.5, 0.5}, PICO_ANCHOR_NW };
        assert(pico_vs_pos_rect(NULL, &p1, NULL, NULL) == 1);
    }

    // rect_rect - both NULL: both default to cur's bounds, overlap
    {
        puts("vs_rect_rect - r1=NULL+r2=NULL uses cur's bounds");
        assert(pico_vs_rect_rect(NULL, NULL, NULL, NULL) == 1);
    }

    // rect_pos - mirror of pos_rect
    {
        puts("vs_rect_pos - point inside rect (abs)");
        Pico_Rel_Rect r1 = { '!', {25, 25, 50, 50}, PICO_ANCHOR_NW };
        Pico_Rel_Pos  p2 = { '!', {50, 50}, PICO_ANCHOR_NW };
        assert(pico_vs_rect_pos(NULL, &r1, NULL, &p2) == 1);
    }
    {
        puts("vs_rect_pos - r1=NULL uses cur's bounds");
        Pico_Rel_Pos p2 = { '%', {0.5, 0.5}, PICO_ANCHOR_NW };
        assert(pico_vs_rect_pos(NULL, NULL, NULL, &p2) == 1);
    }
    {
        puts("vs_rect_pos - p2 in sub maps into r1 in cur");
        Pico_Rel_Rect r1 = { '!', {15, 35, 20, 20}, PICO_ANCHOR_NW };
        Pico_Rel_Pos  p2 = { '!', {10, 20}, PICO_ANCHOR_NW };  // sub -> cur (20,40)
        assert(pico_vs_rect_pos(NULL, &r1, "sub_vs", &p2) == 1);
    }

    // % mode out of [0,1] -> projects outside cur's bounds
    {
        puts("vs_pos_rect - p1=% over 1.0 outside cur bounds");
        Pico_Rel_Pos p1 = { '%', {1.5, 1.5}, PICO_ANCHOR_NW };
        assert(pico_vs_pos_rect(NULL, &p1, NULL, NULL) == 0);
    }
    {
        puts("vs_pos_rect - p1=% under 0.0 outside cur bounds");
        Pico_Rel_Pos p1 = { '%', {-0.1, -0.1}, PICO_ANCHOR_NW };
        assert(pico_vs_pos_rect(NULL, &p1, NULL, NULL) == 0);
    }
    {
        puts("vs_rect_rect - r1=% over 1.0 outside cur bounds");
        Pico_Rel_Rect r1 = { '%', {1.5, 1.5, 0.1, 0.1}, PICO_ANCHOR_NW };
        assert(pico_vs_rect_rect(NULL, &r1, NULL, NULL) == 0);
    }
    {
        puts("vs_rect_rect - r1=% under 0.0 outside cur bounds");
        Pico_Rel_Rect r1 = { '%', {-0.5, -0.5, 0.2, 0.2}, PICO_ANCHOR_NW };
        assert(pico_vs_rect_rect(NULL, &r1, NULL, NULL) == 0);
    }
    {
        puts("vs_pos_pos - one %=0.5 vs other %=1.5 differ");
        Pico_Rel_Pos p1 = { '%', { 0.5,  0.5}, PICO_ANCHOR_NW };
        Pico_Rel_Pos p2 = { '%', { 1.5,  1.5}, PICO_ANCHOR_NW };
        assert(pico_vs_pos_pos(NULL, &p1, NULL, &p2) == 0);
    }
    {
        puts("vs_pos_pos - one %=0.5 vs other %=-0.1 differ");
        Pico_Rel_Pos p1 = { '%', { 0.5,  0.5}, PICO_ANCHOR_NW };
        Pico_Rel_Pos p2 = { '%', {-0.1, -0.1}, PICO_ANCHOR_NW };
        assert(pico_vs_pos_pos(NULL, &p1, NULL, &p2) == 0);
    }
    {
        puts("vs_rect_pos - p2=% over 1.0 outside r1");
        Pico_Rel_Rect r1 = { '!', {25, 25, 50, 50}, PICO_ANCHOR_NW };
        Pico_Rel_Pos  p2 = { '%', { 1.5,  1.5}, PICO_ANCHOR_NW };
        assert(pico_vs_rect_pos(NULL, &r1, NULL, &p2) == 0);
    }
    {
        puts("vs_rect_pos - p2=% under 0.0 outside r1");
        Pico_Rel_Rect r1 = { '!', {25, 25, 50, 50}, PICO_ANCHOR_NW };
        Pico_Rel_Pos  p2 = { '%', {-0.1, -0.1}, PICO_ANCHOR_NW };
        assert(pico_vs_rect_pos(NULL, &r1, NULL, &p2) == 0);
    }

    // grandchild of cur: deeper than direct child
    pico_layer_empty("sub_vs", "sub_sub_vs", 1,
                     (Pico_Rel_Dim){'!', {25, 25}}, NULL);
    {
        puts("vs_pos_pos - p1 in grandchild (scale 4x) vs cur");
        // sub_sub:10,10 -> sub_vs:20,20 -> world:40,40
        Pico_Rel_Pos p1 = { '!', {10, 10}, PICO_ANCHOR_NW };
        Pico_Rel_Pos p2 = { '!', {40, 40}, PICO_ANCHOR_NW };
        assert(pico_vs_pos_pos("sub_sub_vs", &p1, NULL, &p2) == 1);
    }
    {
        puts("vs_pos_pos - p1 in grandchild vs mismatched cur pixel");
        Pico_Rel_Pos p1 = { '!', {10, 10}, PICO_ANCHOR_NW };
        Pico_Rel_Pos p2 = { '!', {50, 50}, PICO_ANCHOR_NW };
        assert(pico_vs_pos_pos("sub_sub_vs", &p1, NULL, &p2) == 0);
    }

    // inverse ancestor (bidirectional): cur = sub_vs, L1 = world
    pico_set_layer("sub_vs");
    {
        puts("vs_pos_pos - L1=world (ancestor of cur=sub_vs)");
        // world:20,40 -> sub_vs:10,20 (sub 50x50 in world 100x100, scale 0.5x)
        Pico_Rel_Pos p1 = { '!', {20, 40}, PICO_ANCHOR_NW };
        Pico_Rel_Pos p2 = { '!', {10, 20}, PICO_ANCHOR_NW };
        assert(pico_vs_pos_pos("world", &p1, NULL, &p2) == 1);
    }
    {
        puts("vs_rect_rect - L1=world (ancestor of cur=sub_vs)");
        Pico_Rel_Rect r1 = { '!', {20, 40, 10, 20}, PICO_ANCHOR_NW };
        Pico_Rel_Rect r2 = { '!', {10, 20,  5, 10}, PICO_ANCHOR_NW };
        assert(pico_vs_rect_rect("world", &r1, NULL, &r2) == 1);
    }
    pico_set_layer("world");

    // deep descendant w/ NON-full scene.dst: explicit r_pct path.
    // sub_sub_tight (10x10) sits in top-left 10x10 of sub_vs (50x50);
    // projected to world, that's the top-left 20x20.
    pico_layer_empty("sub_vs", "sub_sub_tight", 1,
                     (Pico_Rel_Dim){'!', {10, 10}}, NULL);
    pico_set_layer("sub_sub_tight");
    pico_set_scene_dst((Pico_Rel_Rect){'!', {0, 0, 10, 10}, PICO_ANCHOR_NW});
    pico_set_layer("world");
    {
        puts("vs_pos_rect - non-full scene.dst, explicit r_pct grandchild");
        Pico_Rel_Pos  p_in  = { '!', {15, 15}, PICO_ANCHOR_NW };
        Pico_Rel_Pos  p_out = { '!', {30, 30}, PICO_ANCHOR_NW };
        Pico_Rel_Rect r_pct = { '%', {0, 0, 1, 1}, PICO_ANCHOR_NW };
        assert(pico_vs_pos_rect(NULL, &p_in,  "sub_sub_tight", &r_pct) == 1);
        assert(pico_vs_pos_rect(NULL, &p_out, "sub_sub_tight", &r_pct) == 0);
        assert(pico_vs_rect_pos("sub_sub_tight", &r_pct, NULL, &p_in)  == 1);
        assert(pico_vs_rect_pos("sub_sub_tight", &r_pct, NULL, &p_out) == 0);
    }
    {
        puts("vs_pos_rect - non-full scene.dst, default r2 (grandchild bounds)");
        Pico_Rel_Pos p_in  = { '!', {15, 15}, PICO_ANCHOR_NW };
        Pico_Rel_Pos p_out = { '!', {30, 30}, PICO_ANCHOR_NW };
        assert(pico_vs_pos_rect(NULL, &p_in,  "sub_sub_tight", NULL) == 1);
        assert(pico_vs_pos_rect(NULL, &p_out, "sub_sub_tight", NULL) == 0);
        assert(pico_vs_rect_pos("sub_sub_tight", NULL, NULL, &p_in)  == 1);
        assert(pico_vs_rect_pos("sub_sub_tight", NULL, NULL, &p_out) == 0);
    }

    pico_init(0);
    return 0;
}
