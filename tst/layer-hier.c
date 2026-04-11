#include "pico.h"
#include "../check.h"

/* Scene-graph smoke test: one child attached to root.
 *
 * Creates an empty layer under "root", draws a red square into it,
 * and relies on auto-composite (present walk) to blit it onto root.
 * No explicit pico_output_draw_layer call.
 */
int main (void) {
    pico_init(1);

    pico_set_color_clear(PICO_COLOR_BLACK);
    pico_output_clear();

    pico_layer_empty("root", "L", (Pico_Abs_Dim){8, 8}, NULL);

    pico_set_layer("L");
    pico_set_view(-1, NULL, NULL,
        &(Pico_Rel_Rect){'%', {0.3, 0.3, 0.4, 0.4}, PICO_ANCHOR_C, NULL},
        NULL, NULL, NULL, NULL, NULL
    );
    pico_set_color_clear(PICO_COLOR_WHITE);
    pico_output_clear();
    pico_set_color_draw(PICO_COLOR_RED);
    pico_output_draw_rect(
        &(Pico_Rel_Rect){'%', {.5, .5, 0.5, 0.5}, PICO_ANCHOR_C, NULL}
    );

    pico_set_layer("root");
    pico_output_present();
    _pico_check("layer-hier-01");

    pico_layer_image("root", "img", "open.png");
    pico_set_layer("img");
    pico_set_view(-1, NULL, NULL,
        &(Pico_Rel_Rect){'%', {0.7, 0.7, 0.4, 0.4}, PICO_ANCHOR_C, NULL},
        NULL, NULL, NULL, NULL, NULL
    );

    pico_set_layer("root");
    pico_output_present();
    _pico_check("layer-hier-02");

    static Pico_Color buf[4] = {
        {255, 0, 0, 255}, {0, 255, 0, 255},
        {0, 0, 255, 255}, {255, 255, 0, 255},
    };
    pico_layer_buffer("root", "buf", (Pico_Abs_Dim){2, 2}, buf);
    pico_set_layer("buf");
    pico_set_view(-1, NULL, NULL,
        &(Pico_Rel_Rect){'%', {0.7, 0.3, 0.4, 0.4}, PICO_ANCHOR_C, NULL},
        NULL, NULL, NULL, NULL, NULL
    );

    pico_set_layer("root");
    pico_output_present();
    _pico_check("layer-hier-03");

    pico_set_color_draw(PICO_COLOR_GREEN);
    pico_layer_text("root", "txt", 10, "hello");
    pico_set_layer("txt");
    pico_set_view(-1, NULL, NULL,
        &(Pico_Rel_Rect){'%', {0.3, 0.7, 0.4, 0.4}, PICO_ANCHOR_C, NULL},
        NULL, NULL, NULL, NULL, NULL
    );

    pico_set_layer("root");
    pico_output_present();
    _pico_check("layer-hier-04");

    ///////////////////////////////////////////////////////////////////////////

    // transparent overlay with yellow background
    pico_layer_empty("root", "over", (Pico_Abs_Dim){500, 500}, NULL);
    pico_set_layer("over");
    pico_set_color_clear((Pico_Color){0xFF, 0xFF, 0x00, 0xFF});
    pico_output_clear();
    unsigned char alpha = 0x80;
    pico_set_view(-1, NULL, NULL,
        &(Pico_Rel_Rect){'%', {0.5, 0.5, 1, 1}, PICO_ANCHOR_C, NULL},
        NULL, NULL, NULL, NULL, &alpha
    );
    pico_set_layer("root");

    pico_set_layer("root");
    pico_output_present();
    _pico_check("layer-hier-05");

    // sub-layers: crop blue and green from "buf", parented under "over"
    pico_layer_sub("over", "blue", "buf",
        &(Pico_Rel_Rect){'!', {0, 1, 1, 1}, PICO_ANCHOR_NW, NULL});
    pico_set_layer("blue");
    pico_set_view(-1, NULL, NULL,
        &(Pico_Rel_Rect){'%', {0.20, 0.9, 0.10, 0.10}, PICO_ANCHOR_C, NULL},
        NULL, NULL, NULL, NULL, NULL
    );

    pico_layer_sub("over", "green", "buf",
        &(Pico_Rel_Rect){'!', {1, 0, 1, 1}, PICO_ANCHOR_NW, NULL});
    pico_set_layer("green");
    pico_set_view(-1, NULL, NULL,
        &(Pico_Rel_Rect){'%', {0.40, 0.9, 0.10, 0.10}, PICO_ANCHOR_C, NULL},
        NULL, NULL, NULL, NULL, NULL
    );

    pico_set_layer("root");
    pico_output_present();
    _pico_check("layer-hier-06");

    pico_init(0);
    return 0;
}
