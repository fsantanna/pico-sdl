#include "pico.h"
#include "../check.h"

int main (void) {
    pico_open(NULL);

    // Create a 4x4 "sprite sheet" with colored quadrants
    Pico_Layer* sheet = pico_layer_empty(NULL, 1, (Pico_Rel_Dim){'!', {4, 4}}, NULL);
    pico_set_layer(sheet);
    pico_set_effect_color(PICO_COLOR_BLACK);
    pico_output_clear();

    // TL red, TR green, BL blue, BR white
    pico_set_pencil_color(PICO_COLOR_RED);
    pico_output_draw_rect((Pico_Rel_Rect){
        '!', {0,0,2,2}, PICO_ANCHOR_NW});
    pico_set_pencil_color(PICO_COLOR_GREEN);
    pico_output_draw_rect((Pico_Rel_Rect){
        '!', {2,0,2,2}, PICO_ANCHOR_NW});
    pico_set_pencil_color(PICO_COLOR_BLUE);
    pico_output_draw_rect((Pico_Rel_Rect){
        '!', {0,2,2,2}, PICO_ANCHOR_NW});
    pico_set_pencil_color(PICO_COLOR_WHITE);
    pico_output_draw_rect((Pico_Rel_Rect){
        '!', {2,2,2,2}, PICO_ANCHOR_NW});
    pico_set_layer(pico_layer_world());

    // Shot 1: draw full layer (normal quadrants)
    pico_output_clear();
    pico_output_draw_layer(sheet, &(Pico_Rel_Rect){
        '%', {0.5,0.5, 1,1}, PICO_ANCHOR_C});
    _pico_check("sheet-01");

    // Create sub-layers for each quadrant
    Pico_Layer* tl = pico_layer_sub(NULL, sheet,
        &(Pico_Rel_Rect){'!', {0,0,2,2}, PICO_ANCHOR_NW});
    Pico_Layer* tr = pico_layer_sub(NULL, sheet,
        &(Pico_Rel_Rect){'!', {2,0,2,2}, PICO_ANCHOR_NW});
    Pico_Layer* bl = pico_layer_sub(NULL, sheet,
        &(Pico_Rel_Rect){'!', {0,2,2,2}, PICO_ANCHOR_NW});
    Pico_Layer* br = pico_layer_sub(NULL, sheet,
        &(Pico_Rel_Rect){'!', {2,2,2,2}, PICO_ANCHOR_NW});

    // Shot 2: draw swapped sub-layers
    pico_output_clear();
    pico_output_draw_layer(tl, &(Pico_Rel_Rect){
        '%', {0.75,0.75, 0.5,0.5}, PICO_ANCHOR_C});
    pico_output_draw_layer(tr, &(Pico_Rel_Rect){
        '%', {0.25,0.75, 0.5,0.5}, PICO_ANCHOR_C});
    pico_output_draw_layer(bl, &(Pico_Rel_Rect){
        '%', {0.75,0.25, 0.5,0.5}, PICO_ANCHOR_C});
    pico_output_draw_layer(br, &(Pico_Rel_Rect){
        '%', {0.25,0.25, 0.5,0.5}, PICO_ANCHOR_C});

    pico_layer_free(tl);
    pico_layer_free(tr);
    pico_layer_free(bl);
    pico_layer_free(br);
    pico_layer_free(sheet);
    pico_close();
    return 0;
}
