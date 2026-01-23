#include "pico.h"
#include "../check.h"

void check (const char* out, const char* asr) {
    printf("Testing: %s\n", asr);
    assert(_pico_cmp_files(out, asr));
}

int main (void) {
    pico_init(1);
    pico_output_clear();

    {
        puts("entire screen - 01");
        pico_output_draw_rect_raw((Pico_Rect){10, 10, 20, 20});
        const char* f = pico_output_screenshot(NULL);
        assert(f != NULL);
        check(f, "asr/shot-01.png");
        assert(remove(f) == 0);
    }

    {
        puts("entire screen - 02");
        pico_set_color_draw((Pico_Color){200, 0, 0});
        pico_output_draw_rect_raw((Pico_Rect){30, 30, 10, 10});
        const char* f = pico_output_screenshot("out/shot-02.png");
        assert(!strcmp(f, "out/shot-02.png"));
        check(f, "asr/shot-02.png");
    }

    {
        puts("part of screen (raw)");
        pico_set_color_draw((Pico_Color){0, 200, 0});
        pico_output_draw_rect_raw((Pico_Rect){40, 5, 10, 10});
        const char* f = pico_output_screenshot_raw(NULL, (Pico_Rect){0, 0, 250, 150});
        assert(f != NULL);
        check(f, "asr/shot-03.png");
        assert(remove(f) == 0);
    }

    {
        puts("part of screen (pct)");
        pico_set_color_draw((Pico_Color){0, 0, 200});
        pico_output_draw_rect_raw((Pico_Rect){50, 50, 10, 10});
        const char* f = pico_output_screenshot_pct(NULL,
            &(Pico_Rect_Pct){0, 0, 0.5, 0.3, {.x=0,.y=0}, NULL});
        assert(f != NULL);
        check(f, "asr/shot-04.png");
        assert(remove(f) == 0);
    }

    pico_init(0);
    return 0;
}
