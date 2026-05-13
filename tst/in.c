#include "pico.h"
#include <stdio.h>
#include <assert.h>

int main (void) {
    pico_init(1);
    Pico_Rel_Dim log = { '!', {100, 100} };
    pico_set_scene_dim(&log);

    // RECT IN RECT - % C inside ! NW (composition)
    {
        puts("rect in rect - % C in ! NW");
        Pico_Rel_Rect out = { '!', {20, 20, 60, 60}, PICO_ANCHOR_NW };
        Pico_Rel_Rect in  = { '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C };
        Pico_Rel_Rect ret = pico_in_rect(out, in);
        Pico_Rel_Rect abs = { '!', {0, 0, 0, 0}, PICO_ANCHOR_NW };
        pico_cv_rect(NULL, &abs, NULL, &ret);
        assert((int)(abs.x+0.5f)==35 && (int)(abs.y+0.5f)==35);
        assert((int)(abs.w+0.5f)==30 && (int)(abs.h+0.5f)==30);
    }

    // POS IN RECT - % NW
    {
        puts("pos in rect - % NW");
        Pico_Rel_Rect out = { '!', {20, 20, 60, 60}, PICO_ANCHOR_NW };
        Pico_Rel_Pos  in  = { '%', {0.5, 0.5}, PICO_ANCHOR_NW };
        Pico_Rel_Pos  ret = pico_in_pos(out, in);
        Pico_Rel_Pos abs = { '!', {0, 0}, PICO_ANCHOR_NW };
        pico_cv_pos(NULL, &abs, NULL, &ret);
        assert((int)(abs.x+0.5f)==50 && (int)(abs.y+0.5f)==50);
    }

    // DIM IN RECT - %
    {
        puts("dim in rect - %");
        Pico_Rel_Rect out = { '!', {20, 20, 60, 60}, PICO_ANCHOR_NW };
        Pico_Rel_Dim  in  = { '%', {0.5, 0.5} };
        Pico_Rel_Dim  ret = pico_in_dim(out, in);
        Pico_Rel_Dim abs = { '!', {0, 0} };
        pico_cv_dim(NULL, &abs, NULL, &ret);
        assert((int)(abs.w+0.5f)==30 && (int)(abs.h+0.5f)==30);
    }

    // MODE / ANCHOR PRESERVED FROM in
    {
        puts("mode and anchor preserved");
        Pico_Rel_Rect out = { '!', {20, 20, 60, 60}, PICO_ANCHOR_NW };
        Pico_Rel_Rect in  = { '%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C };
        Pico_Rel_Rect ret = pico_in_rect(out, in);
        assert(ret.mode == in.mode);
        assert(ret.anchor.x == in.anchor.x);
        assert(ret.anchor.y == in.anchor.y);
    }

    printf("\n=== ALL TESTS PASSED ===\n");
    pico_init(0);
    return 0;
}
