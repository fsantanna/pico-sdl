#include <SDL2/SDL.h>

#include "pico.h"
#include "../check.h"

void check (const char* out, const char* asr) {
    printf("Testing: %s\n", asr);
    assert(_pico_cmp_files(out, asr));
}

int main (void) {
    pico_init(1);
    pico_output_clear();

    // PICO_OUTPUT_SCREENSHOT

    {
        puts("entire screen - 01");
        Pico_Rel_Rect r = { '!', {10, 10, 20, 20}, PICO_ANCHOR_NW };
        pico_output_draw_rect(r);
        pico_set_layer("window");
        const char* f = pico_output_screenshot(NULL, NULL, NULL);
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
        const char* f = pico_output_screenshot(NULL, "out/shot-02.png", NULL);
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
        const char* f = pico_output_screenshot(NULL, NULL, &clip);
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
        const char* f = pico_output_screenshot(NULL, NULL,
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
        const char* f = pico_output_screenshot(NULL, "out/shot-world.png", NULL);
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
        const char* f = pico_output_screenshot(NULL, "out/shot-empty.png", NULL);
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
        const char* f = pico_output_screenshot(NULL, "out/shot-pixmap.png", NULL);
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
        const char* f = pico_output_screenshot(NULL, "out/shot-sub.png", NULL);
        pico_set_layer("world");
        assert(!strcmp(f, "out/shot-sub.png"));
        check(f, "asr/shot-sub.png");
    }

    {
        puts("image layer");
        pico_layer_image(NULL, "img1", "../res/open.png");
        pico_set_layer("img1");
        const char* f = pico_output_screenshot(NULL, "out/shot-image.png", NULL);
        pico_set_layer("world");
        assert(!strcmp(f, "out/shot-image.png"));
        check(f, "asr/shot-image.png");
    }

    {
        puts("text layer");
        pico_set_pencil_color((Pico_Color){0xFF, 0xFF, 0xFF, 0xFF});
        pico_layer_text(NULL, "txt1", 16, "hello");
        pico_set_layer("txt1");
        const char* f = pico_output_screenshot(NULL, "out/shot-text.png", NULL);
        pico_set_layer("world");
        assert(!strcmp(f, "out/shot-text.png"));
        check(f, "asr/shot-text.png");
    }

    {
        puts("video layer");
        pico_layer_video(NULL, "vid1", "video.y4m");
        pico_set_layer("vid1");
        assert(pico_set_video("vid1", 0) == 1);
        const char* f = pico_output_screenshot(NULL, "out/shot-video.png", NULL);
        pico_set_layer("world");
        assert(!strcmp(f, "out/shot-video.png"));
        check(f, "asr/shot-video.png");
    }

    // PICO_LAYER_SCREENSHOT

    // 1. capture an existing layer into a new layer; the new layer must
    // hold the same pixels, so its screenshot matches empty1's reference
    {
        puts("screenshot layer - reuse empty1");
        pico_layer_screenshot(NULL, "snap_empty", "empty1", NULL);
        pico_set_layer("snap_empty");
        const char* f = pico_output_screenshot(NULL, "out/shot-snap-empty.png", NULL);
        pico_set_layer("world");
        assert(!strcmp(f, "out/shot-snap-empty.png"));
        check(f, "asr/shot-empty.png");
    }

    // 2. compose a multi-layer scene on world, capture it as one layer,
    // clear, redraw the capture, and confirm it matches the original scene
    {
        puts("screenshot layer - scene round-trip");

        pico_layer_empty(NULL, "red", 1, (Pico_Rel_Dim){'!', {20, 20}}, NULL);
        pico_set_layer("red");
        pico_set_effect_color((Pico_Color){0xFF, 0, 0, 0xFF});
        pico_output_clear();

        pico_layer_empty(NULL, "blue", 1, (Pico_Rel_Dim){'!', {20, 20}}, NULL);
        pico_set_layer("blue");
        pico_set_effect_color((Pico_Color){0, 0, 0xFF, 0xFF});
        pico_output_clear();

        pico_set_layer("world");
        pico_set_effect_color((Pico_Color){0x20, 0x20, 0x20, 0xFF});
        pico_output_clear();
        pico_output_draw_layer("red", &(Pico_Rel_Rect){ '!', {20, 20, 20, 20}, PICO_ANCHOR_NW });
        pico_output_draw_layer("blue", &(Pico_Rel_Rect){ '!', {50, 40, 20, 20}, PICO_ANCHOR_NW });

        const char* f1 = pico_output_screenshot("world", "out/shot-scene.png", NULL);
        check(f1, "asr/shot-scene.png");

        pico_layer_screenshot(NULL, "snap_scene", "world", NULL);
        pico_set_layer("world");
        pico_output_clear();
        pico_output_draw_layer("snap_scene", NULL);
        const char* f2 = pico_output_screenshot("world", "out/shot-scene-2.png", NULL);
        check(f2, "asr/shot-scene.png");
        pico_set_layer("world");
    }

    // 3. capture a relative region of a layer; the rect resolves against
    // the source layer (empty1 = 64x32), so this yields its left half
    {
        puts("screenshot layer - region (pct)");
        pico_layer_screenshot(NULL, "snap_half", "empty1",
            &(Pico_Rel_Rect){ '%', {0, 0, 0.5, 1.0}, PICO_ANCHOR_NW }
        );
        pico_set_layer("snap_half");
        const char* f = pico_output_screenshot(NULL, "out/shot-snap-half.png", NULL);
        pico_set_layer("world");
        assert(!strcmp(f, "out/shot-snap-half.png"));
        check(f, "asr/shot-snap-half.png");
    }

    // 4. regression: a capture must not be corrupted by a present fired from
    // the event loop during the capture's internal delay. A queued resize
    // event makes _pico_shot's pico_input_delay present, which rebinds the
    // render target to the *current* layer; the capture must still read the
    // requested layer (else it returns a blank/wrong screenshot).
    {
        puts("screenshot layer - render-target race");
        pico_layer_empty(NULL, "race_cur", 1,
            (Pico_Rel_Dim){'!', {64, 32}}, NULL);
        pico_set_layer("race_cur");
        pico_set_effect_color((Pico_Color){0x00, 0x80, 0x00, 0xFF});
        pico_output_clear();        // current layer: solid green, != empty1

        // queue an event the capture's internal pico_input_delay will pick up;
        // resizing to the current size keeps the window content unchanged
        SDL_Event e = {0};
        e.type = SDL_WINDOWEVENT;
        e.window.event = SDL_WINDOWEVENT_RESIZED;
        e.window.data1 = PICO_DIM_PHY.w;
        e.window.data2 = PICO_DIM_PHY.h;
        SDL_PushEvent(&e);

        // capture empty1 while race_cur is current: must match empty1's
        // reference, not race_cur's blank/green content
        const char* f = pico_output_screenshot("empty1", "out/shot-race.png", NULL);
        pico_set_layer("world");
        assert(!strcmp(f, "out/shot-race.png"));
        check(f, "asr/shot-empty.png");
    }

    pico_init(0);
    return 0;
}
