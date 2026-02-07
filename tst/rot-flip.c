#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_window("Rotate / Flip", -1, NULL);
    pico_set_color_clear((Pico_Color){0xFF,0xFF,0xFF});

    // Create a layer from image for rotation/flip tests
    pico_layer_image("img", "open.png");

    // ROTATION - center anchor
    {
        puts("rotate 0 degrees (no rotation)");
        pico_output_clear();
        pico_set_layer("img");
        Pico_Rot rot = {0, PICO_ANCHOR_C};
        pico_set_view(-1, NULL, NULL, NULL, NULL, NULL, &rot, NULL);
        pico_set_layer(NULL);
        Pico_Rel_Rect r = {'!', {50, 50, 48, 48}, PICO_ANCHOR_C, NULL};
        pico_output_draw_layer("img", &r);
        _pico_check("rot-flip-01");
    }
    {
        puts("rotate 45 degrees");
        pico_output_clear();
        pico_set_layer("img");
        Pico_Rot rot = {45, PICO_ANCHOR_C};
        pico_set_view(-1, NULL, NULL, NULL, NULL, NULL, &rot, NULL);
        pico_set_layer(NULL);
        Pico_Rel_Rect r = {'!', {50, 50, 48, 48}, PICO_ANCHOR_C, NULL};
        pico_output_draw_layer("img", &r);
        _pico_check("rot-flip-02");
    }
    {
        puts("rotate 90 degrees");
        pico_output_clear();
        pico_set_layer("img");
        Pico_Rot rot = {90, PICO_ANCHOR_C};
        pico_set_view(-1, NULL, NULL, NULL, NULL, NULL, &rot, NULL);
        pico_set_layer(NULL);
        Pico_Rel_Rect r = {'!', {50, 50, 48, 48}, PICO_ANCHOR_C, NULL};
        pico_output_draw_layer("img", &r);
        _pico_check("rot-flip-03");
    }
    {
        puts("rotate 180 degrees");
        pico_output_clear();
        pico_set_layer("img");
        Pico_Rot rot = {180, PICO_ANCHOR_C};
        pico_set_view(-1, NULL, NULL, NULL, NULL, NULL, &rot, NULL);
        pico_set_layer(NULL);
        Pico_Rel_Rect r = {'!', {50, 50, 48, 48}, PICO_ANCHOR_C, NULL};
        pico_output_draw_layer("img", &r);
        _pico_check("rot-flip-04");
    }

    // ROTATION - corner anchor (NW)
    {
        puts("rotate 45 degrees, anchor NW");
        pico_output_clear();
        pico_set_layer("img");
        Pico_Rot rot = {45, PICO_ANCHOR_NW};
        pico_set_view(-1, NULL, NULL, NULL, NULL, NULL, &rot, NULL);
        pico_set_layer(NULL);
        Pico_Rel_Rect r = {'!', {50, 50, 48, 48}, PICO_ANCHOR_C, NULL};
        pico_output_draw_layer("img", &r);
        _pico_check("rot-flip-05");
    }

    // Reset rotation for flip tests
    {
        pico_set_layer("img");
        Pico_Rot rot = {0, PICO_ANCHOR_C};
        pico_set_view(-1, NULL, NULL, NULL, NULL, NULL, &rot, NULL);
        pico_set_layer(NULL);
    }

    // FLIP - horizontal
    {
        puts("flip horizontal");
        pico_output_clear();
        pico_set_layer("img");
        PICO_FLIP flip = PICO_FLIP_HORIZONTAL;
        pico_set_view(-1, NULL, NULL, NULL, NULL, NULL, NULL, &flip);
        pico_set_layer(NULL);
        Pico_Rel_Rect r = {'!', {50, 50, 48, 48}, PICO_ANCHOR_C, NULL};
        pico_output_draw_layer("img", &r);
        _pico_check("rot-flip-06");
    }
    {
        puts("flip vertical");
        pico_output_clear();
        pico_set_layer("img");
        PICO_FLIP flip = PICO_FLIP_VERTICAL;
        pico_set_view(-1, NULL, NULL, NULL, NULL, NULL, NULL, &flip);
        pico_set_layer(NULL);
        Pico_Rel_Rect r = {'!', {50, 50, 48, 48}, PICO_ANCHOR_C, NULL};
        pico_output_draw_layer("img", &r);
        _pico_check("rot-flip-07");
    }
    {
        puts("flip both");
        pico_output_clear();
        pico_set_layer("img");
        PICO_FLIP flip = PICO_FLIP_BOTH;
        pico_set_view(-1, NULL, NULL, NULL, NULL, NULL, NULL, &flip);
        pico_set_layer(NULL);
        Pico_Rel_Rect r = {'!', {50, 50, 48, 48}, PICO_ANCHOR_C, NULL};
        pico_output_draw_layer("img", &r);
        _pico_check("rot-flip-08");
    }

    // COMBINED - rotate + flip
    {
        puts("rotate 45 + flip horizontal");
        pico_output_clear();
        pico_set_layer("img");
        Pico_Rot rot = {45, PICO_ANCHOR_C};
        PICO_FLIP flip = PICO_FLIP_HORIZONTAL;
        pico_set_view(-1, NULL, NULL, NULL, NULL, NULL, &rot, &flip);
        pico_set_layer(NULL);
        Pico_Rel_Rect r = {'!', {50, 50, 48, 48}, PICO_ANCHOR_C, NULL};
        pico_output_draw_layer("img", &r);
        _pico_check("rot-flip-09");
    }

    // ANIMATED ROTATION - center anchor (default)
    {
        puts("animated rotation - center anchor");
        Pico_Rel_Rect r = {'!', {50, 50, 48, 48}, PICO_ANCHOR_C, NULL};
        for (int angle = 0; angle < 360; angle += 5) {
            pico_output_clear();
            pico_set_layer("img");
            Pico_Rot rot = {angle, PICO_ANCHOR_C};
            pico_set_view(-1, NULL, NULL, NULL, NULL, NULL, &rot, NULL);
            pico_set_layer(NULL);
            pico_output_draw_layer("img", &r);
            if (angle == 180) {
                _pico_check("rot-flip-10");
            }
            pico_input_delay(10);
        }
    }

    // ANIMATED ROTATION - anchor outside (1.1, 1.1)
    {
        puts("animated rotation - anchor outside (1.1, 1.1)");
        Pico_Rel_Rect r = {'!', {50, 50, 48, 48}, PICO_ANCHOR_C, NULL};
        for (int angle = 0; angle < 360; angle += 5) {
            pico_output_clear();
            pico_set_layer("img");
            Pico_Rot rot = {angle, {1.1, 1.1}};
            pico_set_view(-1, NULL, NULL, NULL, NULL, NULL, &rot, NULL);
            pico_set_layer(NULL);
            pico_output_draw_layer("img", &r);
            if (angle == 180) {
                _pico_check("rot-flip-11");
            }
            pico_input_delay(10);
        }
    }

    // ANIMATED ROTATION - anchor negative (-0.1, -0.1)
    {
        puts("animated rotation - anchor negative (-0.1, -0.1)");
        Pico_Rel_Rect r = {'!', {50, 50, 48, 48}, PICO_ANCHOR_C, NULL};
        for (int angle = 0; angle < 360; angle += 5) {
            pico_output_clear();
            pico_set_layer("img");
            Pico_Rot rot = {angle, {-0.1, -0.1}};
            pico_set_view(-1, NULL, NULL, NULL, NULL, NULL, &rot, NULL);
            pico_set_layer(NULL);
            pico_output_draw_layer("img", &r);
            if (angle == 180) {
                _pico_check("rot-flip-12");
            }
            pico_input_delay(10);
        }
    }

    pico_init(0);
    return 0;
}
