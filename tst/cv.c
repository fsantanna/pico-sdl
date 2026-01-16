#include "pico.h"

// TODO: TGT=0 (phy)

int main (void) {
    // EXT - POS - PCT->RAW
    {
        puts("ext - pos - pct->raw - C");
        Pico_Rect    up  = {25, 25, 50, 50};
        Pico_Pos_Pct pct = {0.5, 0.5, PICO_ANCHOR_C, NULL};
        Pico_Pos     raw = pico_cv_pos_pct_raw_ext(&pct, up);
        printf("pos: (%d, %d)\n", raw.x, raw.y);
        assert(raw.x==50 && raw.y==50);
    }
    {
        puts("ext - pct->raw - NW - top/left");
        Pico_Rect    up  = {25, 25, 50, 50};
        Pico_Pos_Pct pct = {0.5, 0.5, PICO_ANCHOR_NW, NULL};
        Pico_Pos     raw = pico_cv_pos_pct_raw_ext(&pct, up);
        printf("pos: (%d, %d)\n", raw.x, raw.y);
        assert(raw.x==50 && raw.y==50);
    }
    {
        puts("ext - pct->raw - SE - bottom/right");
        Pico_Rect    up  = {25, 25, 50, 50};
        Pico_Pos_Pct pct = {0.5, 0.5, PICO_ANCHOR_SE, NULL};
        Pico_Pos     raw = pico_cv_pos_pct_raw_ext(&pct, up);
        printf("pos: (%d, %d)\n", raw.x, raw.y);
        assert(raw.x==49 && raw.y==49);
    }

    // EXT - RECT - PCT->RAW
    {
        puts("ext - rect - pct->raw - C");
        Pico_Rect     up  = {20, 20, 60, 60};
        Pico_Rect_Pct pct = {0.25, 0.25, 0.5, 0.25, PICO_ANCHOR_C, NULL};
        Pico_Rect     raw = pico_cv_rect_pct_raw_ext(&pct, up);
        printf("rect: (%d, %d, %d, %d)\n", raw.x, raw.y, raw.w, raw.h);
        assert(raw.x==20 && raw.y==28 && raw.w==30 && raw.h==15);
    }
    {
        puts("ext - rect - pct->raw - NE");
        Pico_Rect     up  = {20, 20, 60, 60};
        Pico_Rect_Pct pct = {0.5, 0.5, 0.5, 0.5, PICO_ANCHOR_NE, NULL};
        Pico_Rect     raw = pico_cv_rect_pct_raw_ext(&pct, up);
        printf("rect: (%d, %d, %d, %d)\n", raw.x, raw.y, raw.w, raw.h);
        assert(raw.x==20 && raw.y==50 && raw.w==30 && raw.h==30);
    }
    {
        puts("ext - rect - pct->raw - SW");
        Pico_Rect     up  = {20, 20, 60, 60};
        Pico_Rect_Pct pct = {0.5, 0.5, 0.5, 0.5, PICO_ANCHOR_SW, NULL};
        Pico_Rect     raw = pico_cv_rect_pct_raw_ext(&pct, up);
        printf("rect: (%d, %d, %d, %d)\n", raw.x, raw.y, raw.w, raw.h);
        assert(raw.x==50 && raw.y==20 && raw.w==30 && raw.h==30);
    }

    pico_init(1);
    Pico_Dim log = {100, 100};
    pico_set_view_raw(-1, NULL, NULL, &log, NULL, NULL);

    // LOG - POS - PCT->RAW
    {
        puts("log - pos - pct->raw - C");
        Pico_Pos_Pct pct = {0.4, 0.7, PICO_ANCHOR_C, NULL};
        Pico_Pos     raw = pico_cv_pos_pct_raw(&pct);
        printf("pos: (%d, %d)\n", raw.x, raw.y);
        assert(raw.x==40 && raw.y==70);
    }
    {
        puts("log - pos - pct->raw - NW");
        Pico_Pos_Pct pct = {0.55, 0.45, PICO_ANCHOR_NW, NULL};
        Pico_Pos     raw = pico_cv_pos_pct_raw(&pct);
        printf("pos: (%d, %d)\n", raw.x, raw.y);
        assert(raw.x==55 && raw.y==45);
    }
    {
        puts("log - pos - pct->raw - SE");
        Pico_Pos_Pct pct = {0.5, 0.5, PICO_ANCHOR_SE, NULL};
        Pico_Pos     raw = pico_cv_pos_pct_raw(&pct);
        printf("pos: (%d, %d)\n", raw.x, raw.y);
        assert(raw.x==49 && raw.y==49);
    }
    {
        puts("log - pos - pct->raw - corners");
        Pico_Pos_Pct p1 = {0.0, 0.0, PICO_ANCHOR_NW, NULL};
        Pico_Pos_Pct p2 = {1.0, 1.0, PICO_ANCHOR_NW, NULL};
        Pico_Pos     r1 = pico_cv_pos_pct_raw(&p1);
        Pico_Pos     r2 = pico_cv_pos_pct_raw(&p2);
        assert(r1.x==0   && r1.y==0);
        assert(r2.x==100 && r2.y==100);
    }
    {
        puts("log - pos - pct->raw - corners");
        Pico_Pos_Pct p1 = {0.0, 0.0, PICO_ANCHOR_C, NULL};
        Pico_Pos_Pct p2 = {1.0, 1.0, PICO_ANCHOR_C, NULL};
        Pico_Pos     r1 = pico_cv_pos_pct_raw(&p1);
        Pico_Pos     r2 = pico_cv_pos_pct_raw(&p2);
        assert(r1.x==-1  && r1.y==-1);
        assert(r2.x==100 && r2.y==100);
    }

    // LOG - RECT - PCT->RAW
    {
        puts("log - rect - pct->raw - C");
        Pico_Rect_Pct pct = {0.5, 0.5, 0.4, 0.4, PICO_ANCHOR_C, NULL};
        Pico_Rect     raw = pico_cv_rect_pct_raw(&pct);
        printf("rect: (%d, %d, %d, %d)\n", raw.x, raw.y, raw.w, raw.h);
        assert(raw.x==30 && raw.y==30 && raw.w==40 && raw.h==40);
    }
    {
        puts("log - rect - pct->raw - NW");
        Pico_Rect_Pct pct = {0.5, 0.5, 0.3, 0.3, PICO_ANCHOR_NW, NULL};
        Pico_Rect     raw = pico_cv_rect_pct_raw(&pct);
        printf("rect: (%d, %d, %d, %d)\n", raw.x, raw.y, raw.w, raw.h);
        assert(raw.x==50 && raw.y==50 && raw.w==30 && raw.h==30);
    }

    pico_init(0);
    return 0;
}
