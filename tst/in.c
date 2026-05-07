#include "pico.h"
#include <stdio.h>
#include <assert.h>

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

int main (void) {
    pico_init(1);
    Pico_Rel_Dim log = { '!', {100, 100} };
    pico_set_scene_dim(NULL, &log);

    // RECT IN RECT - ! NW
    {
        puts("rect in rect - ! NW");
        Pico_Rel_Rect out = { '!', {20, 20, 60, 60}, PICO_ANCHOR_NW };
        Pico_Rel_Rect in  = { '!', {10, 10, 20, 20}, PICO_ANCHOR_NW };
        Pico_Rel_Rect ret = pico_in_rect(&out, &in);
        Pico_Abs_Rect abs = pico_cv_rect_rel_abs(&ret, NULL);
        printf("  ret: mode='%c' (%.2f,%.2f,%.2f,%.2f) abs (%d,%d,%d,%d)\n",
               ret.mode, ret.x, ret.y, ret.w, ret.h,
               abs.x, abs.y, abs.w, abs.h);
        assert(rect_eq(abs, (Pico_Abs_Rect){30, 30, 20, 20}));
    }

    // RECT IN RECT - % C inside ! NW
    {
        puts("rect in rect - % C in ! NW");
        Pico_Rel_Rect out = { '!', {20, 20, 60, 60}, PICO_ANCHOR_NW };
        Pico_Rel_Rect in  = { '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C };
        Pico_Rel_Rect ret = pico_in_rect(&out, &in);
        Pico_Abs_Rect abs = pico_cv_rect_rel_abs(&ret, NULL);
        printf("  ret: mode='%c' abs (%d,%d,%d,%d)\n",
               ret.mode, abs.x, abs.y, abs.w, abs.h);
        assert(rect_eq(abs, (Pico_Abs_Rect){35, 35, 30, 30}));
    }

    // RECT IN RECT - % NW inside % C (nested pct)
    {
        puts("rect in rect - % in %");
        Pico_Rel_Rect out = { '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C };
        Pico_Rel_Rect in  = { '%', {0.0, 0.0, 1.0, 1.0}, PICO_ANCHOR_NW };
        Pico_Rel_Rect ret = pico_in_rect(&out, &in);
        Pico_Abs_Rect abs = pico_cv_rect_rel_abs(&ret, NULL);
        Pico_Abs_Rect out_abs = pico_cv_rect_rel_abs(&out, NULL);
        printf("  out_abs (%d,%d,%d,%d) ret_abs (%d,%d,%d,%d)\n",
               out_abs.x, out_abs.y, out_abs.w, out_abs.h,
               abs.x, abs.y, abs.w, abs.h);
        assert(rect_eq(abs, out_abs));
    }

    // POS IN RECT - ! NW
    {
        puts("pos in rect - ! NW");
        Pico_Rel_Rect out = { '!', {20, 20, 60, 60}, PICO_ANCHOR_NW };
        Pico_Rel_Pos  in  = { '!', {10, 10}, PICO_ANCHOR_NW };
        Pico_Rel_Pos  ret = pico_in_pos(&out, &in);
        Pico_Abs_Pos  abs = pico_cv_pos_rel_abs(&ret, NULL);
        printf("  ret: mode='%c' abs (%d,%d)\n", ret.mode, abs.x, abs.y);
        assert(pos_eq(abs, (Pico_Abs_Pos){30, 30}));
    }

    // POS IN RECT - % NW
    {
        puts("pos in rect - % NW");
        Pico_Rel_Rect out = { '!', {20, 20, 60, 60}, PICO_ANCHOR_NW };
        Pico_Rel_Pos  in  = { '%', {0.5, 0.5}, PICO_ANCHOR_NW };
        Pico_Rel_Pos  ret = pico_in_pos(&out, &in);
        Pico_Abs_Pos  abs = pico_cv_pos_rel_abs(&ret, NULL);
        printf("  ret: mode='%c' abs (%d,%d)\n", ret.mode, abs.x, abs.y);
        assert(pos_eq(abs, (Pico_Abs_Pos){50, 50}));
    }

    // DIM IN RECT - !
    {
        puts("dim in rect - !");
        Pico_Rel_Rect out = { '!', {20, 20, 60, 60}, PICO_ANCHOR_NW };
        Pico_Rel_Dim  in  = { '!', {30, 20} };
        Pico_Rel_Dim  ret = pico_in_dim(&out, &in);
        Pico_Abs_Dim  abs = pico_cv_dim_rel_abs(&ret, NULL);
        printf("  ret: mode='%c' abs (%d,%d)\n", ret.mode, abs.w, abs.h);
        assert(dim_eq(abs, (Pico_Abs_Dim){30, 20}));
    }

    // DIM IN RECT - %
    {
        puts("dim in rect - %");
        Pico_Rel_Rect out = { '!', {20, 20, 60, 60}, PICO_ANCHOR_NW };
        Pico_Rel_Dim  in  = { '%', {0.5, 0.5} };
        Pico_Rel_Dim  ret = pico_in_dim(&out, &in);
        Pico_Abs_Dim  abs = pico_cv_dim_rel_abs(&ret, NULL);
        printf("  ret: mode='%c' abs (%d,%d)\n", ret.mode, abs.w, abs.h);
        assert(dim_eq(abs, (Pico_Abs_Dim){30, 30}));
    }

    // MODE / ANCHOR PRESERVED FROM in
    {
        puts("mode and anchor preserved");
        Pico_Rel_Rect out = { '!', {20, 20, 60, 60}, PICO_ANCHOR_NW };
        Pico_Rel_Rect in  = { '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C };
        Pico_Rel_Rect ret = pico_in_rect(&out, &in);
        assert(ret.mode == in.mode);
        assert(ret.anchor.x == in.anchor.x);
        assert(ret.anchor.y == in.anchor.y);
    }

    // EQUIVALENCE: pico_in_rect matches old up-chain semantics
    // old: in.up = &out  → pico_cv_rect_rel_abs(&in, NULL) walks chain
    // new: ret = pico_in_rect(&out, &in) → pico_cv_rect_rel_abs(&ret, NULL)
    {
        puts("equivalence: in_rect == cv with out_abs as base");
        Pico_Rel_Rect out = { '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C };
        Pico_Rel_Rect in  = { '!', {5, 5, 10, 10}, PICO_ANCHOR_NW };
        Pico_Abs_Rect out_abs = pico_cv_rect_rel_abs(&out, NULL);
        Pico_Abs_Rect old_way = pico_cv_rect_rel_abs(&in, &out_abs);
        Pico_Rel_Rect ret = pico_in_rect(&out, &in);
        Pico_Abs_Rect new_way = pico_cv_rect_rel_abs(&ret, NULL);
        printf("  old (%d,%d,%d,%d) new (%d,%d,%d,%d)\n",
               old_way.x, old_way.y, old_way.w, old_way.h,
               new_way.x, new_way.y, new_way.w, new_way.h);
        assert(rect_eq(old_way, new_way));
    }

    // EQUIVALENCE for pos
    {
        puts("equivalence: in_pos == cv_pos with out_abs as base");
        Pico_Rel_Rect out = { '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C };
        Pico_Rel_Pos  in  = { '!', {10, 10}, PICO_ANCHOR_NW };
        Pico_Abs_Rect out_abs = pico_cv_rect_rel_abs(&out, NULL);
        Pico_Abs_Pos  old_way = pico_cv_pos_rel_abs(&in, &out_abs);
        Pico_Rel_Pos  ret = pico_in_pos(&out, &in);
        Pico_Abs_Pos  new_way = pico_cv_pos_rel_abs(&ret, NULL);
        assert(pos_eq(old_way, new_way));
    }

    // EQUIVALENCE for dim
    {
        puts("equivalence: in_dim == cv_dim with out_abs as base");
        Pico_Rel_Rect out = { '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C };
        Pico_Rel_Dim  in  = { '%', {0.5, 0.5} };
        Pico_Abs_Rect out_abs = pico_cv_rect_rel_abs(&out, NULL);
        Pico_Abs_Dim  old_way = pico_cv_dim_rel_abs(&in, &out_abs);
        Pico_Rel_Dim  ret = pico_in_dim(&out, &in);
        Pico_Abs_Dim  new_way = pico_cv_dim_rel_abs(&ret, NULL);
        assert(dim_eq(old_way, new_way));
    }

    printf("\n=== ALL TESTS PASSED ===\n");
    pico_init(0);
    return 0;
}
