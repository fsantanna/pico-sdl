#include "pico.h"
#include <assert.h>
#include <string.h>

int main (void) {
    pico_init(1);
    pico_set_window((Pico_Window){ .dim={640,480}, .fs=0, .show=1, .title="Get-Set" });
    pico_set_scene_dim("root", &(Pico_Rel_Dim){ '!', {64, 48}, NULL });

    // color_clear
    puts("color_clear");
    {
        pico_set_effect_color("root", PICO_COLOR_RED);
        Pico_Color c = pico_get_effect_color("root");
        assert(c.r == 255 && c.g == 0 && c.b == 0);
        pico_set_effect_color("root", PICO_COLOR_BLUE);
        c = pico_get_effect_color("root");
        assert(c.r == 0 && c.g == 0 && c.b == 255);
        pico_set_effect_color("root", PICO_COLOR_BLACK);
        c = pico_get_effect_color("root");
        assert(c.r == 0 && c.g == 0 && c.b == 0);
    }

    // color_draw
    puts("color_draw");
    {
        pico_set_pencil_color("root", PICO_COLOR_GREEN);
        Pico_Color c = pico_get_pencil_color("root");
        assert(c.r == 0 && c.g == 255 && c.b == 0);
        pico_set_pencil_color("root", PICO_COLOR_YELLOW);
        c = pico_get_pencil_color("root");
        assert(c.r == 255 && c.g == 255 && c.b == 0);
        pico_set_pencil_color("root", PICO_COLOR_WHITE);
        c = pico_get_pencil_color("root");
        assert(c.r == 255 && c.g == 255 && c.b == 255);
    }

    // draw_style
    puts("draw_style");
    {
        pico_set_pencil_style("root", PICO_STYLE_FILL);
        assert(pico_get_pencil_style("root") == PICO_STYLE_FILL);
        pico_set_pencil_style("root", PICO_STYLE_STROKE);
        assert(pico_get_pencil_style("root") == PICO_STYLE_STROKE);
        pico_set_pencil_style("root", PICO_STYLE_FILL);
        assert(pico_get_pencil_style("root") == PICO_STYLE_FILL);
    }

    // draw (bulk setter/getter)
    puts("draw bulk");
    {
        pico_set_pencil("root", (Pico_Layer_Pencil){
            .color=PICO_COLOR_RED, .font="test.ttf", .style=PICO_STYLE_STROKE
        });

        Pico_Layer_Pencil gd;
        pico_get_pencil("root", &gd);
        assert(gd.color.r == 255 && gd.color.g == 0 && gd.color.b == 0);
        assert(strcmp(gd.font, "test.ttf") == 0);
        assert(gd.style == PICO_STYLE_STROKE);

        // restore
        pico_set_pencil("root", (Pico_Layer_Pencil){
            .color={0xFF,0xFF,0xFF,0xFF}, .font=NULL, .style=PICO_STYLE_FILL
        });
    }

    // show (individual setters)
    puts("show individual");
    {
        pico_set_effect_alpha("root", 0x80);
        assert(pico_get_effect_alpha("root") == 0x80);
        pico_set_effect_alpha("root", 0xFF);

        pico_set_effect_grid("root", 1);
        assert(pico_get_effect_grid("root") == 1);
        pico_set_effect_grid("root", 0);
        assert(pico_get_effect_grid("root") == 0);

        pico_set_effect_flip("root", PICO_FLIP_HORIZONTAL);
        assert(pico_get_effect_flip("root") == PICO_FLIP_HORIZONTAL);
        pico_set_effect_flip("root", PICO_FLIP_NONE);

        Pico_Rot rot = {45, PICO_ANCHOR_C};
        pico_set_effect_rotate("root", rot);
        Pico_Rot got = pico_get_effect_rotate("root");
        assert(got.angle == 45);
        pico_set_effect_rotate("root", (Pico_Rot){0, PICO_ANCHOR_C});
    }

    // show (bulk setter/getter)
    puts("show bulk");
    {
        pico_set_effect("root", (Pico_Layer_Effect){
            .alpha=0x40, .color=PICO_COLOR_RED, .flip=PICO_FLIP_VERTICAL, .grid=1, .rotate={90, PICO_ANCHOR_C}
        });

        Pico_Layer_Effect gs;
        pico_get_effect("root", &gs);
        assert(gs.alpha == 0x40);
        assert(gs.color.r == 255 && gs.color.g == 0 && gs.color.b == 0);
        assert(gs.flip == PICO_FLIP_VERTICAL);
        assert(gs.grid == 1);
        assert(gs.rotate.angle == 90);

        // restore
        pico_set_effect("root", (Pico_Layer_Effect){
            .alpha=0xFF, .color={0,0,0,0xFF}, .flip=PICO_FLIP_NONE, .grid=0, .rotate={0, PICO_ANCHOR_C}
        });
    }

    // window roundtrip: get -> set -> get must be idempotent
    puts("window roundtrip");
    {
        pico_set_window_color((Pico_Color){0x12, 0x34, 0x56, 0x78});
        Pico_Window w1;
        pico_get_window(&w1);
        pico_set_window(w1);
        Pico_Window w2;
        pico_get_window(&w2);
        assert(w2.dim.w   == w1.dim.w   && w2.dim.h == w1.dim.h);
        assert(w2.fs      == w1.fs);
        assert(w2.color.r == w1.color.r);
        assert(w2.color.g == w1.color.g);
        assert(w2.color.b == w1.color.b);
        assert(w2.color.a == w1.color.a);
    }

    // view roundtrip: get -> set -> get must be idempotent
    puts("view roundtrip");
    {
        Pico_Layer_Scene v1;
        pico_get_scene("root", &v1);
        pico_set_scene("root", v1);
        Pico_Layer_Scene v2;
        pico_get_scene("root", &v2);
        assert(v2.dim.w == v1.dim.w && v2.dim.h == v1.dim.h);
        assert(v2.dst.mode == v1.dst.mode);
        assert(v2.src.mode == v1.src.mode);
        assert(v2.clip.mode == v1.clip.mode);
    }

    puts("OK");
    pico_init(0);
    return 0;
}
