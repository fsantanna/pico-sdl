#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_window((Pico_Window){ .dim={640,480}, .fs=0, .show=1, .title="Colors Test" });
    pico_set_view_dim(NULL, &(Pico_Rel_Dim){ '!', {64, 48}, NULL });

    Pico_Anchor C = PICO_ANCHOR_C;

    puts("predefined colors");
    {
        pico_output_clear();

        pico_set_draw_color(NULL, PICO_COLOR_RED);
        pico_output_draw_rect(&(Pico_Rel_Rect){ '%', {0.5, 0.1, 0.8, 0.05}, C, NULL });

        pico_set_draw_color(NULL, PICO_COLOR_GREEN);
        pico_output_draw_rect(&(Pico_Rel_Rect){ '%', {0.5, 0.2, 0.8, 0.05}, C, NULL });

        pico_set_draw_color(NULL, PICO_COLOR_BLUE);
        pico_output_draw_rect(&(Pico_Rel_Rect){ '%', {0.5, 0.3, 0.8, 0.05}, C, NULL });

        pico_set_draw_color(NULL, PICO_COLOR_YELLOW);
        pico_output_draw_rect(&(Pico_Rel_Rect){ '%', {0.5, 0.4, 0.8, 0.05}, C, NULL });

        pico_set_draw_color(NULL, PICO_COLOR_CYAN);
        pico_output_draw_rect(&(Pico_Rel_Rect){ '%', {0.5, 0.5, 0.8, 0.05}, C, NULL });

        pico_set_draw_color(NULL, PICO_COLOR_MAGENTA);
        pico_output_draw_rect(&(Pico_Rel_Rect){ '%', {0.5, 0.6, 0.8, 0.05}, C, NULL });

        pico_set_draw_color(NULL, PICO_COLOR_ORANGE);
        pico_output_draw_rect(&(Pico_Rel_Rect){ '%', {0.5, 0.7, 0.8, 0.05}, C, NULL });

        pico_set_draw_color(NULL, PICO_COLOR_PURPLE);
        pico_output_draw_rect(&(Pico_Rel_Rect){ '%', {0.5, 0.8, 0.8, 0.05}, C, NULL });

        pico_set_draw_color(NULL, PICO_COLOR_PINK);
        pico_output_draw_rect(&(Pico_Rel_Rect){ '%', {0.5, 0.9, 0.8, 0.05}, C, NULL });

        _pico_check("colors-01");
    }

    puts("darker function");
    {
        pico_output_clear();
        for (int i=0; i<9; i++) {
            Pico_Color c = pico_color_darker(PICO_COLOR_RED, i*0.1);
            pico_set_draw_color(NULL, c);
            pico_output_draw_rect(
                &(Pico_Rel_Rect){ '%', {0.5, 0.1+i*0.1, 0.8, 0.05}, PICO_ANCHOR_C, NULL }
            );
        }
        _pico_check("colors-02");
    }

    puts("ligher function");
    {
        pico_output_clear();
        for (int i=0; i<9; i++) {
            Pico_Color c = pico_color_lighter(PICO_COLOR_BLUE, i*0.1);
            pico_set_draw_color(NULL, c);
            pico_output_draw_rect(
                &(Pico_Rel_Rect){ '%', {0.5, 0.1+i*0.1, 0.8, 0.05}, PICO_ANCHOR_C, NULL }
            );
        }
        _pico_check("colors-03");
    }

    {
        puts("negative percentages");
        pico_output_clear();

        // -dark -> light
        pico_set_draw_color(NULL, pico_color_darker(PICO_COLOR_GREEN, -0.50));
        pico_output_draw_rect(
            &(Pico_Rel_Rect){ '%', {0.5, 0.25, 0.5, 0.15}, PICO_ANCHOR_C, NULL }
        );

        // green
        pico_set_draw_color(NULL, PICO_COLOR_GREEN);
        pico_output_draw_rect(
            &(Pico_Rel_Rect){ '%', {0.5, 0.50, 0.5, 0.15}, PICO_ANCHOR_C, NULL }
        );

        // -light -> dark
        pico_set_draw_color(NULL, pico_color_lighter(PICO_COLOR_GREEN, -0.50));
        pico_output_draw_rect(
            &(Pico_Rel_Rect){ '%', {0.5, 0.75, 0.5, 0.15}, PICO_ANCHOR_C, NULL }
        );

        _pico_check("colors-04");
    }

    puts("mix function");
    {
        pico_output_clear();

        // red + blue = purple
        Pico_Color c1 = pico_color_mix(PICO_COLOR_RED, PICO_COLOR_BLUE);
        pico_set_draw_color(NULL, c1);
        pico_output_draw_rect(
            &(Pico_Rel_Rect){ '%', {0.5, 0.15, 0.8, 0.1}, PICO_ANCHOR_C, NULL }
        );

        // red + green = olive
        Pico_Color c2 = pico_color_mix(PICO_COLOR_RED, PICO_COLOR_GREEN);
        pico_set_draw_color(NULL, c2);
        pico_output_draw_rect(
            &(Pico_Rel_Rect){ '%', {0.5, 0.35, 0.8, 0.1}, PICO_ANCHOR_C, NULL }
        );

        // blue + green = teal
        Pico_Color c3 = pico_color_mix(PICO_COLOR_BLUE, PICO_COLOR_GREEN);
        pico_set_draw_color(NULL, c3);
        pico_output_draw_rect(
            &(Pico_Rel_Rect){ '%', {0.5, 0.55, 0.8, 0.1}, PICO_ANCHOR_C, NULL }
        );

        // black + white = gray
        Pico_Color c4 = pico_color_mix(PICO_COLOR_BLACK, PICO_COLOR_WHITE);
        pico_set_draw_color(NULL, c4);
        pico_output_draw_rect(
            &(Pico_Rel_Rect){ '%', {0.5, 0.75, 0.8, 0.1}, PICO_ANCHOR_C, NULL }
        );

        _pico_check("colors-05");
    }

    // mirrors Lua test for '%' and '!' color table formats
    {
        puts("percentage color format");
        pico_output_clear();

        // {'%', r=1, g=0, b=0} -> (255, 0, 0)
        pico_set_draw_color(NULL, (Pico_Color){255, 0, 0, 0xFF});
        pico_output_draw_rect(
            &(Pico_Rel_Rect){ '%', {0.5, 0.15, 0.8, 0.1}, C, NULL }
        );

        // {'%', r=0, g=1, b=0} -> (0, 255, 0)
        pico_set_draw_color(NULL, (Pico_Color){0, 255, 0, 0xFF});
        pico_output_draw_rect(
            &(Pico_Rel_Rect){ '%', {0.5, 0.30, 0.8, 0.1}, C, NULL }
        );

        // {'%', r=0, g=0, b=1} -> (0, 0, 255)
        pico_set_draw_color(NULL, (Pico_Color){0, 0, 255, 0xFF});
        pico_output_draw_rect(
            &(Pico_Rel_Rect){ '%', {0.5, 0.45, 0.8, 0.1}, C, NULL }
        );

        // {'%', r=0.5, g=0.5, b=0.5} -> (127, 127, 127)
        pico_set_draw_color(NULL, (Pico_Color){127, 127, 127, 0xFF});
        pico_output_draw_rect(
            &(Pico_Rel_Rect){ '%', {0.5, 0.60, 0.8, 0.1}, C, NULL }
        );

        // {'!', r=255, g=255, b=0} -> (255, 255, 0)
        pico_set_draw_color(NULL, (Pico_Color){255, 255, 0, 0xFF});
        pico_output_draw_rect(
            &(Pico_Rel_Rect){ '%', {0.5, 0.75, 0.8, 0.1}, C, NULL }
        );

        // {r=0, g=255, b=255} -> (0, 255, 255)
        pico_set_draw_color(NULL, (Pico_Color){0, 255, 255, 0xFF});
        pico_output_draw_rect(
            &(Pico_Rel_Rect){ '%', {0.5, 0.90, 0.8, 0.1}, C, NULL }
        );

        _pico_check("colors-06");
    }

    puts("hex color format");
    {
        pico_output_clear();

        pico_set_draw_color(NULL, pico_color_hex(0xFF0000));
        pico_output_draw_rect(&(Pico_Rel_Rect){ '%', {0.5, 0.15, 0.8, 0.1}, C, NULL });

        pico_set_draw_color(NULL, pico_color_hex(0x00FF00));
        pico_output_draw_rect(&(Pico_Rel_Rect){ '%', {0.5, 0.30, 0.8, 0.1}, C, NULL });

        pico_set_draw_color(NULL, pico_color_hex(0x0000FF));
        pico_output_draw_rect(&(Pico_Rel_Rect){ '%', {0.5, 0.45, 0.8, 0.1}, C, NULL });

        pico_set_draw_color(NULL, pico_color_hex(0x7F7F7F));
        pico_output_draw_rect(&(Pico_Rel_Rect){ '%', {0.5, 0.60, 0.8, 0.1}, C, NULL });

        pico_set_draw_color(NULL, pico_color_hex(0xFFFF00));
        pico_output_draw_rect(&(Pico_Rel_Rect){ '%', {0.5, 0.75, 0.8, 0.1}, C, NULL });

        pico_set_draw_color(NULL, pico_color_hex(0x00FFFF));
        pico_output_draw_rect(&(Pico_Rel_Rect){ '%', {0.5, 0.90, 0.8, 0.1}, C, NULL });

        _pico_check("colors-06");
    }

    puts("hex color conversion");
    {
        Pico_Color c;

        c = pico_color_hex(0xFF0000);
        assert(c.r==255 && c.g==0 && c.b==0);

        c = pico_color_hex(0x00FF00);
        assert(c.r==0 && c.g==255 && c.b==0);

        c = pico_color_hex(0x0000FF);
        assert(c.r==0 && c.g==0 && c.b==255);

        c = pico_color_hex(0x336699);
        assert(c.r==0x33 && c.g==0x66 && c.b==0x99);

        c = pico_color_hex(0x000000);
        assert(c.r==0 && c.g==0 && c.b==0);

        c = pico_color_hex(0xFFFFFF);
        assert(c.r==255 && c.g==255 && c.b==255);

        c = pico_color_hex(0xFF000080);
        assert(c.r==255 && c.g==0 && c.b==0 && c.a==0x80);

        c = pico_color_hex(0x33669900);
        assert(c.r==0x33 && c.g==0x66 && c.b==0x99 && c.a==0);

        c = pico_color_hex(0xFFFFFFFF);
        assert(c.r==255 && c.g==255 && c.b==255 && c.a==255);
    }

    puts("color alpha function");
    {
        pico_set_show_color(NULL, PICO_COLOR_WHITE);
        pico_output_clear();
        Pico_Color buffer[] = {
            pico_color_alpha(PICO_COLOR_RED, 0xFF),
            pico_color_alpha(PICO_COLOR_RED, 0x80),
            PICO_COLOR_TRANSPARENT,
        };
        Pico_Rel_Rect r = {
            '%', {0.5, 0.5, 0, 0}, PICO_ANCHOR_C, NULL
        };
        pico_output_draw_buffer(
            "alpha", (Pico_Abs_Dim){3,1}, buffer, &r
        );
        _pico_check("colors-07");
    }

    pico_init(0);
    return 0;
}
