#include "pico.h"
#include <stdio.h>

// resolve any rel-pos into cur's frame as integer pixels (NW origin)
static SDL_Point _abs_pos (const Pico_Rel_Pos* p) {
    Pico_Rel_Pos out = { '!', {0, 0}, PICO_ANCHOR_NW };
    pico_cv_pos_to(NULL, p, &out);
    return (SDL_Point){ (int)(out.x + 0.5f), (int)(out.y + 0.5f) };
}

static SDL_Rect _abs_rect (const Pico_Rel_Rect* r) {
    Pico_Rel_Rect out = { '!', {0, 0, 0, 0}, PICO_ANCHOR_NW };
    pico_cv_rect_to(NULL, r, &out);
    return (SDL_Rect){
        (int)(out.x + 0.5f), (int)(out.y + 0.5f),
        (int)(out.w + 0.5f), (int)(out.h + 0.5f),
    };
}

static int pt_eq (SDL_Point a, SDL_Point b) {
    return abs(a.x - b.x) <= 1 && abs(a.y - b.y) <= 1;
}

static int rc_eq (SDL_Rect a, SDL_Rect b) {
    return abs(a.x - b.x) <= 1 && abs(a.y - b.y) <= 1
        && abs(a.w - b.w) <= 1 && abs(a.h - b.h) <= 1;
}

int main (void) {
    pico_init(1);
    pico_set_scene_tile((Pico_Abs_Dim){0, 0});
    Pico_Rel_Dim dim = { '!', {100, 100} };
    pico_set_scene_dim(&dim);

    ///////////////////////////////////////////////////////////////////////////
    // Resolution: pos, rect through _to(NULL, ...) (mode-conv in cur)
    ///////////////////////////////////////////////////////////////////////////

    printf("=== resolution: pos ===\n");
    {
        puts("50% C in 100x100 -> (50,50)");
        Pico_Rel_Pos p = { '%', {0.5, 0.5}, PICO_ANCHOR_C };
        SDL_Point r = _abs_pos(&p);
        assert(r.x == 50 && r.y == 50);
    }
    {
        puts("50% NW -> (50,50)");
        Pico_Rel_Pos p = { '%', {0.5, 0.5}, PICO_ANCHOR_NW };
        SDL_Point r = _abs_pos(&p);
        assert(r.x == 50 && r.y == 50);
    }
    {
        puts("50% SE -> (49,49)");
        Pico_Rel_Pos p = { '%', {0.5, 0.5}, PICO_ANCHOR_SE };
        SDL_Point r = _abs_pos(&p);
        assert(r.x == 49 && r.y == 49);
    }
    {
        puts("! NW (10,15) -> (10,15)");
        Pico_Rel_Pos p = { '!', {10, 15}, PICO_ANCHOR_NW };
        SDL_Point r = _abs_pos(&p);
        assert(r.x == 10 && r.y == 15);
    }

    printf("=== resolution: rect ===\n");
    {
        puts("50% C 40x40 -> (30,30,40,40)");
        Pico_Rel_Rect p = { '%', {0.5, 0.5, 0.4, 0.4}, PICO_ANCHOR_C };
        SDL_Rect r = _abs_rect(&p);
        assert(r.x==30 && r.y==30 && r.w==40 && r.h==40);
    }
    {
        puts("50% NW 30x30 -> (50,50,30,30)");
        Pico_Rel_Rect p = { '%', {0.5, 0.5, 0.3, 0.3}, PICO_ANCHOR_NW };
        SDL_Rect r = _abs_rect(&p);
        assert(r.x==50 && r.y==50 && r.w==30 && r.h==30);
    }

    ///////////////////////////////////////////////////////////////////////////
    // Round-trip: starting (!, NW) value -> any (mode, anchor) -> back to (!, NW)
    ///////////////////////////////////////////////////////////////////////////

    Pico_Anchor anchors[] = {
        PICO_ANCHOR_NW, PICO_ANCHOR_N, PICO_ANCHOR_NE,
        PICO_ANCHOR_W,  PICO_ANCHOR_C, PICO_ANCHOR_E,
        PICO_ANCHOR_SW, PICO_ANCHOR_S, PICO_ANCHOR_SE,
    };
    int n_anchors = sizeof(anchors) / sizeof(anchors[0]);
    char modes[] = { '!', '%' };
    int n_modes = sizeof(modes) / sizeof(modes[0]);

    printf("=== round-trip: pos x (mode, anchor) ===\n");
    {
        SDL_Point starts[] = {
            {0, 0}, {50, 50}, {25, 75}, {100, 100}, {10, 90},
        };
        int n = sizeof(starts) / sizeof(starts[0]);
        int total = 0;
        for (int i = 0; i < n; i++) {
            Pico_Rel_Pos orig = { '!',
                {(float)starts[i].x, (float)starts[i].y}, PICO_ANCHOR_NW };
            for (int m = 0; m < n_modes; m++) {
                for (int a = 0; a < n_anchors; a++) {
                    Pico_Rel_Pos mid = { modes[m], {0, 0}, anchors[a] };
                    pico_cv_pos_to(NULL, &orig, &mid);
                    SDL_Point back = _abs_pos(&mid);
                    if (!pt_eq(back, starts[i])) {
                        printf("FAIL pos round-trip: start=(%d,%d) "
                               "mode='%c' anchor=(%.1f,%.1f) back=(%d,%d)\n",
                               starts[i].x, starts[i].y,
                               modes[m], anchors[a].x, anchors[a].y,
                               back.x, back.y);
                        assert(0);
                    }
                    total++;
                }
            }
        }
        printf("  passed: %d combos\n", total);
    }

    printf("=== round-trip: rect x (mode, anchor) ===\n");
    {
        SDL_Rect starts[] = {
            {0, 0, 10, 10}, {25, 25, 50, 50},
            {10, 20, 30, 40}, {0, 0, 100, 100},
        };
        int n = sizeof(starts) / sizeof(starts[0]);
        int total = 0;
        for (int i = 0; i < n; i++) {
            Pico_Rel_Rect orig = { '!',
                {(float)starts[i].x, (float)starts[i].y,
                 (float)starts[i].w, (float)starts[i].h}, PICO_ANCHOR_NW };
            for (int m = 0; m < n_modes; m++) {
                for (int a = 0; a < n_anchors; a++) {
                    Pico_Rel_Rect mid = { modes[m], {0, 0, 0, 0}, anchors[a] };
                    pico_cv_rect_to(NULL, &orig, &mid);
                    SDL_Rect back = _abs_rect(&mid);
                    if (!rc_eq(back, starts[i])) {
                        printf("FAIL rect round-trip: "
                               "start=(%d,%d,%d,%d) mode='%c' "
                               "anchor=(%.1f,%.1f) back=(%d,%d,%d,%d)\n",
                               starts[i].x, starts[i].y,
                               starts[i].w, starts[i].h,
                               modes[m], anchors[a].x, anchors[a].y,
                               back.x, back.y, back.w, back.h);
                        assert(0);
                    }
                    total++;
                }
            }
        }
        printf("  passed: %d combos\n", total);
    }

    printf("=== round-trip: dim x mode ===\n");
    {
        Pico_Abs_Dim starts[] = {
            {10, 10}, {50, 30}, {100, 100}, {25, 75},
        };
        int n = sizeof(starts) / sizeof(starts[0]);
        int total = 0;
        for (int i = 0; i < n; i++) {
            Pico_Rel_Dim orig = { '!',
                {(float)starts[i].w, (float)starts[i].h} };
            for (int m = 0; m < n_modes; m++) {
                Pico_Rel_Dim mid = { modes[m], {0, 0} };
                pico_cv_dim_to(NULL, &orig, &mid);
                Pico_Rel_Dim back = { '!', {0, 0} };
                pico_cv_dim_to(NULL, &mid, &back);
                int bw = (int)(back.w + 0.5f);
                int bh = (int)(back.h + 0.5f);
                if (abs(bw - starts[i].w) > 1 || abs(bh - starts[i].h) > 1) {
                    printf("FAIL dim round-trip: start=(%d,%d) "
                           "mode='%c' back=(%d,%d)\n",
                           starts[i].w, starts[i].h, modes[m], bw, bh);
                    assert(0);
                }
                total++;
            }
        }
        printf("  passed: %d combos\n", total);
    }

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
    pico_layer_empty("world", "sub_cv", (Pico_Abs_Dim){50, 50}, NULL);
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

    printf("\n=== ALL TESTS PASSED ===\n");

    pico_init(0);
    return 0;
}
