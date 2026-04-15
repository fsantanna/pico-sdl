#include "pico.h"
#include <assert.h>
#include <string.h>

int main (void) {
    pico_init(1);
    pico_set_window((Pico_Window){ .dim={640,480}, .fs=0, .show=1, .title="Get-Set" });
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
        pico_set_draw(NULL, (Pico_Layer_Draw){
            .color=PICO_COLOR_RED, .font="test.ttf", .style=PICO_STYLE_STROKE
        });

        Pico_Layer_Draw gd;
        pico_get_draw(NULL, &gd);
        assert(gd.color.r == 255 && gd.color.g == 0 && gd.color.b == 0);
        assert(strcmp(gd.font, "test.ttf") == 0);
        assert(gd.style == PICO_STYLE_STROKE);

        // restore
        pico_set_draw(NULL, (Pico_Layer_Draw){
            .color={0xFF,0xFF,0xFF,0xFF}, .font=NULL, .style=PICO_STYLE_FILL
        });
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

        pico_set_show_flip(NULL, PICO_FLIP_HORIZONTAL);
        assert(pico_get_show_flip(NULL) == PICO_FLIP_HORIZONTAL);
        pico_set_show_flip(NULL, PICO_FLIP_NONE);

        Pico_Rot rot = {45, PICO_ANCHOR_C};
        pico_set_show_rotate(NULL, rot);
        Pico_Rot got = pico_get_show_rotate(NULL);
        assert(got.angle == 45);
        pico_set_show_rotate(NULL, (Pico_Rot){0, PICO_ANCHOR_C});
    }

    // show (bulk setter/getter)
    puts("show bulk");
    {
        pico_set_show(NULL, (Pico_Layer_Show){
            .alpha=0x40, .color=PICO_COLOR_RED, .flip=PICO_FLIP_VERTICAL, .grid=1, .keep=0, .rotate={90, PICO_ANCHOR_C}
        });

        Pico_Layer_Show gs;
        pico_get_show(NULL, &gs);
        assert(gs.alpha == 0x40);
        assert(gs.color.r == 255 && gs.color.g == 0 && gs.color.b == 0);
        assert(gs.flip == PICO_FLIP_VERTICAL);
        assert(gs.grid == 1);
        assert(gs.keep == 0);
        assert(gs.rotate.angle == 90);

        // restore
        pico_set_show(NULL, (Pico_Layer_Show){
            .alpha=0xFF, .color={0,0,0,0xFF}, .flip=PICO_FLIP_NONE, .grid=0, .keep=0, .rotate={0, PICO_ANCHOR_C}
        });
    }

    puts("OK");
    pico_init(0);
    return 0;
}
