#include "pico.h"

// TODO: TGT=0 (phy)

int main (void) {
    // EXT - POS - PCT->RAW
    {
        puts("ext - pos - pct->abs - C");
        Pico_Abs_Rect ref = {25, 25, 50, 50};
        Pico_Rel_Pos  pct = { '%', {0.5, 0.5}, PICO_ANCHOR_C, NULL};
        Pico_Abs_Pos  abs = pico_cv_pos_rel_abs(&pct, &ref);
        printf("pos: (%d, %d)\n", abs.x, abs.y);
        assert(abs.x==50 && abs.y==50);
    }
    {
        puts("ext - pct->abs - NW - top/left");
        Pico_Abs_Rect ref = {25, 25, 50, 50};
        Pico_Rel_Pos  pct = { '%', {0.5, 0.5}, PICO_ANCHOR_NW, NULL};
        Pico_Abs_Pos  abs = pico_cv_pos_rel_abs(&pct, &ref);
        printf("pos: (%d, %d)\n", abs.x, abs.y);
        assert(abs.x==50 && abs.y==50);
    }
    {
        puts("ext - pct->abs - SE - bottom/right");
        Pico_Abs_Rect ref = {25, 25, 50, 50};
        Pico_Rel_Pos  pct = { '%', {0.5, 0.5}, PICO_ANCHOR_SE, NULL};
        Pico_Abs_Pos  abs = pico_cv_pos_rel_abs(&pct, &ref);
        printf("pos: (%d, %d)\n", abs.x, abs.y);
        assert(abs.x==49 && abs.y==49);
    }

    // EXT - RECT - PCT->RAW
    {
        puts("ext - rect - pct->abs - C");
        Pico_Abs_Rect ref = {20, 20, 60, 60};
        Pico_Rel_Rect pct = { '%', {0.25, 0.25, 0.5, 0.25}, PICO_ANCHOR_C, NULL};
        Pico_Abs_Rect abs = pico_cv_rect_rel_abs(&pct, &ref);
        printf("rect: (%d, %d, %d, %d)\n", abs.x, abs.y, abs.w, abs.h);
        assert(abs.x==20 && abs.y==28 && abs.w==30 && abs.h==15);
    }
    {
        puts("ext - rect - pct->abs - NE");
        Pico_Abs_Rect ref  = {20, 20, 60, 60};
        Pico_Rel_Rect pct = { '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_NE, NULL};
        Pico_Abs_Rect abs = pico_cv_rect_rel_abs(&pct, &ref);
        printf("rect: (%d, %d, %d, %d)\n", abs.x, abs.y, abs.w, abs.h);
        assert(abs.x==20 && abs.y==50 && abs.w==30 && abs.h==30);
    }
    {
        puts("ext - rect - pct->abs - SW");
        Pico_Abs_Rect ref = {20, 20, 60, 60};
        Pico_Rel_Rect pct = { '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_SW, NULL};
        Pico_Abs_Rect abs = pico_cv_rect_rel_abs(&pct, &ref);
        printf("rect: (%d, %d, %d, %d)\n", abs.x, abs.y, abs.w, abs.h);
        assert(abs.x==50 && abs.y==20 && abs.w==30 && abs.h==30);
    }

    // ABS - POS - with ref (1/0)
    {
        puts("abs - pos - ref only");
        Pico_Abs_Rect ref = {20, 20, 60, 60};
        Pico_Rel_Pos  pos = { '!', {10, 15}, PICO_ANCHOR_NW, NULL };
        Pico_Abs_Pos  abs = pico_cv_pos_rel_abs(&pos, &ref);
        printf("pos: (%d, %d)\n", abs.x, abs.y);
        assert(abs.x==30 && abs.y==35);
    }
    {
        puts("abs - pos - ref only - anchor C");
        Pico_Abs_Rect ref = {20, 20, 60, 60};
        Pico_Rel_Pos  pos = { '!', {30, 30}, PICO_ANCHOR_C, NULL };
        Pico_Abs_Pos  abs = pico_cv_pos_rel_abs(&pos, &ref);
        printf("pos: (%d, %d)\n", abs.x, abs.y);
        assert(abs.x==50 && abs.y==50);
    }

    // ABS - POS - with ref and up (1/1)
    {
        puts("abs - pos - ref and up");
        Pico_Abs_Rect ref = {10, 10, 80, 80};
        Pico_Rel_Rect up  = { '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, NULL };
        Pico_Rel_Pos  pos = { '!', {5, 5}, PICO_ANCHOR_NW, &up };
        Pico_Abs_Pos  abs = pico_cv_pos_rel_abs(&pos, &ref);
        // up on ref(10,10,80,80) -> (30,30,40,40), pos adds (5,5)
        printf("pos: (%d, %d)\n", abs.x, abs.y);
        assert(abs.x==35 && abs.y==35);
    }

    // ABS - RECT - with ref (1/0)
    {
        puts("abs - rect - ref only");
        Pico_Abs_Rect ref  = {20, 20, 60, 60};
        Pico_Rel_Rect rect = { '!', {10, 10, 20, 20}, PICO_ANCHOR_NW, NULL };
        Pico_Abs_Rect abs  = pico_cv_rect_rel_abs(&rect, &ref);
        printf("rect: (%d, %d, %d, %d)\n", abs.x, abs.y, abs.w, abs.h);
        assert(abs.x==30 && abs.y==30 && abs.w==20 && abs.h==20);
    }
    {
        puts("abs - rect - ref only - anchor C");
        Pico_Abs_Rect ref  = {20, 20, 60, 60};
        Pico_Rel_Rect rect = { '!', {30, 30, 20, 20}, PICO_ANCHOR_C, NULL };
        Pico_Abs_Rect abs  = pico_cv_rect_rel_abs(&rect, &ref);
        printf("rect: (%d, %d, %d, %d)\n", abs.x, abs.y, abs.w, abs.h);
        assert(abs.x==40 && abs.y==40 && abs.w==20 && abs.h==20);
    }

    // ABS - RECT - with ref and up (1/1)
    {
        puts("abs - rect - ref and up");
        Pico_Abs_Rect ref  = {10, 10, 80, 80};
        Pico_Rel_Rect up   = { '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, NULL };
        Pico_Rel_Rect rect = { '!', {5, 5, 10, 10}, PICO_ANCHOR_NW, &up };
        Pico_Abs_Rect abs  = pico_cv_rect_rel_abs(&rect, &ref);
        // up on ref(10,10,80,80) -> (30,30,40,40), rect adds (5,5)
        printf("rect: (%d, %d, %d, %d)\n", abs.x, abs.y, abs.w, abs.h);
        assert(abs.x==35 && abs.y==35 && abs.w==10 && abs.h==10);
    }

    pico_init(1);
    Pico_Rel_Dim log = { '!', {100, 100}, NULL };
    pico_set_view(NULL, -1, -1, NULL, NULL, &log, NULL, NULL, NULL);

    // ABS - POS - with up (0/1) - requires pico_init
    {
        puts("abs - pos - up only");
        Pico_Rel_Rect up  = { '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, NULL };
        Pico_Rel_Pos  pos = { '!', {10, 10}, PICO_ANCHOR_NW, &up };
        Pico_Abs_Pos  abs = pico_cv_pos_rel_abs(&pos, NULL);
        // up resolves to (25,25,50,50) on 100x100, pos adds (10,10)
        printf("pos: (%d, %d)\n", abs.x, abs.y);
        assert(abs.x==35 && abs.y==35);
    }

    // ABS - RECT - with up (0/1) - requires pico_init
    {
        puts("abs - rect - up only");
        Pico_Rel_Rect up   = { '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, NULL };
        Pico_Rel_Rect rect = { '!', {5, 5, 10, 10}, PICO_ANCHOR_NW, &up };
        Pico_Abs_Rect abs  = pico_cv_rect_rel_abs(&rect, NULL);
        // up resolves to (25,25,50,50) on 100x100, rect adds (5,5)
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
        pico_set_view(NULL, -1, -1, NULL, NULL, &log, NULL, NULL, &tile);
    }

    // TILE - POS - tile->abs
    {
        puts("tile - pos - tile->abs - NW");
        Pico_Rel_Pos pos = { '#', {2, 3}, PICO_ANCHOR_NW, NULL };
        Pico_Abs_Pos abs = pico_cv_pos_rel_abs(&pos, NULL);
        printf("pos: (%d, %d)\n", abs.x, abs.y);
        // tile (2,3) NW -> pixel ((2-1)*4, (3-1)*4) = (4, 8)
        assert(abs.x==4 && abs.y==8);
    }
    {
        puts("tile - pos - tile->abs - C");
        Pico_Rel_Pos pos = { '#', {2, 2}, PICO_ANCHOR_C, NULL };
        Pico_Abs_Pos abs = pico_cv_pos_rel_abs(&pos, NULL);
        printf("pos: (%d, %d)\n", abs.x, abs.y);
        // tile (2,2) C -> pixel (2-1+0.5)*4 = 6
        assert(abs.x==6 && abs.y==6);
    }
    {
        puts("tile - pos - tile->abs - corners");
        Pico_Rel_Pos p1 = { '#', {1, 1}, PICO_ANCHOR_NW, NULL };
        Pico_Rel_Pos p4 = { '#', {4, 4}, PICO_ANCHOR_NW, NULL };
        Pico_Abs_Pos r1 = pico_cv_pos_rel_abs(&p1, NULL);
        Pico_Abs_Pos r4 = pico_cv_pos_rel_abs(&p4, NULL);
        // tile (1,1) -> pixel (0, 0)
        // tile (4,4) -> pixel (12, 12)
        assert(r1.x==0  && r1.y==0);
        assert(r4.x==12 && r4.y==12);
    }

    // TILE - RECT - tile->abs
    {
        puts("tile - rect - tile->abs - NW");
        Pico_Rel_Rect rect = { '#', {1, 1, 1, 1}, PICO_ANCHOR_NW, NULL };
        Pico_Abs_Rect abs  = pico_cv_rect_rel_abs(&rect, NULL);
        printf("rect: (%d, %d, %d, %d)\n", abs.x, abs.y, abs.w, abs.h);
        // tile (1,1,1,1) NW -> pixel (0, 0, 4, 4)
        assert(abs.x==0 && abs.y==0 && abs.w==4 && abs.h==4);
    }
    {
        puts("tile - rect - tile->abs - C");
        Pico_Rel_Rect rect = { '#', {2, 2, 2, 2}, PICO_ANCHOR_C, NULL };
        Pico_Abs_Rect abs  = pico_cv_rect_rel_abs(&rect, NULL);
        printf("rect: (%d, %d, %d, %d)\n", abs.x, abs.y, abs.w, abs.h);
        // tile (2,2,2,2) C -> d=(8,8), x=(2-1+0.5)*4-0.5*8=2, y=2 -> (2, 2, 8, 8)
        assert(abs.x==2 && abs.y==2 && abs.w==8 && abs.h==8);
    }
    {
        puts("tile - rect - tile->abs - 2x2 at (2,2) NW");
        Pico_Rel_Rect rect = { '#', {2, 2, 2, 2}, PICO_ANCHOR_NW, NULL };
        Pico_Abs_Rect abs  = pico_cv_rect_rel_abs(&rect, NULL);
        printf("rect: (%d, %d, %d, %d)\n", abs.x, abs.y, abs.w, abs.h);
        // tile (2,2,2,2) NW -> pixel ((2-1)*4, (2-1)*4, 8, 8) = (4, 4, 8, 8)
        assert(abs.x==4 && abs.y==4 && abs.w==8 && abs.h==8);
    }
    {
        puts("tile - rect - tile->abs - fractional position");
        Pico_Rel_Rect rect = { '#', {2.5, 2.5, 2, 2}, PICO_ANCHOR_C, NULL };
        Pico_Abs_Rect abs  = pico_cv_rect_rel_abs(&rect, NULL);
        printf("rect: (%d, %d, %d, %d)\n", abs.x, abs.y, abs.w, abs.h);
        // tile (2.5,2.5,2,2) C -> d=(8,8), x=(2.5-1+0.5)*4-0.5*8=8-4=4, y=4
        assert(abs.x==4 && abs.y==4 && abs.w==8 && abs.h==8);
    }

    pico_init(0);
    return 0;
}
