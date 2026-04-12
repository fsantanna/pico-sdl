#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);

    // Create a 4x4 "sprite sheet" with colored quadrants
    const char* sheet = "sheet";
    pico_layer_empty(NULL, sheet, (Pico_Abs_Dim){4, 4}, NULL);
    pico_set_layer(sheet);
    pico_set_color_clear(PICO_COLOR_BLACK);
    pico_output_clear();

    // TL red, TR green, BL blue, BR white
    pico_set_draw_color(PICO_COLOR_RED);
    pico_output_draw_rect(&(Pico_Rel_Rect){
        '!', {0,0,2,2}, PICO_ANCHOR_NW, NULL});
    pico_set_draw_color(PICO_COLOR_GREEN);
    pico_output_draw_rect(&(Pico_Rel_Rect){
        '!', {2,0,2,2}, PICO_ANCHOR_NW, NULL});
    pico_set_draw_color(PICO_COLOR_BLUE);
    pico_output_draw_rect(&(Pico_Rel_Rect){
        '!', {0,2,2,2}, PICO_ANCHOR_NW, NULL});
    pico_set_draw_color(PICO_COLOR_WHITE);
    pico_output_draw_rect(&(Pico_Rel_Rect){
        '!', {2,2,2,2}, PICO_ANCHOR_NW, NULL});
    pico_set_layer(NULL);

    // Shot 1: draw full layer (normal quadrants)
    pico_output_clear();
    pico_output_draw_layer(sheet, &(Pico_Rel_Rect){
        '%', {0.5,0.5, 1,1}, PICO_ANCHOR_C, NULL});
    _pico_check("sheet-01");

    // Create sub-layers for each quadrant
    pico_layer_sub(NULL, "tl", sheet,
        &(Pico_Rel_Rect){'!', {0,0,2,2}, PICO_ANCHOR_NW, NULL});
    pico_layer_sub(NULL, "tr", sheet,
        &(Pico_Rel_Rect){'!', {2,0,2,2}, PICO_ANCHOR_NW, NULL});
    pico_layer_sub(NULL, "bl", sheet,
        &(Pico_Rel_Rect){'!', {0,2,2,2}, PICO_ANCHOR_NW, NULL});
    pico_layer_sub(NULL, "br", sheet,
        &(Pico_Rel_Rect){'!', {2,2,2,2}, PICO_ANCHOR_NW, NULL});

    // Shot 2: draw swapped sub-layers
    pico_output_clear();
    pico_output_draw_layer("tl", &(Pico_Rel_Rect){
        '%', {0.75,0.75, 0.5,0.5}, PICO_ANCHOR_C, NULL});
    pico_output_draw_layer("tr", &(Pico_Rel_Rect){
        '%', {0.25,0.75, 0.5,0.5}, PICO_ANCHOR_C, NULL});
    pico_output_draw_layer("bl", &(Pico_Rel_Rect){
        '%', {0.75,0.25, 0.5,0.5}, PICO_ANCHOR_C, NULL});
    pico_output_draw_layer("br", &(Pico_Rel_Rect){
        '%', {0.25,0.25, 0.5,0.5}, PICO_ANCHOR_C, NULL});

    pico_init(0);
    return 0;
}
