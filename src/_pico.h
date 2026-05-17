#ifndef PICO_INTERNAL_H
#define PICO_INTERNAL_H

#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "realm.hc"
#include "pico.h"

///////////////////////////////////////////////////////////////////////////////
// LAYER (types)
///////////////////////////////////////////////////////////////////////////////

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
    Pico_Abs_Dim sup;
} Pico_Layer_Sub;

///////////////////////////////////////////////////////////////////////////////
// VIDEO (type)
///////////////////////////////////////////////////////////////////////////////

typedef struct {
    Pico_Layer base;
    FILE*      fp;
    int        fps;
    struct {
        unsigned char* y;
        unsigned char* u;
        unsigned char* v;
    } plane;
    struct {
        int y;
        int uv;
        int frame;
    } size;
    long       data_offset;
    struct {
        int total;
        int cur;
        int done;
    } frame;
    Uint32     t0;
} Pico_Layer_Video;

///////////////////////////////////////////////////////////////////////////////
// MEM (alloc-ctx types)
///////////////////////////////////////////////////////////////////////////////

typedef struct {
    Pico_Abs_Dim dim;
    const Pico_Color* pixels;
} _pico_mem_alloc_pixmap_t;

typedef struct {
    const char*   up;
    int           clear;
    Pico_Rel_Dim  dim;
    Pico_Abs_Dim* tile;
} _pico_mem_alloc_empty_t;

typedef struct {
    Pico_Layer* par;
    Pico_Rel_Rect crop;
} _pico_mem_alloc_sub_t;

typedef struct {
    int height;
    const char* text;
} _pico_mem_alloc_text_t;

typedef struct {
    const char* path;
    int h;
} _pico_mem_alloc_font_t;

///////////////////////////////////////////////////////////////////////////////
// STATE
///////////////////////////////////////////////////////////////////////////////

typedef struct PicoState {
    int           init;
    realm_t*      realm;
    Pico_Layer    world;
    Pico_Layer*   layer;
    struct {
        int on;
        int fps;
        int ms;
        int t0;
    } expert;
    struct {
        SDL_Window*   win;
        SDL_Renderer* ren;
        Pico_Layer    layer;
        struct {
            int fs;
            int out;
        } ing;
        struct { int fs; } pub;
    } window;
} PicoState;

extern PicoState G;

///////////////////////////////////////////////////////////////////////////////
// pico (init/aux helpers, defined in pico.c)
///////////////////////////////////////////////////////////////////////////////

void         _pico_guard      (void);
SDL_Texture* _pico_tex_create (Pico_Abs_Dim dim);
TTF_Font*    _pico_font_get   (const char* path, int h);

///////////////////////////////////////////////////////////////////////////////
// output
///////////////////////////////////////////////////////////////////////////////

void _pico_output_present (int force);

///////////////////////////////////////////////////////////////////////////////
// aux
///////////////////////////////////////////////////////////////////////////////

SDL_FDim   _pico_raw_dim  (Pico_Rel_Dim* dim, const Pico_Abs_Rect* base,
                               const Pico_Abs_Dim* ratio);
SDL_FPoint _pico_raw_pos  (Pico_Rel_Pos pos, const Pico_Abs_Rect* base);
SDL_FRect  _pico_raw_rect (Pico_Rel_Rect rect, const Pico_Abs_Rect* base,
                               const Pico_Abs_Dim* ratio);

void _pico_rel_dim  (SDL_FDim   flt, Pico_Rel_Dim*  to, const Pico_Abs_Rect* base);
void _pico_rel_pos  (SDL_FPoint flt, Pico_Rel_Pos*  to, const Pico_Abs_Rect* base);
void _pico_rel_rect (SDL_FRect  flt, Pico_Rel_Rect* to, const Pico_Abs_Rect* base);

Pico_Abs_Dim  _pico_rnd_dim  (SDL_FDim   f);
Pico_Abs_Pos  _pico_rnd_pos  (SDL_FPoint f);
Pico_Abs_Rect _pico_rnd_rect (SDL_FRect  f);

///////////////////////////////////////////////////////////////////////////////
// layers
///////////////////////////////////////////////////////////////////////////////

Pico_Layer* _pico_layer_name   (const char* name);
void        _pico_layer_attach (const char* up, const char* dn);

Pico_Layer* _pico_layer_pixmap (
    int mode, const char* key, Pico_Abs_Dim dim, const Pico_Color* pixels
);
Pico_Layer* _pico_layer_image (
    int mode, const char* key, const char* path
);
Pico_Layer* _pico_layer_text (
    int mode, const char* key, int height, const char* text
);

void _pico_layer_output   (Pico_Layer* layer, const Pico_Rel_Rect* rect);
void _pico_layer_draw_all (Pico_Layer* UP);

///////////////////////////////////////////////////////////////////////////////
// mem
///////////////////////////////////////////////////////////////////////////////

void _pico_mem_free_font  (int, const void*, void*);
void _pico_mem_free_layer (int, const void*, void*);
void _pico_mem_free_sound (int, const void*, void*);

void* _pico_mem_alloc_layer_pixmap (int, const void*, void*);
void* _pico_mem_alloc_layer_empty  (int, const void*, void*);
void* _pico_mem_alloc_layer_image  (int, const void*, void*);
void* _pico_mem_alloc_layer_sub    (int, const void*, void*);
void* _pico_mem_alloc_layer_text   (int, const void*, void*);
void* _pico_mem_alloc_font         (int, const void*, void*);
void* _pico_mem_alloc_sound        (int, const void*, void*);

Pico_Layer* _pico_mem_layer_new (
    int clear, int type, size_t size,
    const char* key, SDL_Texture* tex, Pico_Abs_Dim dim
);

///////////////////////////////////////////////////////////////////////////////
// video
///////////////////////////////////////////////////////////////////////////////

void _pico_video_free_layer (Pico_Layer_Video*);

#endif
