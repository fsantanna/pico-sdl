#ifndef PICO_LAYERS_H
#define PICO_LAYERS_H

#include "pico.h"

typedef enum {
    PICO_LAYER_PLAIN,
    PICO_LAYER_VIDEO,
    PICO_LAYER_SUB,
} PICO_LAYER;

typedef struct Pico_Layer {
    PICO_LAYER            type;
    char*                 name;     // "world" for the world/root layer
    SDL_Texture*          tex;
    Pico_Layer_Pencil     pencil;
    Pico_Layer_Effect     effect;
    Pico_Layer_Scene      scene;
    struct {
        const char* up;             // parent id; NULL = root or detached
        const char* nxt;            // next sibling under same up
        struct {
            const char* fst;        // first child (back; drawn first)
            const char* lst;        // last child  (front; drawn last)
        } dn;
    } hier;
} Pico_Layer;

typedef struct {
    Pico_Layer   base;
    Pico_Abs_Dim sup;     // snapshot of source scene.dim
} Pico_Layer_Sub;

Pico_Layer* _pico_layer_name (const char* name);

void        _layer_attach    (const char* up, const char* dn);

Pico_Layer* _pico_layer_pixmap (
    int mode, const char* key, Pico_Abs_Dim dim,
    const Pico_Color* pixels
);
Pico_Layer* _pico_layer_image  (
    int mode, const char* key, const char* path
);
Pico_Layer* _pico_layer_text   (
    int mode, const char* key, int height, const char* text
);

void _pico_output_draw_layer  (Pico_Layer* layer, const Pico_Rel_Rect* rect);
void _pico_output_draw_layers (Pico_Layer* UP);

#endif
