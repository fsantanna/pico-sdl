#ifndef PICO_MEM_H
#define PICO_MEM_H

#include "pico.h"
#include "layers.h"

///////////////////////////////////////////////////////////////////////////////
// Context structs for realm alloc callbacks
///////////////////////////////////////////////////////////////////////////////

typedef struct {
    Pico_Abs_Dim dim;
    const Pico_Color* pixels;
} _pico_alloc_pixmap_t;

typedef struct {
    const char*   up;
    int           clear;
    Pico_Rel_Dim  dim;
    Pico_Abs_Dim* tile;
} _pico_alloc_empty_t;

typedef struct {
    Pico_Layer* par;
    Pico_Rel_Rect crop;
} _pico_alloc_sub_t;

typedef struct {
    int height;
    const char* text;
} _pico_alloc_text_t;

typedef struct {
    const char* path;
    int h;
} _pico_alloc_font_t;

///////////////////////////////////////////////////////////////////////////////
// Free callbacks
///////////////////////////////////////////////////////////////////////////////

void _pico_free_font  (int, const void*, void*);
void _pico_free_layer (int, const void*, void*);
void _pico_free_sound (int, const void*, void*);

///////////////////////////////////////////////////////////////////////////////
// Alloc callbacks
///////////////////////////////////////////////////////////////////////////////

void* _pico_alloc_layer_pixmap (int, const void*, void*);
void* _pico_alloc_layer_empty  (int, const void*, void*);
void* _pico_alloc_layer_image  (int, const void*, void*);
void* _pico_alloc_layer_sub    (int, const void*, void*);
void* _pico_alloc_layer_text   (int, const void*, void*);
void* _pico_alloc_font         (int, const void*, void*);
void* _pico_alloc_sound        (int, const void*, void*);

///////////////////////////////////////////////////////////////////////////////
// Internal helpers exposed to other modules
///////////////////////////////////////////////////////////////////////////////

Pico_Layer* _pico_layer_new (
    int clear, int type, size_t size,
    const char* key, SDL_Texture* tex, Pico_Abs_Dim dim
);

#endif
