#include "pico.h"
#include <assert.h>
#include <string.h>

int main (void) {
    pico_init(1);
    pico_set_window((Pico_Window){ .fs=0, .show=1, .title="Get-Set" });

    pico_set_layer("window");
    pico_set_scene_dim((Pico_Rel_Dim){ '!', {640, 480} });
    pico_set_layer("world");
    pico_set_scene_dim((Pico_Rel_Dim){ '!', {64, 48} });

    // color_clear
    puts("color_clear");
    {
        pico_set_effect_color(PICO_COLOR_RED);
        Pico_Color c = pico_get_effect_color();
        assert(c.r == 255 && c.g == 0 && c.b == 0);
        pico_set_effect_color(PICO_COLOR_BLUE);
        c = pico_get_effect_color();
        assert(c.r == 0 && c.g == 0 && c.b == 255);
        pico_set_effect_color(PICO_COLOR_BLACK);
        c = pico_get_effect_color();
        assert(c.r == 0 && c.g == 0 && c.b == 0);
    }

    // color_pencil
    puts("color_pencil");
    {
        pico_set_pencil_color(PICO_COLOR_GREEN);
        Pico_Color c = pico_get_pencil_color();
        assert(c.r == 0 && c.g == 255 && c.b == 0);
        pico_set_pencil_color(PICO_COLOR_YELLOW);
        c = pico_get_pencil_color();
        assert(c.r == 255 && c.g == 255 && c.b == 0);
        pico_set_pencil_color(PICO_COLOR_WHITE);
        c = pico_get_pencil_color();
        assert(c.r == 255 && c.g == 255 && c.b == 255);
    }

    // pencil_style
    puts("pencil_style");
    {
        pico_set_pencil_style(PICO_STYLE_FILL);
        assert(pico_get_pencil_style() == PICO_STYLE_FILL);
        pico_set_pencil_style(PICO_STYLE_STROKE);
        assert(pico_get_pencil_style() == PICO_STYLE_STROKE);
        pico_set_pencil_style(PICO_STYLE_FILL);
        assert(pico_get_pencil_style() == PICO_STYLE_FILL);
    }

    // pencil (bulk setter/getter)
    puts("pencil bulk");
    {
        pico_set_pencil((Pico_Layer_Pencil){
            .color=PICO_COLOR_RED, .font="test.ttf", .style=PICO_STYLE_STROKE
        });

        Pico_Layer_Pencil gd;
        pico_get_pencil(&gd);
        assert(gd.color.r == 255 && gd.color.g == 0 && gd.color.b == 0);
        assert(strcmp(gd.font, "test.ttf") == 0);
        assert(gd.style == PICO_STYLE_STROKE);

        // restore
        pico_set_pencil((Pico_Layer_Pencil){
            .color={0xFF,0xFF,0xFF,0xFF}, .font=NULL, .style=PICO_STYLE_FILL
        });
    }

    // effect (individual setters)
    puts("effect individual");
    {
        pico_set_effect_alpha(0x80);
        assert(pico_get_effect_alpha() == 0x80);
        pico_set_effect_alpha(0xFF);

        pico_set_effect_grid(1);
        assert(pico_get_effect_grid() == 1);
        pico_set_effect_grid(0);
        assert(pico_get_effect_grid() == 0);

        pico_set_effect_flip(PICO_FLIP_HORIZONTAL);
        assert(pico_get_effect_flip() == PICO_FLIP_HORIZONTAL);
        pico_set_effect_flip(PICO_FLIP_NONE);

        Pico_Rot rot = {45, PICO_ANCHOR_C};
        pico_set_effect_rotate(rot);
        Pico_Rot got = pico_get_effect_rotate();
        assert(got.angle == 45);
        pico_set_effect_rotate((Pico_Rot){0, PICO_ANCHOR_C});
    }

    // effect (bulk setter/getter)
    puts("effect bulk");
    {
        pico_set_effect((Pico_Layer_Effect){
            .alpha=0x40, .color=PICO_COLOR_RED, .flip=PICO_FLIP_VERTICAL, .grid=1, .rotate={90, PICO_ANCHOR_C}
        });

        Pico_Layer_Effect gs;
        pico_get_effect(&gs);
        assert(gs.alpha == 0x40);
        assert(gs.color.r == 255 && gs.color.g == 0 && gs.color.b == 0);
        assert(gs.flip == PICO_FLIP_VERTICAL);
        assert(gs.grid == 1);
        assert(gs.rotate.angle == 90);

        // restore
        pico_set_effect((Pico_Layer_Effect){
            .alpha=0xFF, .color={0,0,0,0xFF}, .flip=PICO_FLIP_NONE, .grid=0, .rotate={0, PICO_ANCHOR_C}
        });
    }

    // window roundtrip: get -> set -> get must be idempotent
    puts("window roundtrip");
    {
        pico_set_layer("window");
        pico_set_effect_color((Pico_Color){0x12, 0x34, 0x56, 0x78});
        pico_set_layer("world");
        Pico_Window w1;
        pico_get_window(&w1);
        pico_set_window(w1);
        Pico_Window w2;
        pico_get_window(&w2);
        assert(w2.fs == w1.fs);
        Pico_Color c1;
        pico_set_layer("window");
        c1 = pico_get_effect_color();
        pico_set_layer("world");
        Pico_Abs_Dim d1;
        pico_set_layer("window");
        d1 = pico_get_scene_dim();
        pico_set_layer("world");
        assert(c1.r == 0x12 && c1.g == 0x34 && c1.b == 0x56 && c1.a == 0x78);
        assert(d1.w > 0 && d1.h > 0);
    }

    // scene roundtrip: get -> set -> get must be idempotent
    puts("scene roundtrip");
    {
        Pico_Layer_Scene v1;
        pico_get_scene(&v1);
        pico_set_scene(v1);
        Pico_Layer_Scene v2;
        pico_get_scene(&v2);
        assert(v2.dim.w == v1.dim.w && v2.dim.h == v1.dim.h);
        assert(v2.dst.mode == v1.dst.mode);
        assert(v2.src.mode == v1.src.mode);
        assert(v2.clip.mode == v1.clip.mode);
    }

    puts("OK");
    pico_init(0);
    return 0;
}
