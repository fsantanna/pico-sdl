#include "pico.h"

int main (void) {
    pico_init(1);

    Pico_Rel_Dim phy = { '!', {200, 200}, NULL };
    Pico_Rel_Dim log = { '!', {100, 100}, NULL };
    pico_set_view(NULL, -1, -1, &phy, NULL, &log, NULL, NULL);

    // pico_vs_pos_rect - abs
    {
        puts("vs_pos_rect - point inside rect (abs)");
        Pico_Rel_Rect rect = { '!', {25, 25, 50, 50}, PICO_ANCHOR_NW, NULL };
        Pico_Rel_Pos pos_in = { '!', {50, 50}, PICO_ANCHOR_NW, NULL };
        int result = pico_vs_pos_rect(&pos_in, &rect);
        printf("pos (%g, %g) vs rect (%g, %g, %g, %g): %d\n",
               pos_in.x, pos_in.y, rect.x, rect.y, rect.w, rect.h, result);
        assert(result == 1);
    }
    {
        puts("vs_pos_rect - point outside rect (abs)");
        Pico_Rel_Rect rect = { '!', {25, 25, 50, 50}, PICO_ANCHOR_NW, NULL };
        Pico_Rel_Pos pos_out = { '!', {10, 10}, PICO_ANCHOR_NW, NULL };
        int result = pico_vs_pos_rect(&pos_out, &rect);
        printf("pos (%g, %g) vs rect (%g, %g, %g, %g): %d\n",
               pos_out.x, pos_out.y, rect.x, rect.y, rect.w, rect.h, result);
        assert(result == 0);
    }

    // pico_vs_pos_rect - pct
    {
        puts("vs_pos_rect - point inside rect (pct)");
        Pico_Rel_Rect rect = { '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C, NULL };
        Pico_Rel_Pos pos_in = { '%', {0.5, 0.5}, PICO_ANCHOR_C, NULL };
        int result = pico_vs_pos_rect(&pos_in, &rect);
        printf("pos (%.1f, %.1f) vs rect (%.1f, %.1f, %.1f, %.1f): %d\n",
               pos_in.x, pos_in.y, rect.x, rect.y, rect.w, rect.h, result);
        assert(result == 1);
    }
    {
        puts("vs_pos_rect - point outside rect (pct)");
        Pico_Rel_Rect rect = { '%', {0.5, 0.5, 0.3, 0.3}, PICO_ANCHOR_C, NULL };
        Pico_Rel_Pos pos_out = { '%', {0.1, 0.1}, PICO_ANCHOR_C, NULL };
        int result = pico_vs_pos_rect(&pos_out, &rect);
        printf("pos (%.1f, %.1f) vs rect (%.1f, %.1f, %.1f, %.1f): %d\n",
               pos_out.x, pos_out.y, rect.x, rect.y, rect.w, rect.h, result);
        assert(result == 0);
    }

    // pico_vs_rect_rect - abs
    {
        puts("vs_rect_rect - overlapping rects (abs)");
        Pico_Rel_Rect r1 = { '!', {20, 20, 40, 40}, PICO_ANCHOR_NW, NULL };
        Pico_Rel_Rect r2 = { '!', {30, 30, 40, 40}, PICO_ANCHOR_NW, NULL };
        int result = pico_vs_rect_rect(&r1, &r2);
        printf("rect1 (%g, %g, %g, %g) vs rect2 (%g, %g, %g, %g): %d\n",
               r1.x, r1.y, r1.w, r1.h, r2.x, r2.y, r2.w, r2.h, result);
        assert(result == 1);
    }
    {
        puts("vs_rect_rect - non-overlapping rects (abs)");
        Pico_Rel_Rect r1 = { '!', {10, 10, 30, 30}, PICO_ANCHOR_NW, NULL };
        Pico_Rel_Rect r2 = { '!', {50, 50, 30, 30}, PICO_ANCHOR_NW, NULL };
        int result = pico_vs_rect_rect(&r1, &r2);
        printf("rect1 (%g, %g, %g, %g) vs rect2 (%g, %g, %g, %g): %d\n",
               r1.x, r1.y, r1.w, r1.h, r2.x, r2.y, r2.w, r2.h, result);
        assert(result == 0);
    }

    // pico_vs_rect_rect - pct
    {
        puts("vs_rect_rect - overlapping rects (pct)");
        Pico_Rel_Rect r1 = { '%', {0.3, 0.3, 0.4, 0.4}, PICO_ANCHOR_C, NULL };
        Pico_Rel_Rect r2 = { '%', {0.5, 0.5, 0.4, 0.4}, PICO_ANCHOR_C, NULL };
        int result = pico_vs_rect_rect(&r1, &r2);
        printf("rect1 (%.1f, %.1f, %.1f, %.1f) vs rect2 (%.1f, %.1f, %.1f, %.1f): %d\n",
               r1.x, r1.y, r1.w, r1.h, r2.x, r2.y, r2.w, r2.h, result);
        assert(result == 1);
    }
    {
        puts("vs_rect_rect - non-overlapping rects (pct)");
        Pico_Rel_Rect r1 = { '%', {0.2, 0.2, 0.2, 0.2}, PICO_ANCHOR_C, NULL };
        Pico_Rel_Rect r2 = { '%', {0.8, 0.8, 0.2, 0.2}, PICO_ANCHOR_C, NULL };
        int result = pico_vs_rect_rect(&r1, &r2);
        printf("rect1 (%.1f, %.1f, %.1f, %.1f) vs rect2 (%.1f, %.1f, %.1f, %.1f): %d\n",
               r1.x, r1.y, r1.w, r1.h, r2.x, r2.y, r2.w, r2.h, result);
        assert(result == 0);
    }

    pico_init(0);
    return 0;
}
