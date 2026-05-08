#include "pico.h"
#include <string.h>
#include "../check.h"

int main (void) {
    pico_init(1);

    // initial current layer is world (not window)
    {
        puts("init: current layer is world");
        assert(!strcmp(pico_get_layer(), "world"));
    }

    // switching to the predefined window layer
    {
        puts("set.layer(\"window\")");
        const char* prev = pico_set_layer("window");
        assert(!strcmp(prev, "world"));
        assert(!strcmp(pico_get_layer(), "window"));
        pico_set_layer("world");
    }

    // dims: window=500x500 (physical), world=100x100 (logical)
    {
        puts("dims: window vs world");
        pico_set_layer("window");
        Pico_Abs_Dim w = pico_get_scene_dim();
        assert(w.w == 500 && w.h == 500);
        pico_set_layer("world");
        Pico_Abs_Dim l = pico_get_scene_dim();
        assert(l.w == 100 && l.h == 100);
    }

    // drawing: window direct red rect (NW-ish) + world blue (SE-ish).
    // Both in pct mode, centered, 40% × 40%:
    //   red  rect on window at (33%, 33%)
    //   blue world dst         at (66%, 66%)
    // The two regions slightly overlap; world composites over red in
    // the overlap.
    {
        puts("window red @33% + world blue @66%");

        // configure world: blue, dst at (66%, 66%)
        pico_set_layer("world");
        pico_set_scene_dst (
            (Pico_Rel_Rect){'%', {0.66, 0.66, 0.4, 0.4}, PICO_ANCHOR_C}
        );
        pico_set_effect_color((Pico_Color){0x00, 0x00, 0xFF, 0xFF});
        pico_output_clear();

        // direct draw on window: gray bg + red rect at (33%, 33%)
        pico_set_layer("window");
        pico_set_effect_color((Pico_Color){0x80, 0x80, 0x80, 0xFF});
        pico_output_clear();
        pico_set_pencil_color((Pico_Color){0xFF, 0x00, 0x00, 0xFF});
        pico_output_draw_rect (
            &(Pico_Rel_Rect){'%', {0.33, 0.33, 0.4, 0.4}, PICO_ANCHOR_C}
        );

        _pico_check("window-01");
        pico_set_layer("world");
    }

    pico_init(0);
    return 0;
}
