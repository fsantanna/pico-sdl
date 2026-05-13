#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_window((Pico_Window){ .fs=0, .show=1, .title="Style" });

    pico_set_layer("window");
    pico_set_scene_dim((Pico_Rel_Dim){ '!', {640, 480} });
    pico_set_layer("world");
    pico_set_scene_dim((Pico_Rel_Dim){ '!', {64, 48} });

    // shapes used across tests
    Pico_Rel_Rect rect = {
        '!', {5, 5, 25, 18}, PICO_ANCHOR_NW,
    };
    Pico_Rel_Rect oval = {
        '!', {34, 5, 25, 18}, PICO_ANCHOR_NW,
    };
    Pico_Rel_Pos t1 = {
        '!', {5, 26}, PICO_ANCHOR_NW,
    };
    Pico_Rel_Pos t2 = {
        '!', {5, 43}, PICO_ANCHOR_NW,
    };
    Pico_Rel_Pos t3 = {
        '!', {30, 43}, PICO_ANCHOR_NW,
    };
    Pico_Rel_Pos poly[] = {
        {'!', {34, 26}, PICO_ANCHOR_NW},
        {'!', {59, 30}, PICO_ANCHOR_NW},
        {'!', {50, 43}, PICO_ANCHOR_NW},
        {'!', {38, 43}, PICO_ANCHOR_NW},
    };

    puts("default fill");
    {
        pico_output_clear();
        pico_set_pencil_color((Pico_Color){255, 255, 255, 0xFF});
        pico_output_draw_rect(rect);
        pico_output_draw_oval(oval);
        pico_output_draw_tri(t1, t2, t3);
        pico_output_draw_poly(4, poly);
        _pico_check("style-01");
    }

    puts("stroke");
    {
        pico_output_clear();
        pico_set_pencil_style(PICO_STYLE_STROKE);
        pico_set_pencil_color((Pico_Color){255, 255, 255, 0xFF});
        pico_output_draw_rect(rect);
        pico_output_draw_oval(oval);
        pico_output_draw_tri(t1, t2, t3);
        pico_output_draw_poly(4, poly);
        _pico_check("style-02");
    }

    puts("back to fill");
    {
        pico_output_clear();
        pico_set_pencil_style(PICO_STYLE_FILL);
        pico_set_pencil_color((Pico_Color){255, 255, 255, 0xFF});
        pico_output_draw_rect(rect);
        pico_output_draw_oval(oval);
        pico_output_draw_tri(t1, t2, t3);
        pico_output_draw_poly(4, poly);
        _pico_check("style-03");
    }

    puts("get style");
    {
        pico_set_pencil_style(PICO_STYLE_FILL);
        assert(pico_get_pencil_style() == PICO_STYLE_FILL);
        pico_set_pencil_style(PICO_STYLE_STROKE);
        assert(pico_get_pencil_style() == PICO_STYLE_STROKE);
        pico_set_pencil_style(PICO_STYLE_FILL);
        assert(pico_get_pencil_style() == PICO_STYLE_FILL);
    }

    pico_init(0);
    return 0;
}
