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

    // rect_rect - L1 NULL, r1=NULL would be invalid (assertion); skip

    // pos_pos - p1 in sub vs p2 in cur, both land on same pixel
    {
        puts("vs_pos_pos - p1 in sub maps to p2 in cur");
        Pico_Rel_Pos p1 = { '!', {10, 20}, PICO_ANCHOR_NW };  // sub -> cur (20,40)
        Pico_Rel_Pos p2 = { '!', {20, 40}, PICO_ANCHOR_NW };
        assert(pico_vs_pos_pos("sub_vs", &p1, NULL, &p2) == 1);
    }

    pico_init(0);
    return 0;
}
