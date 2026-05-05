#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);

    // Create a 4x4 "sprite sheet" with colored quadrants
    const char* sheet = "sheet";
    pico_layer_empty(NULL, sheet, (Pico_Abs_Dim){4, 4}, NULL);
    pico_set_effect_color(sheet, PICO_COLOR_BLACK);
    pico_output_clear(sheet);

    // TL red, TR green, BL blue, BR white
    pico_set_pencil_color(sheet, PICO_COLOR_RED);
    pico_output_draw_rect(sheet, &(Pico_Rel_Rect){
        '!', {0,0,2,2}, PICO_ANCHOR_NW, NULL});
    pico_set_pencil_color(sheet, PICO_COLOR_GREEN);
    pico_output_draw_rect(sheet, &(Pico_Rel_Rect){
        '!', {2,0,2,2}, PICO_ANCHOR_NW, NULL});
    pico_set_pencil_color(sheet, PICO_COLOR_BLUE);
    pico_output_draw_rect(sheet, &(Pico_Rel_Rect){
        '!', {0,2,2,2}, PICO_ANCHOR_NW, NULL});
    pico_set_pencil_color(sheet, PICO_COLOR_WHITE);
    pico_output_draw_rect(sheet, &(Pico_Rel_Rect){
        '!', {2,2,2,2}, PICO_ANCHOR_NW, NULL});

    // Shot 1: draw full layer (normal quadrants)
    pico_output_clear("root");
    pico_output_draw_layer("root", sheet, &(Pico_Rel_Rect){
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
    pico_output_clear("root");
    pico_output_draw_layer("root", "tl", &(Pico_Rel_Rect){
        '%', {0.75,0.75, 0.5,0.5}, PICO_ANCHOR_C, NULL});
    pico_output_draw_layer("root", "tr", &(Pico_Rel_Rect){
        '%', {0.25,0.75, 0.5,0.5}, PICO_ANCHOR_C, NULL});
    pico_output_draw_layer("root", "bl", &(Pico_Rel_Rect){
        '%', {0.75,0.25, 0.5,0.5}, PICO_ANCHOR_C, NULL});
    pico_output_draw_layer("root", "br", &(Pico_Rel_Rect){
        '%', {0.25,0.25, 0.5,0.5}, PICO_ANCHOR_C, NULL});

    pico_init(0);
    return 0;
}
