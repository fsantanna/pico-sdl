#include "pico.h"

int main (void) {
    pico_init(1);

    Pico_Dim phy = {200, 200};
    Pico_Dim log = {100, 100};
    pico_set_view_raw(-1, &phy, NULL, &log, NULL, NULL);

    // pico_pos_vs_rect_raw
    {
        puts("pos_vs_rect_raw - point inside rect");
        Pico_Rect rect = {25, 25, 50, 50};
        Pico_Pos pos_in = {50, 50};
        int result = pico_pos_vs_rect_raw(pos_in, rect);
        printf("pos (%d, %d) vs rect (%d, %d, %d, %d): %d\n",
               pos_in.x, pos_in.y, rect.x, rect.y, rect.w, rect.h, result);
        assert(result == 1);
    }
    {
        puts("pos_vs_rect_raw - point outside rect");
        Pico_Rect rect = {25, 25, 50, 50};
        Pico_Pos pos_out = {10, 10};
        int result = pico_pos_vs_rect_raw(pos_out, rect);
        printf("pos (%d, %d) vs rect (%d, %d, %d, %d): %d\n",
               pos_out.x, pos_out.y, rect.x, rect.y, rect.w, rect.h, result);
        assert(result == 0);
    }

    // pico_pos_vs_rect_pct
    {
        puts("pos_vs_rect_pct - point inside rect");
        Pico_Rect_Pct rect = {0.5, 0.5, 0.5, 0.5, PICO_ANCHOR_C, NULL};
        Pico_Pos_Pct pos_in = {0.5, 0.5, PICO_ANCHOR_C, NULL};
        int result = pico_pos_vs_rect_pct(&pos_in, &rect);
        printf("pos (%.1f, %.1f) vs rect (%.1f, %.1f, %.1f, %.1f): %d\n",
               pos_in.x, pos_in.y, rect.x, rect.y, rect.w, rect.h, result);
        assert(result == 1);
    }
    {
        puts("pos_vs_rect_pct - point outside rect");
        Pico_Rect_Pct rect = {0.5, 0.5, 0.3, 0.3, PICO_ANCHOR_C, NULL};
        Pico_Pos_Pct pos_out = {0.1, 0.1, PICO_ANCHOR_C, NULL};
        int result = pico_pos_vs_rect_pct(&pos_out, &rect);
        printf("pos (%.1f, %.1f) vs rect (%.1f, %.1f, %.1f, %.1f): %d\n",
               pos_out.x, pos_out.y, rect.x, rect.y, rect.w, rect.h, result);
        assert(result == 0);
    }

    // pico_rect_vs_rect_raw
    {
        puts("rect_vs_rect_raw - overlapping rects");
        Pico_Rect r1 = {20, 20, 40, 40};
        Pico_Rect r2 = {30, 30, 40, 40};
        int result = pico_rect_vs_rect_raw(r1, r2);
        printf("rect1 (%d, %d, %d, %d) vs rect2 (%d, %d, %d, %d): %d\n",
               r1.x, r1.y, r1.w, r1.h, r2.x, r2.y, r2.w, r2.h, result);
        assert(result == 1);
    }
    {
        puts("rect_vs_rect_raw - non-overlapping rects");
        Pico_Rect r1 = {10, 10, 30, 30};
        Pico_Rect r2 = {50, 50, 30, 30};
        int result = pico_rect_vs_rect_raw(r1, r2);
        printf("rect1 (%d, %d, %d, %d) vs rect2 (%d, %d, %d, %d): %d\n",
               r1.x, r1.y, r1.w, r1.h, r2.x, r2.y, r2.w, r2.h, result);
        assert(result == 0);
    }

    // pico_rect_vs_rect_pct
    {
        puts("rect_vs_rect_pct - overlapping rects");
        Pico_Rect_Pct r1 = {0.3, 0.3, 0.4, 0.4, PICO_ANCHOR_C, NULL};
        Pico_Rect_Pct r2 = {0.5, 0.5, 0.4, 0.4, PICO_ANCHOR_C, NULL};
        int result = pico_rect_vs_rect_pct(&r1, &r2);
        printf("rect1 (%.1f, %.1f, %.1f, %.1f) vs rect2 (%.1f, %.1f, %.1f, %.1f): %d\n",
               r1.x, r1.y, r1.w, r1.h, r2.x, r2.y, r2.w, r2.h, result);
        assert(result == 1);
    }
    {
        puts("rect_vs_rect_pct - non-overlapping rects");
        Pico_Rect_Pct r1 = {0.2, 0.2, 0.2, 0.2, PICO_ANCHOR_C, NULL};
        Pico_Rect_Pct r2 = {0.8, 0.8, 0.2, 0.2, PICO_ANCHOR_C, NULL};
        int result = pico_rect_vs_rect_pct(&r1, &r2);
        printf("rect1 (%.1f, %.1f, %.1f, %.1f) vs rect2 (%.1f, %.1f, %.1f, %.1f): %d\n",
               r1.x, r1.y, r1.w, r1.h, r2.x, r2.y, r2.w, r2.h, result);
        assert(result == 0);
    }

    pico_init(0);
    return 0;
}
