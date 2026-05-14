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
        Pico_Rel_Rect r = { '!', {10, 10, 20, 20}, PICO_ANCHOR_NW };
        pico_output_draw_rect(r);
        pico_set_layer("window");
        const char* f = pico_output_screenshot(NULL, NULL);
        pico_set_layer("world");
        assert(f != NULL);
        check(f, "asr/shot-01.png");
        assert(remove(f) == 0);
    }

    {
        puts("entire screen - 02");
        pico_set_pencil_color((Pico_Color){200, 0, 0, 0xFF});
        Pico_Rel_Rect r = { '!', {30, 30, 10, 10}, PICO_ANCHOR_NW };
        pico_output_draw_rect(r);
        pico_set_layer("window");
        const char* f = pico_output_screenshot("out/shot-02.png", NULL);
        pico_set_layer("world");
        assert(!strcmp(f, "out/shot-02.png"));
        check(f, "asr/shot-02.png");
    }

    {
        puts("part of screen (raw)");
        pico_set_pencil_color((Pico_Color){0, 200, 0, 0xFF});
        Pico_Rel_Rect r = { '!', {40, 5, 10, 10}, PICO_ANCHOR_NW };
        pico_output_draw_rect(r);
        Pico_Rel_Rect clip = { '!', {0, 0, 250, 150}, PICO_ANCHOR_NW };
        pico_set_layer("window");
        const char* f = pico_output_screenshot(NULL, &clip);
        pico_set_layer("world");
        assert(f != NULL);
        check(f, "asr/shot-03.png");
        assert(remove(f) == 0);
    }

    {
        puts("part of screen (pct)");
        pico_set_pencil_color((Pico_Color){0, 0, 200, 0xFF});
        pico_output_draw_rect (
            (Pico_Rel_Rect) { '!', {50, 50, 10, 10}, PICO_ANCHOR_NW }
        );
        pico_set_layer("window");
        const char* f = pico_output_screenshot(NULL,
            &(Pico_Rel_Rect) { '%', {0, 0, 0.5, 0.3}, PICO_ANCHOR_NW }
        );
        pico_set_layer("world");
        assert(f != NULL);
        check(f, "asr/shot-04.png");
        assert(remove(f) == 0);
    }

    {
        puts("world layer");
        pico_set_effect_color((Pico_Color){0x10, 0x10, 0x10, 0xFF});
        pico_output_clear();
        pico_set_pencil_color((Pico_Color){0xFF, 0x00, 0x00, 0xFF});
        pico_output_draw_rect (
            (Pico_Rel_Rect) { '!', {10, 10, 30, 30}, PICO_ANCHOR_NW }
        );
        const char* f = pico_output_screenshot("out/shot-world.png", NULL);
        assert(!strcmp(f, "out/shot-world.png"));
        check(f, "asr/shot-world.png");
    }

    {
        puts("empty layer");
        pico_layer_empty(NULL, "empty1", 1, (Pico_Rel_Dim){'!', {64, 32}}, NULL);
        pico_set_layer("empty1");
        pico_set_effect_color((Pico_Color){0x00, 0x80, 0x00, 0xFF});
        pico_output_clear();
        pico_set_pencil_color((Pico_Color){0xFF, 0xFF, 0x00, 0xFF});
        pico_output_draw_rect (
            (Pico_Rel_Rect) { '!', {5, 5, 10, 10}, PICO_ANCHOR_NW }
        );
        const char* f = pico_output_screenshot("out/shot-empty.png", NULL);
        pico_set_layer("world");
        assert(!strcmp(f, "out/shot-empty.png"));
        check(f, "asr/shot-empty.png");
    }

    {
        puts("pixmap layer");
        static Pico_Color buf[4] = {
            {255,   0,   0, 255}, {  0, 255,   0, 255},
            {  0,   0, 255, 255}, {255, 255,   0, 255}
        };
        pico_layer_pixmap(NULL, "pmap1", (Pico_Abs_Dim){2, 2}, buf);
        pico_set_layer("pmap1");
        const char* f = pico_output_screenshot("out/shot-pixmap.png", NULL);
        pico_set_layer("world");
        assert(!strcmp(f, "out/shot-pixmap.png"));
        check(f, "asr/shot-pixmap.png");
    }

    {
        puts("sub layer");
        pico_layer_sub(NULL, "sub1", "empty1",
            &(Pico_Rel_Rect) { '!', {0, 0, 32, 16}, PICO_ANCHOR_NW }
        );
        pico_set_layer("sub1");
        const char* f = pico_output_screenshot("out/shot-sub.png", NULL);
        pico_set_layer("world");
        assert(!strcmp(f, "out/shot-sub.png"));
        check(f, "asr/shot-sub.png");
    }

    {
        puts("image layer");
        pico_layer_image(NULL, "img1", "../res/open.png");
        pico_set_layer("img1");
        const char* f = pico_output_screenshot("out/shot-image.png", NULL);
        pico_set_layer("world");
        assert(!strcmp(f, "out/shot-image.png"));
        check(f, "asr/shot-image.png");
    }

    {
        puts("text layer");
        pico_set_pencil_color((Pico_Color){0xFF, 0xFF, 0xFF, 0xFF});
        pico_layer_text(NULL, "txt1", 16, "hello");
        pico_set_layer("txt1");
        const char* f = pico_output_screenshot("out/shot-text.png", NULL);
        pico_set_layer("world");
        assert(!strcmp(f, "out/shot-text.png"));
        check(f, "asr/shot-text.png");
    }

    {
        puts("video layer");
        pico_layer_video(NULL, "vid1", "video.y4m");
        pico_set_layer("vid1");
        assert(pico_set_video("vid1", 0) == 1);
        const char* f = pico_output_screenshot("out/shot-video.png", NULL);
        pico_set_layer("world");
        assert(!strcmp(f, "out/shot-video.png"));
        check(f, "asr/shot-video.png");
    }

    pico_init(0);
    return 0;
}
