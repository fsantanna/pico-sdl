#include "pico.h"

int main (void) {
    // EXT - POS - PCT->RAW
    {
        puts("ext - pos - pct->raw - C");
        Pico_Rect up = {25, 25, 50, 50};
        Pico_Pos_Pct p_pct = {0.5, 0.5, PICO_ANCHOR_C, NULL};
        Pico_Pos p_raw = pico_cv_pos_pct_raw_ext(&p_pct, up);
        printf("pos: (%d, %d)\n", p_raw.x, p_raw.y);
        assert(p_raw.x==50 && p_raw.y==50);
    }
    {
        puts("ext - pct->raw - NW - top/left");
        Pico_Rect up = {25, 25, 50, 50};
        Pico_Pos_Pct p_pct = {0.5, 0.5, PICO_ANCHOR_NW, NULL};
        Pico_Pos p_raw = pico_cv_pos_pct_raw_ext(&p_pct, up);
        printf("pos: (%d, %d)\n", p_raw.x, p_raw.y);
        assert(p_raw.x==50 && p_raw.y==50);
    }
    {
        puts("ext - pct->raw - SE - bottom/right");
        Pico_Rect up = {25, 25, 50, 50};
        Pico_Pos_Pct p_pct = {0.5, 0.5, PICO_ANCHOR_SE, NULL};
        Pico_Pos p_raw = pico_cv_pos_pct_raw_ext(&p_pct, up);
        printf("pos: (%d, %d)\n", p_raw.x, p_raw.y);
        assert(p_raw.x==49 && p_raw.y==49);
    }

    // EXT - RECT - PCT->RAW
    {
        puts("ext - rect - pct->raw - C");
        Pico_Rect parent = {20, 20, 60, 60};
        Pico_Rect_Pct r_pct = {0.5, 0.5, 0.5, 0.5, PICO_ANCHOR_C, NULL};
        Pico_Rect r_raw = pico_cv_rect_pct_raw_ext(&r_pct, parent);
        printf("rect: (%d, %d, %d, %d)\n", r_raw.x, r_raw.y, r_raw.w, r_raw.h);
        assert(r_raw.w==30 && r_raw.h==30);
    }
    {
        puts("ext - rect - pct->raw - NE");
        Pico_Rect parent = {20, 20, 60, 60};
        Pico_Rect_Pct r_pct = {0.5, 0.5, 0.5, 0.5, PICO_ANCHOR_NE, NULL};
        Pico_Rect r_raw = pico_cv_rect_pct_raw_ext(&r_pct, parent);
        printf("rect: (%d, %d, %d, %d)\n", r_raw.x, r_raw.y, r_raw.w, r_raw.h);
        assert(r_raw.w==30 && r_raw.h==30);
    }
    {
        puts("ext - rect - pct->raw - SW");
        Pico_Rect parent = {20, 20, 60, 60};
        Pico_Rect_Pct r_pct = {0.5, 0.5, 0.5, 0.5, PICO_ANCHOR_SW, NULL};
        Pico_Rect r_raw = pico_cv_rect_pct_raw_ext(&r_pct, parent);
        printf("rect: (%d, %d, %d, %d)\n", r_raw.x, r_raw.y, r_raw.w, r_raw.h);
        assert(r_raw.w==30 && r_raw.h==30);
    }

    Pico_Dim log = {100, 100};
    pico_set_view_raw(-1, NULL, NULL, &log, NULL, NULL);

    // LOG - RECT - PCT->RAW
    {
        puts("cv_pos_pct_raw - center position - anchor C");
        Pico_Pos_Pct p_pct = {0.5, 0.5, PICO_ANCHOR_C, NULL};
        Pico_Pos p_raw = pico_cv_pos_pct_raw(&p_pct);
        printf("pos: (%d, %d)\n", p_raw.x, p_raw.y);
        assert(p_raw.x==50 && p_raw.y==50);
    }
    {
        puts("cv_pos_pct_raw - center position - anchor NW");
        Pico_Pos_Pct p_pct = {0.5, 0.5, PICO_ANCHOR_NW, NULL};
        Pico_Pos p_raw = pico_cv_pos_pct_raw(&p_pct);
        printf("pos: (%d, %d)\n", p_raw.x, p_raw.y);
        assert(p_raw.x==50 && p_raw.y==50);
    }
    {
        puts("cv_pos_pct_raw - center position - anchor SE");
        Pico_Pos_Pct p_pct = {0.5, 0.5, PICO_ANCHOR_SE, NULL};
        Pico_Pos p_raw = pico_cv_pos_pct_raw(&p_pct);
        printf("pos: (%d, %d)\n", p_raw.x, p_raw.y);
        assert(p_raw.x==50 && p_raw.y==50);
    }
    {
        puts("cv_pos_pct_raw - corner positions");
        Pico_Pos_Pct p1 = {0.0, 0.0, PICO_ANCHOR_C, NULL};
        Pico_Pos_Pct p2 = {1.0, 1.0, PICO_ANCHOR_C, NULL};
        Pico_Pos r1 = pico_cv_pos_pct_raw(&p1);
        Pico_Pos r2 = pico_cv_pos_pct_raw(&p2);
        assert(r1.x == 0 && r1.y == 0);
        assert(r2.x == 100 && r2.y == 100);
    }

    // LOG - POS - PCT->RAW
    {
        puts("cv_rect_pct_raw - centered rect");
        Pico_Rect_Pct r_pct = {0.5, 0.5, 0.4, 0.4, PICO_ANCHOR_C, NULL};
        Pico_Rect r_raw = pico_cv_rect_pct_raw(&r_pct);
        printf("rect: (%d, %d, %d, %d)\n", r_raw.x, r_raw.y, r_raw.w, r_raw.h);
        assert(r_raw.w == 40 && r_raw.h == 40);
    }
    {
        puts("cv_rect_pct_raw - with different anchor");
        Pico_Rect_Pct r_pct = {0.5, 0.5, 0.3, 0.3, PICO_ANCHOR_NW, NULL};
        Pico_Rect r_raw = pico_cv_rect_pct_raw(&r_pct);
        printf("rect: (%d, %d, %d, %d)\n", r_raw.x, r_raw.y, r_raw.w, r_raw.h);
    }

    return 0;
}
