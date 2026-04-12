#include "pico.h"
#include <assert.h>
#include <string.h>

int main (void) {
    pico_init(1);
    pico_set_window("Get-Set", -1, &(Pico_Rel_Dim){ '!', {640, 480}, NULL });
    pico_set_view(-1, &(Pico_Rel_Dim){ '!', {64, 48}, NULL }, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

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

    // style
    puts("style");
    {
        pico_set_draw_style(NULL, PICO_STYLE_FILL);
        assert(pico_get_draw_style(NULL) == PICO_STYLE_FILL);
        pico_set_draw_style(NULL, PICO_STYLE_STROKE);
        assert(pico_get_draw_style(NULL) == PICO_STYLE_STROKE);
        pico_set_draw_style(NULL, PICO_STYLE_FILL);
        assert(pico_get_draw_style(NULL) == PICO_STYLE_FILL);
    }

    puts("OK");
    pico_init(0);
    return 0;
}
