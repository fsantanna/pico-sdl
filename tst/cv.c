#include "pico.h"
#include <stdio.h>
#include <math.h>

// TODO: TGT=0 (phy)

#define EPSILON 1

static int pos_eq (Pico_Abs_Pos a, Pico_Abs_Pos b) {
    return (abs(a.x - b.x) <= EPSILON) && (abs(a.y - b.y) <= EPSILON);
}

static int rect_eq (Pico_Abs_Rect a, Pico_Abs_Rect b) {
    return (abs(a.x - b.x) <= EPSILON) && (abs(a.y - b.y) <= EPSILON) &&
           (abs(a.w - b.w) <= EPSILON) && (abs(a.h - b.h) <= EPSILON);
}

static int dim_eq (Pico_Abs_Dim a, Pico_Abs_Dim b) {
    return (abs(a.w - b.w) <= EPSILON) && (abs(a.h - b.h) <= EPSILON);
}

static void test_pos_abs_rel (
    Pico_Abs_Pos abs,
    char mode,
    Pico_Anchor anchor,
    Pico_Rel_Rect* up,
    Pico_Abs_Rect* base
) {
    Pico_Rel_Pos to = { mode, {0, 0}, anchor, up };
    pico_cv_pos_abs_rel(&abs, &to, base);
    Pico_Abs_Pos abs2 = pico_cv_pos_rel_abs(&to, base);
    if (!pos_eq(abs, abs2)) {
        printf("FAIL pos_abs_rel: mode='%c' anchor=(%.1f,%.1f)\n",
               mode, anchor.x, anchor.y);
        printf("  abs=(%d,%d) -> rel=(%.2f,%.2f) -> abs2=(%d,%d)\n",
               abs.x, abs.y, to.x, to.y, abs2.x, abs2.y);
        assert(0);
    }
}

static void test_pos_rel_rel (
    Pico_Rel_Pos fr,
    char mode_to,
    Pico_Anchor anchor_to,
    Pico_Rel_Rect* up_to,
    Pico_Abs_Rect* base
) {
    Pico_Abs_Pos abs1 = pico_cv_pos_rel_abs(&fr, base);
    Pico_Rel_Pos to = { mode_to, {0, 0}, anchor_to, up_to };
    pico_cv_pos_rel_rel(&fr, &to, base);
    Pico_Abs_Pos abs2 = pico_cv_pos_rel_abs(&to, base);
    if (!pos_eq(abs1, abs2)) {
        printf("FAIL pos_rel_rel: fr.mode='%c' to.mode='%c'\n",
               fr.mode, mode_to);
        printf("  abs1=(%d,%d) abs2=(%d,%d)\n",
               abs1.x, abs1.y, abs2.x, abs2.y);
        assert(0);
    }
}

static void test_rect_abs_rel (
    Pico_Abs_Rect abs,
    char mode,
    Pico_Anchor anchor,
    Pico_Rel_Rect* up,
    Pico_Abs_Rect* base
) {
    Pico_Rel_Rect to = { mode, {0, 0, 0, 0}, anchor, up };
    pico_cv_rect_abs_rel(&abs, &to, base);
    Pico_Abs_Rect abs2 = pico_cv_rect_rel_abs(&to, base);
    if (!rect_eq(abs, abs2)) {
        printf("FAIL rect_abs_rel: mode='%c' anchor=(%.1f,%.1f)\n",
               mode, anchor.x, anchor.y);
        printf("  abs=(%d,%d,%d,%d) -> rel=(%.2f,%.2f,%.2f,%.2f) "
               "-> abs2=(%d,%d,%d,%d)\n",
               abs.x, abs.y, abs.w, abs.h,
               to.x, to.y, to.w, to.h,
               abs2.x, abs2.y, abs2.w, abs2.h);
        assert(0);
    }
}

static void test_rect_rel_rel (
    Pico_Rel_Rect fr,
    char mode_to,
    Pico_Anchor anchor_to,
    Pico_Rel_Rect* up_to,
    Pico_Abs_Rect* base
) {
    Pico_Abs_Rect abs1 = pico_cv_rect_rel_abs(&fr, base);
    Pico_Rel_Rect to = { mode_to, {0, 0, 0, 0}, anchor_to, up_to };
    pico_cv_rect_rel_rel(&fr, &to, base);
    Pico_Abs_Rect abs2 = pico_cv_rect_rel_abs(&to, base);
    if (!rect_eq(abs1, abs2)) {
        printf("FAIL rect_rel_rel: fr.mode='%c' to.mode='%c'\n",
               fr.mode, mode_to);
        printf("  abs1=(%d,%d,%d,%d) abs2=(%d,%d,%d,%d)\n",
               abs1.x, abs1.y, abs1.w, abs1.h,
               abs2.x, abs2.y, abs2.w, abs2.h);
        assert(0);
    }
}

int main (void) {

    ///////////////////////////////////////////////////////////////////////////
    // EXISTING TESTS: rel_abs
    ///////////////////////////////////////////////////////////////////////////

    // EXT - POS - PCT->RAW
    {
        puts("ext - pos - pct->abs - C");
        Pico_Abs_Rect base = {25, 25, 50, 50};
        Pico_Rel_Pos  pct = { '%', {0.5, 0.5}, PICO_ANCHOR_C, NULL};
        Pico_Abs_Pos  abs = pico_cv_pos_rel_abs(&pct, &base);
        printf("pos: (%d, %d)\n", abs.x, abs.y);
        assert(abs.x==50 && abs.y==50);
    }
    {
        puts("ext - pct->abs - NW - top/left");
        Pico_Abs_Rect base = {25, 25, 50, 50};
        Pico_Rel_Pos  pct = { '%', {0.5, 0.5}, PICO_ANCHOR_NW, NULL};
        Pico_Abs_Pos  abs = pico_cv_pos_rel_abs(&pct, &base);
        printf("pos: (%d, %d)\n", abs.x, abs.y);
        assert(abs.x==50 && abs.y==50);
    }
    {
        puts("ext - pct->abs - SE - bottom/right");
        Pico_Abs_Rect base = {25, 25, 50, 50};
        Pico_Rel_Pos  pct = { '%', {0.5, 0.5}, PICO_ANCHOR_SE, NULL};
        Pico_Abs_Pos  abs = pico_cv_pos_rel_abs(&pct, &base);
        printf("pos: (%d, %d)\n", abs.x, abs.y);
        assert(abs.x==49 && abs.y==49);
    }

    // EXT - RECT - PCT->RAW
    {
        puts("ext - rect - pct->abs - C");
        Pico_Abs_Rect base = {20, 20, 60, 60};
        Pico_Rel_Rect pct = { '%', {0.25, 0.25, 0.5, 0.25}, PICO_ANCHOR_C, NULL};
        Pico_Abs_Rect abs = pico_cv_rect_rel_abs(&pct, &base);
        printf("rect: (%d, %d, %d, %d)\n", abs.x, abs.y, abs.w, abs.h);
        assert(abs.x==20 && abs.y==28 && abs.w==30 && abs.h==15);
    }
    {
        puts("ext - rect - pct->abs - NE");
        Pico_Abs_Rect base  = {20, 20, 60, 60};
        Pico_Rel_Rect pct = { '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_NE, NULL};
        Pico_Abs_Rect abs = pico_cv_rect_rel_abs(&pct, &base);
        printf("rect: (%d, %d, %d, %d)\n", abs.x, abs.y, abs.w, abs.h);
        assert(abs.x==20 && abs.y==50 && abs.w==30 && abs.h==30);
    }
    {
        puts("ext - rect - pct->abs - SW");
        Pico_Abs_Rect base = {20, 20, 60, 60};
        Pico_Rel_Rect pct = { '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_SW, NULL};
        Pico_Abs_Rect abs = pico_cv_rect_rel_abs(&pct, &base);
        printf("rect: (%d, %d, %d, %d)\n", abs.x, abs.y, abs.w, abs.h);
        assert(abs.x==50 && abs.y==20 && abs.w==30 && abs.h==30);
    }

    // ABS - POS - with base (1/0)
    {
        puts("abs - pos - base only");
        Pico_Abs_Rect base = {20, 20, 60, 60};
        Pico_Rel_Pos  pos = { '!', {10, 15}, PICO_ANCHOR_NW, NULL };
        Pico_Abs_Pos  abs = pico_cv_pos_rel_abs(&pos, &base);
        printf("pos: (%d, %d)\n", abs.x, abs.y);
        assert(abs.x==30 && abs.y==35);
    }
    {
        puts("abs - pos - base only - anchor C");
        Pico_Abs_Rect base = {20, 20, 60, 60};
        Pico_Rel_Pos  pos = { '!', {30, 30}, PICO_ANCHOR_C, NULL };
        Pico_Abs_Pos  abs = pico_cv_pos_rel_abs(&pos, &base);
        printf("pos: (%d, %d)\n", abs.x, abs.y);
        assert(abs.x==50 && abs.y==50);
    }

    // ABS - POS - with base and up (1/1)
    {
        puts("abs - pos - base and up");
        Pico_Abs_Rect base = {10, 10, 80, 80};
        Pico_Rel_Rect up  = { '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, NULL };
        Pico_Rel_Pos  pos = { '!', {5, 5}, PICO_ANCHOR_NW, &up };
        Pico_Abs_Pos  abs = pico_cv_pos_rel_abs(&pos, &base);
        printf("pos: (%d, %d)\n", abs.x, abs.y);
        assert(abs.x==35 && abs.y==35);
    }

    // ABS - RECT - with base (1/0)
    {
        puts("abs - rect - base only");
        Pico_Abs_Rect base  = {20, 20, 60, 60};
        Pico_Rel_Rect rect = { '!', {10, 10, 20, 20}, PICO_ANCHOR_NW, NULL };
        Pico_Abs_Rect abs  = pico_cv_rect_rel_abs(&rect, &base);
        printf("rect: (%d, %d, %d, %d)\n", abs.x, abs.y, abs.w, abs.h);
        assert(abs.x==30 && abs.y==30 && abs.w==20 && abs.h==20);
    }
    {
        puts("abs - rect - base only - anchor C");
        Pico_Abs_Rect base  = {20, 20, 60, 60};
        Pico_Rel_Rect rect = { '!', {30, 30, 20, 20}, PICO_ANCHOR_C, NULL };
        Pico_Abs_Rect abs  = pico_cv_rect_rel_abs(&rect, &base);
        printf("rect: (%d, %d, %d, %d)\n", abs.x, abs.y, abs.w, abs.h);
        assert(abs.x==40 && abs.y==40 && abs.w==20 && abs.h==20);
    }

    // ABS - RECT - with base and up (1/1)
    {
        puts("abs - rect - base and up");
        Pico_Abs_Rect base  = {10, 10, 80, 80};
        Pico_Rel_Rect up   = { '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, NULL };
        Pico_Rel_Rect rect = { '!', {5, 5, 10, 10}, PICO_ANCHOR_NW, &up };
        Pico_Abs_Rect abs  = pico_cv_rect_rel_abs(&rect, &base);
        printf("rect: (%d, %d, %d, %d)\n", abs.x, abs.y, abs.w, abs.h);
        assert(abs.x==35 && abs.y==35 && abs.w==10 && abs.h==10);
    }

    pico_init(1);
    Pico_Rel_Dim log = { '!', {100, 100}, NULL };
    pico_set_view(-1, &log, NULL, NULL, NULL, NULL, NULL, NULL);

    // ABS - POS - with up (0/1) - requires pico_init
    {
        puts("abs - pos - up only");
        Pico_Rel_Rect up  = { '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, NULL };
        Pico_Rel_Pos  pos = { '!', {10, 10}, PICO_ANCHOR_NW, &up };
        Pico_Abs_Pos  abs = pico_cv_pos_rel_abs(&pos, NULL);
        printf("pos: (%d, %d)\n", abs.x, abs.y);
        assert(abs.x==35 && abs.y==35);
    }

    // ABS - RECT - with up (0/1) - requires pico_init
    {
        puts("abs - rect - up only");
        Pico_Rel_Rect up   = { '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, NULL };
        Pico_Rel_Rect rect = { '!', {5, 5, 10, 10}, PICO_ANCHOR_NW, &up };
        Pico_Abs_Rect abs  = pico_cv_rect_rel_abs(&rect, NULL);
        printf("rect: (%d, %d, %d, %d)\n", abs.x, abs.y, abs.w, abs.h);
        assert(abs.x==30 && abs.y==30 && abs.w==10 && abs.h==10);
    }

    // LOG - POS - PCT->RAW
    {
        puts("log - pos - pct->abs - C");
        Pico_Rel_Pos pct = { '%', {0.4, 0.7}, PICO_ANCHOR_C, NULL };
        Pico_Abs_Pos abs = pico_cv_pos_rel_abs(&pct, NULL);
        printf("pos: (%d, %d)\n", abs.x, abs.y);
        assert(abs.x==40 && abs.y==70);
    }
    {
        puts("log - pos - pct->abs - NW");
        Pico_Rel_Pos pct = { '%', {0.55, 0.45}, PICO_ANCHOR_NW, NULL };
        Pico_Abs_Pos abs = pico_cv_pos_rel_abs(&pct, NULL);
        printf("pos: (%d, %d)\n", abs.x, abs.y);
        assert(abs.x==55 && abs.y==45);
    }
    {
        puts("log - pos - pct->abs - SE");
        Pico_Rel_Pos pct = { '%', {0.5, 0.5}, PICO_ANCHOR_SE, NULL };
        Pico_Abs_Pos abs = pico_cv_pos_rel_abs(&pct, NULL);
        printf("pos: (%d, %d)\n", abs.x, abs.y);
        assert(abs.x==49 && abs.y==49);
    }
    {
        puts("log - pos - pct->abs - corners");
        Pico_Rel_Pos p1 = { '%', {0.0, 0.0}, PICO_ANCHOR_NW, NULL };
        Pico_Rel_Pos p2 = { '%', {1.0, 1.0}, PICO_ANCHOR_NW, NULL };
        Pico_Abs_Pos r1 = pico_cv_pos_rel_abs(&p1, NULL);
        Pico_Abs_Pos r2 = pico_cv_pos_rel_abs(&p2, NULL);
        assert(r1.x==0   && r1.y==0);
        assert(r2.x==100 && r2.y==100);
    }
    {
        puts("log - pos - pct->abs - corners");
        Pico_Rel_Pos p1 = { '%', {0.0, 0.0}, PICO_ANCHOR_C, NULL };
        Pico_Rel_Pos p2 = { '%', {1.0, 1.0}, PICO_ANCHOR_C, NULL };
        Pico_Abs_Pos r1 = pico_cv_pos_rel_abs(&p1, NULL);
        Pico_Abs_Pos r2 = pico_cv_pos_rel_abs(&p2, NULL);
        assert(r1.x==-1  && r1.y==-1);
        assert(r2.x==100 && r2.y==100);
    }

    // LOG - RECT - PCT->RAW
    {
        puts("log - rect - pct->abs - C");
        Pico_Rel_Rect pct = { '%', {0.5, 0.5, 0.4, 0.4}, PICO_ANCHOR_C, NULL };
        Pico_Abs_Rect abs = pico_cv_rect_rel_abs(&pct, NULL);
        printf("rect: (%d, %d, %d, %d)\n", abs.x, abs.y, abs.w, abs.h);
        assert(abs.x==30 && abs.y==30 && abs.w==40 && abs.h==40);
    }
    {
        puts("log - rect - pct->abs - NW");
        Pico_Rel_Rect pct = { '%', {0.5, 0.5, 0.3, 0.3}, PICO_ANCHOR_NW, NULL };
        Pico_Abs_Rect abs = pico_cv_rect_rel_abs(&pct, NULL);
        printf("rect: (%d, %d, %d, %d)\n", abs.x, abs.y, abs.w, abs.h);
        assert(abs.x==50 && abs.y==50 && abs.w==30 && abs.h==30);
    }

    // TILE MODE - set up 4x4 grid of 4x4 pixel tiles = 16x16 logical
    {
        Pico_Rel_Dim log  = { '#', {4, 4}, NULL };
        Pico_Abs_Dim tile = { 4, 4 };
        pico_set_view(-1, &log, &tile, NULL, NULL, NULL, NULL, NULL);
    }

    // TILE - POS - tile->abs
    {
        puts("tile - pos - tile->abs - NW");
        Pico_Rel_Pos pos = { '#', {2, 3}, PICO_ANCHOR_NW, NULL };
        Pico_Abs_Pos abs = pico_cv_pos_rel_abs(&pos, NULL);
        printf("pos: (%d, %d)\n", abs.x, abs.y);
        assert(abs.x==4 && abs.y==8);
    }
    {
        puts("tile - pos - tile->abs - C");
        Pico_Rel_Pos pos = { '#', {2, 2}, PICO_ANCHOR_C, NULL };
        Pico_Abs_Pos abs = pico_cv_pos_rel_abs(&pos, NULL);
        printf("pos: (%d, %d)\n", abs.x, abs.y);
        assert(abs.x==6 && abs.y==6);
    }
    {
        puts("tile - pos - tile->abs - corners");
        Pico_Rel_Pos p1 = { '#', {1, 1}, PICO_ANCHOR_NW, NULL };
        Pico_Rel_Pos p4 = { '#', {4, 4}, PICO_ANCHOR_NW, NULL };
        Pico_Abs_Pos r1 = pico_cv_pos_rel_abs(&p1, NULL);
        Pico_Abs_Pos r4 = pico_cv_pos_rel_abs(&p4, NULL);
        assert(r1.x==0  && r1.y==0);
        assert(r4.x==12 && r4.y==12);
    }

    // TILE - RECT - tile->abs
    {
        puts("tile - rect - tile->abs - NW");
        Pico_Rel_Rect rect = { '#', {1, 1, 1, 1}, PICO_ANCHOR_NW, NULL };
        Pico_Abs_Rect abs  = pico_cv_rect_rel_abs(&rect, NULL);
        printf("rect: (%d, %d, %d, %d)\n", abs.x, abs.y, abs.w, abs.h);
        assert(abs.x==0 && abs.y==0 && abs.w==4 && abs.h==4);
    }
    {
        puts("tile - rect - tile->abs - C");
        Pico_Rel_Rect rect = { '#', {2, 2, 2, 2}, PICO_ANCHOR_C, NULL };
        Pico_Abs_Rect abs  = pico_cv_rect_rel_abs(&rect, NULL);
        printf("rect: (%d, %d, %d, %d)\n", abs.x, abs.y, abs.w, abs.h);
        assert(abs.x==2 && abs.y==2 && abs.w==8 && abs.h==8);
    }
    {
        puts("tile - rect - tile->abs - 2x2 at (2,2) NW");
        Pico_Rel_Rect rect = { '#', {2, 2, 2, 2}, PICO_ANCHOR_NW, NULL };
        Pico_Abs_Rect abs  = pico_cv_rect_rel_abs(&rect, NULL);
        printf("rect: (%d, %d, %d, %d)\n", abs.x, abs.y, abs.w, abs.h);
        assert(abs.x==4 && abs.y==4 && abs.w==8 && abs.h==8);
    }
    {
        puts("tile - rect - tile->abs - fractional position");
        Pico_Rel_Rect rect = { '#', {2.5, 2.5, 2, 2}, PICO_ANCHOR_C, NULL };
        Pico_Abs_Rect abs  = pico_cv_rect_rel_abs(&rect, NULL);
        printf("rect: (%d, %d, %d, %d)\n", abs.x, abs.y, abs.w, abs.h);
        assert(abs.x==4 && abs.y==4 && abs.w==8 && abs.h==8);
    }

    // DIM - raw->abs
    {
        puts("dim - raw->abs");
        Pico_Rel_Dim dim = { '!', {50, 30}, NULL };
        Pico_Abs_Dim abs = pico_cv_dim_rel_abs(&dim, NULL);
        printf("dim: (%d, %d)\n", abs.w, abs.h);
        assert(dim_eq(abs, (Pico_Abs_Dim){50, 30}));
    }

    // DIM - pct->abs (16x16 logical from tile view)
    {
        puts("dim - pct->abs");
        Pico_Rel_Dim dim = { '%', {0.5, 0.75}, NULL };
        Pico_Abs_Dim abs = pico_cv_dim_rel_abs(&dim, NULL);
        printf("dim: (%d, %d)\n", abs.w, abs.h);
        assert(dim_eq(abs, (Pico_Abs_Dim){8, 12}));
    }

    // DIM - tile->abs (4x4 tiles)
    {
        puts("dim - tile->abs");
        Pico_Rel_Dim dim = { '#', {2, 3}, NULL };
        Pico_Abs_Dim abs = pico_cv_dim_rel_abs(&dim, NULL);
        printf("dim: (%d, %d)\n", abs.w, abs.h);
        assert(dim_eq(abs, (Pico_Abs_Dim){8, 12}));
    }

    // DIM - pct->abs with base
    {
        puts("dim - pct->abs - base");
        Pico_Abs_Rect base = { 10, 10, 80, 60 };
        Pico_Rel_Dim dim = { '%', {0.5, 0.5}, NULL };
        Pico_Abs_Dim abs = pico_cv_dim_rel_abs(&dim, &base);
        printf("dim: (%d, %d)\n", abs.w, abs.h);
        assert(dim_eq(abs, (Pico_Abs_Dim){40, 30}));
    }

    ///////////////////////////////////////////////////////////////////////////
    // NEW TESTS: abs_rel and rel_rel (canonical form invariant)
    ///////////////////////////////////////////////////////////////////////////

    // Reset to 100x100 for inverse tests
    {
        Pico_Rel_Dim dim = { '!', {100, 100}, NULL };
        pico_set_view(-1, &dim, NULL, NULL, NULL, NULL, NULL, NULL);
    }

    Pico_Anchor anchors[] = {
        PICO_ANCHOR_NW, PICO_ANCHOR_N, PICO_ANCHOR_NE,
        PICO_ANCHOR_W,  PICO_ANCHOR_C, PICO_ANCHOR_E,
        PICO_ANCHOR_SW, PICO_ANCHOR_S, PICO_ANCHOR_SE
    };
    int n_anchors = sizeof(anchors) / sizeof(anchors[0]);

    char modes[] = { '!', '%' };
    int n_modes = sizeof(modes) / sizeof(modes[0]);

    Pico_Abs_Rect base = { 0, 0, 100, 100 };

    printf("\n=== Testing pico_cv_pos_abs_rel ===\n");
    {
        Pico_Abs_Pos positions[] = {
            {0, 0}, {50, 50}, {100, 100}, {25, 75}, {10, 90}
        };
        int n_pos = sizeof(positions) / sizeof(positions[0]);

        for (int i = 0; i < n_pos; i++) {
            for (int m = 0; m < n_modes; m++) {
                for (int a = 0; a < n_anchors; a++) {
                    test_pos_abs_rel(positions[i], modes[m],
                                     anchors[a], NULL, &base);
                }
            }
        }
        printf("  passed: %d positions x %d modes x %d anchors\n",
               n_pos, n_modes, n_anchors);
    }

    printf("=== Testing pico_cv_pos_rel_rel ===\n");
    {
        Pico_Rel_Pos from_positions[] = {
            { '!', {10, 20}, PICO_ANCHOR_NW, NULL },
            { '!', {50, 50}, PICO_ANCHOR_C, NULL },
            { '%', {0.5, 0.5}, PICO_ANCHOR_C, NULL },
            { '%', {0.25, 0.75}, PICO_ANCHOR_NW, NULL },
        };
        int n_from = sizeof(from_positions) / sizeof(from_positions[0]);

        for (int i = 0; i < n_from; i++) {
            for (int m = 0; m < n_modes; m++) {
                for (int a = 0; a < n_anchors; a++) {
                    test_pos_rel_rel(from_positions[i], modes[m],
                                     anchors[a], NULL, &base);
                }
            }
        }
        printf("  passed: %d from x %d modes x %d anchors\n",
               n_from, n_modes, n_anchors);
    }

    printf("=== Testing pico_cv_rect_abs_rel ===\n");
    {
        Pico_Abs_Rect rects[] = {
            {0, 0, 10, 10},
            {25, 25, 50, 50},
            {10, 20, 30, 40},
            {0, 0, 100, 100},
        };
        int n_rects = sizeof(rects) / sizeof(rects[0]);

        for (int i = 0; i < n_rects; i++) {
            for (int m = 0; m < n_modes; m++) {
                for (int a = 0; a < n_anchors; a++) {
                    test_rect_abs_rel(rects[i], modes[m],
                                      anchors[a], NULL, &base);
                }
            }
        }
        printf("  passed: %d rects x %d modes x %d anchors\n",
               n_rects, n_modes, n_anchors);
    }

    printf("=== Testing pico_cv_rect_rel_rel ===\n");
    {
        Pico_Rel_Rect from_rects[] = {
            { '!', {10, 20, 30, 40}, PICO_ANCHOR_NW, NULL },
            { '!', {50, 50, 20, 20}, PICO_ANCHOR_C, NULL },
            { '%', {0.5, 0.5, 0.2, 0.2}, PICO_ANCHOR_C, NULL },
            { '%', {0.25, 0.25, 0.5, 0.5}, PICO_ANCHOR_NW, NULL },
        };
        int n_from = sizeof(from_rects) / sizeof(from_rects[0]);

        for (int i = 0; i < n_from; i++) {
            for (int m = 0; m < n_modes; m++) {
                for (int a = 0; a < n_anchors; a++) {
                    test_rect_rel_rel(from_rects[i], modes[m],
                                      anchors[a], NULL, &base);
                }
            }
        }
        printf("  passed: %d from x %d modes x %d anchors\n",
               n_from, n_modes, n_anchors);
    }

    printf("=== Testing with up hierarchy ===\n");
    {
        Pico_Rel_Rect parent = { '!', {20, 20, 60, 60}, PICO_ANCHOR_NW, NULL };

        Pico_Abs_Pos pos = { 50, 50 };
        test_pos_abs_rel(pos, '!', PICO_ANCHOR_C, &parent, &base);
        test_pos_abs_rel(pos, '%', PICO_ANCHOR_C, &parent, &base);

        Pico_Rel_Pos rel_pos = { '!', {30, 30}, PICO_ANCHOR_C, &parent };
        test_pos_rel_rel(rel_pos, '%', PICO_ANCHOR_NW, &parent, &base);

        Pico_Abs_Rect rect = { 30, 30, 20, 20 };
        test_rect_abs_rel(rect, '!', PICO_ANCHOR_C, &parent, &base);
        test_rect_abs_rel(rect, '%', PICO_ANCHOR_C, &parent, &base);

        Pico_Rel_Rect rel_rect = { '!', {10, 10, 20, 20}, PICO_ANCHOR_NW,
                                   &parent };
        test_rect_rel_rel(rel_rect, '%', PICO_ANCHOR_C, &parent, &base);

        printf("  passed: up hierarchy tests\n");
    }

    printf("\n=== ALL TESTS PASSED ===\n");

    pico_init(0);
    return 0;
}
