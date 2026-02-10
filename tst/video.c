#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_window("Video", -1, NULL);
    pico_set_color_clear((Pico_Color){0x00, 0x00, 0x00});

    /* pico_get_video: check Y4M header parsing */
    {
        puts("get_video: dimensions and fps");
        Pico_Video info = pico_get_video("video.y4m", NULL);
        assert(info.dim.w == 10);
        assert(info.dim.h == 10);
        assert(info.fps == 10);
        assert(info.frame == 0);
        assert(info.done == 0);
    }

    /* pico_layer_video: create layer */
    {
        puts("layer_video: create");
        const char* name = pico_layer_video("vid", "video.y4m");
        assert(strcmp(name, "vid") == 0);
    }

    /* pico_set_video: first frame (0) */
    {
        puts("set_video: frame 0 (top-left)");
        pico_set_layer("vid");
        int ok = pico_set_video("vid", 0);
        assert(ok == 1);
        pico_set_layer(NULL);
        pico_output_clear();
        pico_output_draw_layer("vid",
            &(Pico_Rel_Rect){ '%', {0.5, 0.5, 1, 1}, PICO_ANCHOR_C, NULL }
        );
        _pico_check("video-01");
    }

    /* pico_set_video: middle frame (25) */
    {
        puts("set_video: frame 25 (bottom-middle)");
        int ok = pico_set_video("vid", 25);
        assert(ok == 1);
        pico_output_clear();
        pico_output_draw_layer("vid",
            &(Pico_Rel_Rect){ '%', {0.5, 0.5, 1, 1}, PICO_ANCHOR_C, NULL }
        );
        _pico_check("video-02");
    }

    /* pico_set_video: last frame (49) */
    {
        puts("set_video: frame 49 (top-right)");
        int ok = pico_set_video("vid", 49);
        assert(ok == 1);
        pico_output_clear();
        pico_output_draw_layer("vid",
            &(Pico_Rel_Rect){ '%', {0.5, 0.5, 1, 1}, PICO_ANCHOR_C, NULL }
        );
        _pico_check("video-03");
    }

    /* pico_set_video: backward seek */
    {
        puts("set_video: backward seek to frame 10");
        int ok = pico_set_video("vid", 10);
        assert(ok == 1);
        pico_output_clear();
        pico_output_draw_layer("vid",
            &(Pico_Rel_Rect){ '%', {0.5, 0.5, 1, 1}, PICO_ANCHOR_C, NULL }
        );
        _pico_check("video-04");
    }

    /* pico_set_video: past EOF */
    {
        puts("set_video: past EOF returns 0");
        int ok = pico_set_video("vid", 100);
        assert(ok == 0);
    }

    /* pico_get_video: check state after sync */
    {
        puts("get_video: state after sync");
        Pico_Video info = pico_get_video("vid", NULL);
        assert(info.done == 1);
    }

    /* pico_output_draw_video: all-in-one */
    {
        puts("draw_video: first call");
        pico_output_clear();
        int ok = pico_output_draw_video("video.y4m",
            &(Pico_Rel_Rect){ '%', {0.5, 0.5, 1, 1}, PICO_ANCHOR_C, NULL }
        );
        assert(ok == 1);
        _pico_check("video-05");
    }

    pico_init(0);
    return 0;
}
