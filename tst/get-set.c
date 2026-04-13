#include "pico.h"
#include <assert.h>
#include <string.h>

int main (void) {
    pico_init(1);
    pico_set_window("Get-Set", -1, &(Pico_Rel_Dim){ '!', {640, 480}, NULL });
    pico_set_view_dim(NULL, &(Pico_Rel_Dim){ '!', {64, 48}, NULL });

    // color_clear
    puts("color_clear");
    {
        pico_set_show_color(NULL, PICO_COLOR_RED);
        Pico_Color c = pico_get_show_color(NULL);
        assert(c.r == 255 && c.g == 0 && c.b == 0);
        pico_set_show_color(NULL, PICO_COLOR_BLUE);
        c = pico_get_show_color(NULL);
        assert(c.r == 0 && c.g == 0 && c.b == 255);
        pico_set_show_color(NULL, PICO_COLOR_BLACK);
        c = pico_get_show_color(NULL);
        assert(c.r == 0 && c.g == 0 && c.b == 0);
    }

    // color_draw
    puts("color_draw");
    {
        pico_set_draw_color(NULL, PICO_COLOR_GREEN);
        Pico_Color c = pico_get_draw_color(NULL);
        assert(c.r == 0 && c.g == 255 && c.b == 0);
        pico_set_draw_color(NULL, PICO_COLOR_YELLOW);
        c = pico_get_draw_color(NULL);
        assert(c.r == 255 && c.g == 255 && c.b == 0);
        pico_set_draw_color(NULL, PICO_COLOR_WHITE);
        c = pico_get_draw_color(NULL);
        assert(c.r == 255 && c.g == 255 && c.b == 255);
    }

    // draw_style
    puts("draw_style");
    {
        pico_set_draw_style(NULL, PICO_STYLE_FILL);
        assert(pico_get_draw_style(NULL) == PICO_STYLE_FILL);
        pico_set_draw_style(NULL, PICO_STYLE_STROKE);
        assert(pico_get_draw_style(NULL) == PICO_STYLE_STROKE);
        pico_set_draw_style(NULL, PICO_STYLE_FILL);
        assert(pico_get_draw_style(NULL) == PICO_STYLE_FILL);
    }

    // draw (bulk setter/getter)
    puts("draw bulk");
    {
        Pico_Color c = PICO_COLOR_RED;
        const char* f = "test.ttf";
        PICO_STYLE s = PICO_STYLE_STROKE;
        pico_set_draw(NULL, &c, &f, &s);

        Pico_Color gc;
        const char* gf;
        PICO_STYLE gs;
        pico_get_draw(NULL, &gc, &gf, &gs);
        assert(gc.r == 255 && gc.g == 0 && gc.b == 0);
        assert(strcmp(gf, "test.ttf") == 0);
        assert(gs == PICO_STYLE_STROKE);

        // restore
        pico_set_draw(NULL, &(Pico_Color){0xFF,0xFF,0xFF,0xFF}, &(const char*){NULL}, &(PICO_STYLE){PICO_STYLE_FILL});
    }

    // show (individual setters)
    puts("show individual");
    {
        pico_set_show_alpha(NULL, 0x80);
        assert(pico_get_show_alpha(NULL) == 0x80);
        pico_set_show_alpha(NULL, 0xFF);

        pico_set_show_grid(NULL, 1);
        assert(pico_get_show_grid(NULL) == 1);
        pico_set_show_grid(NULL, 0);
        assert(pico_get_show_grid(NULL) == 0);

        pico_set_show_flip(NULL, PICO_FLIP_H);
        assert(pico_get_show_flip(NULL) == PICO_FLIP_H);
        pico_set_show_flip(NULL, PICO_FLIP_NONE);

        Pico_Rot rot = {45, PICO_ANCHOR_C};
        pico_set_show_rotation(NULL, rot);
        Pico_Rot got = pico_get_show_rotation(NULL);
        assert(got.angle == 45);
        pico_set_show_rotation(NULL, (Pico_Rot){0, PICO_ANCHOR_C});
    }

    // show (bulk setter/getter)
    puts("show bulk");
    {
        unsigned char a = 0x40;
        Pico_Color c = PICO_COLOR_RED;
        PICO_FLIP f = PICO_FLIP_V;
        Pico_Rot r = {90, PICO_ANCHOR_C};
        pico_set_show(NULL, &a, &c, &f, 1, -1, &r);

        unsigned char ga;
        Pico_Color gc;
        PICO_FLIP gf;
        int gg;
        int gk;
        Pico_Rot gr;
        pico_get_show(NULL, &ga, &gc, &gf, &gg, &gk, &gr);
        assert(ga == 0x40);
        assert(gc.r == 255 && gc.g == 0 && gc.b == 0);
        assert(gf == PICO_FLIP_V);
        assert(gg == 1);
        assert(gk == 0);
        assert(gr.angle == 90);

        // restore
        pico_set_show(NULL, &(unsigned char){0xFF}, &(Pico_Color){0,0,0,0xFF}, &(PICO_FLIP){PICO_FLIP_NONE}, 0, -1, &(Pico_Rot){0, PICO_ANCHOR_C});
    }

    puts("OK");
    pico_init(0);
    return 0;
}
