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

// resolve any rel into cur's frame as integer pixels (NW origin)
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

static Pico_Abs_Dim _abs_dim (const Pico_Rel_Dim* d) {
    Pico_Rel_Dim out = { '!', {0, 0} };
    pico_cv_dim_to(NULL, d, &out);
    return (Pico_Abs_Dim){
        (int)(out.w + 0.5f), (int)(out.h + 0.5f),
    };
}

int main (void) {
    pico_init(1);
    Pico_Rel_Dim log = { '!', {100, 100} };
    pico_set_scene_dim(&log);

    // RECT IN RECT - ! NW
    {
        puts("rect in rect - ! NW");
        Pico_Rel_Rect out = { '!', {20, 20, 60, 60}, PICO_ANCHOR_NW };
        Pico_Rel_Rect in  = { '!', {10, 10, 20, 20}, PICO_ANCHOR_NW };
        Pico_Rel_Rect ret = pico_in_rect(&out, &in);
        SDL_Rect abs = _abs_rect(&ret);
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
        SDL_Rect abs = _abs_rect(&ret);
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
        SDL_Rect abs = _abs_rect(&ret);
        SDL_Rect out_abs = _abs_rect(&out);
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
        SDL_Point abs = _abs_pos(&ret);
        printf("  ret: mode='%c' abs (%d,%d)\n", ret.mode, abs.x, abs.y);
        assert(pos_eq(abs, (Pico_Abs_Pos){30, 30}));
    }

    // POS IN RECT - % NW
    {
        puts("pos in rect - % NW");
        Pico_Rel_Rect out = { '!', {20, 20, 60, 60}, PICO_ANCHOR_NW };
        Pico_Rel_Pos  in  = { '%', {0.5, 0.5}, PICO_ANCHOR_NW };
        Pico_Rel_Pos  ret = pico_in_pos(&out, &in);
        SDL_Point abs = _abs_pos(&ret);
        printf("  ret: mode='%c' abs (%d,%d)\n", ret.mode, abs.x, abs.y);
        assert(pos_eq(abs, (Pico_Abs_Pos){50, 50}));
    }

    // DIM IN RECT - !
    {
        puts("dim in rect - !");
        Pico_Rel_Rect out = { '!', {20, 20, 60, 60}, PICO_ANCHOR_NW };
        Pico_Rel_Dim  in  = { '!', {30, 20} };
        Pico_Rel_Dim  ret = pico_in_dim(&out, &in);
        Pico_Abs_Dim abs = _abs_dim(&ret);
        printf("  ret: mode='%c' abs (%d,%d)\n", ret.mode, abs.w, abs.h);
        assert(dim_eq(abs, (Pico_Abs_Dim){30, 20}));
    }

    // DIM IN RECT - %
    {
        puts("dim in rect - %");
        Pico_Rel_Rect out = { '!', {20, 20, 60, 60}, PICO_ANCHOR_NW };
        Pico_Rel_Dim  in  = { '%', {0.5, 0.5} };
        Pico_Rel_Dim  ret = pico_in_dim(&out, &in);
        Pico_Abs_Dim abs = _abs_dim(&ret);
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

    printf("\n=== ALL TESTS PASSED ===\n");
    pico_init(0);
    return 0;
}
