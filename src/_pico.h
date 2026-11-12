#ifndef PICO_INTERNAL_H
#define PICO_INTERNAL_H

#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include "pico.h"

#define MAX(x,y) ((x) > (y) ? (x) : (y))

///////////////////////////////////////////////////////////////////////////////
// LAYER (types)
///////////////////////////////////////////////////////////////////////////////

typedef enum {
    PICO_LAYER_PLAIN,
    PICO_LAYER_VIDEO,
    PICO_LAYER_SUB,
} PICO_LAYER;

struct Pico_Layer {
    PICO_LAYER            type;
    int                   cached;   // owned by the cache (easy paths)
    int                   subs;     // live sub-layers borrowing tex
    SDL_Texture*          tex;
    Pico_Layer_Pencil     pencil;
    Pico_Layer_Effect     effect;
    Pico_Layer_Scene      scene;
    struct {
        Pico_Layer* up;             // parent; NULL = root or detached
        Pico_Layer* nxt;            // next sibling under same up
        struct {
            Pico_Layer* fst;        // first child (back; drawn first)
            Pico_Layer* lst;        // last child  (front; drawn last)
        } dn;
    } hier;
};

typedef struct {
    Pico_Layer   base;
    Pico_Layer*  src;               // layer whose texture is borrowed
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
// FONT / SOUND (types)
///////////////////////////////////////////////////////////////////////////////

// A font file opened at one or more pixel heights (a TTF_Font is bound
// to a single size, so each height used gets its own).
struct Pico_Font {
    int   id;                       // unique serial (text cache keys)
    int   cached;                   // owned by the cache (easy paths)
    char* path;                     // NULL = built-in font
    int   n;
    struct {
        int       h;
        TTF_Font* ttf;
    }* sizes;
};

struct Pico_Sound {
    int        cached;              // owned by the cache (easy paths)
    Mix_Chunk* mix;
};

///////////////////////////////////////////////////////////////////////////////
// CACHE (types)
///////////////////////////////////////////////////////////////////////////////

// Private memo table behind the easy paths: string key -> resource.
// Entries are never exposed, never evicted, and freed at pico_close.

typedef enum {
    PICO_CACHE_LAYER,
    PICO_CACHE_FONT,
    PICO_CACHE_SOUND,
} PICO_CACHE;

typedef struct Pico_Cache_Entry {
    char*                    key;
    PICO_CACHE               kind;
    void*                    value;
    struct Pico_Cache_Entry* next;
} Pico_Cache_Entry;

///////////////////////////////////////////////////////////////////////////////
// STATE
///////////////////////////////////////////////////////////////////////////////

typedef struct PicoState {
    struct {
        unsigned init:   1;
        unsigned aids:   1;
        unsigned loop:   1;
        unsigned expert: 1;
    } flags;
    int           live;         // user-owned layers/fonts/sounds alive
    int           fonts;        // font serial counter
    Pico_Font     font;         // built-in font (pencil.font == NULL)
    Pico_Cache_Entry* cache[PICO_HASH_BUK];
    Pico_Layer    world;
    Pico_Layer*   layer;
    struct {
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
void         _pico_close      (int strict);

///////////////////////////////////////////////////////////////////////////////
// output
///////////////////////////////////////////////////////////////////////////////

void _pico_output_present (int force);
SDL_Surface* _pico_shot (Pico_Layer* layer, const Pico_Rel_Rect* rect);

///////////////////////////////////////////////////////////////////////////////
// aux
///////////////////////////////////////////////////////////////////////////////

Pico_Abs_Dim  _pico_abs_dim  (Pico_Rel_Dim* dim, const Pico_Abs_Rect* base,
                              const Pico_Abs_Dim* ratio);
Pico_Abs_Pos  _pico_abs_pos  (Pico_Rel_Pos pos, const Pico_Abs_Rect* base);
Pico_Abs_Rect _pico_abs_rect (Pico_Rel_Rect rect, const Pico_Abs_Rect* base,
                              const Pico_Abs_Dim* ratio);
Pico_Rel_Dim  _pico_rel_dim  (Pico_Abs_Dim abs, char mode);

// resolve a (dim, tile) pair jointly (see plans/260710-tiles.md §7).
// tile==NULL: no tiles, out_tile={0,0}, dim resolved as usual.
// otherwise: out_dim=D, out_tile=T per the §7 matrix (floor, rejects).
// base is the parent/reference frame for dim='%' (NULL = current scene).
void _pico_resolve_dim_tile (
    Pico_Rel_Dim dim, const Pico_Rel_Dim* tile,
    const Pico_Abs_Rect* base,
    Pico_Abs_Dim* out_dim, Pico_Abs_Dim* out_tile
);

///////////////////////////////////////////////////////////////////////////////
// layers
///////////////////////////////////////////////////////////////////////////////

Pico_Layer* _pico_set_layer    (Pico_Layer* L);
void        _pico_layer_attach (Pico_Layer* up, Pico_Layer* dn);
void        _pico_layer_detach (Pico_Layer* L);

// cache-owned (easy path) layers
Pico_Layer* _pico_layer_image_path (const char* path);
Pico_Layer* _pico_layer_text_fp    (int height, const char* text);

void _pico_layer_output   (Pico_Layer* layer, const Pico_Rel_Rect* rect);
void _pico_layer_draw_all (Pico_Layer* UP);

// depth-first child walk: pre(UP,CUR) runs before descending into CUR,
// pos(UP,CUR) after; either callback may be NULL
typedef void (*_pico_layer_traverse_cb_t) (Pico_Layer* up, Pico_Layer* cur);
void _pico_layer_traverse (Pico_Layer* up, _pico_layer_traverse_cb_t pre, _pico_layer_traverse_cb_t pos);

///////////////////////////////////////////////////////////////////////////////
// mem
///////////////////////////////////////////////////////////////////////////////

// cache
void* _pico_cache_get   (const char* key);
void  _pico_cache_put   (const char* key, PICO_CACHE kind, void* value);
void  _pico_cache_close (void);

// user-owned resource accounting (asserted zero at pico_close)
void  _pico_mem_own  (int cached);
void  _pico_mem_disown (int cached);

// layers: `cached` marks cache ownership (not counted as live)
Pico_Layer* _pico_mem_layer_new (
    int cached, int clear, int type, size_t size,
    SDL_Texture* tex, Pico_Abs_Dim dim
);
void        _pico_mem_layer_free   (Pico_Layer* L);
Pico_Layer* _pico_mem_layer_pixmap (int cached, Pico_Abs_Dim dim, const Pico_Color* pixels);
Pico_Layer* _pico_mem_layer_shot   (int cached, Pico_Layer* src, const Pico_Rel_Rect* rect);
Pico_Layer* _pico_mem_layer_empty  (int cached, Pico_Layer* up, int clear, Pico_Rel_Dim dim, Pico_Rel_Dim* tile);
Pico_Layer* _pico_mem_layer_image  (int cached, const char* path);
Pico_Layer* _pico_mem_layer_sub    (int cached, Pico_Layer* src, Pico_Rel_Rect crop);
Pico_Layer* _pico_mem_layer_text   (int cached, int height, const char* text);

// fonts
Pico_Font*  _pico_mem_font_new  (int cached, const char* path);
void        _pico_mem_font_free (Pico_Font* font);
void        _pico_mem_font_done (Pico_Font* font);   // sizes only (built-in)
TTF_Font*   _pico_font_ttf      (Pico_Font* font, int h);

// sounds
Pico_Sound* _pico_mem_sound_new  (int cached, const char* path);
void        _pico_mem_sound_free (Pico_Sound* sound);

///////////////////////////////////////////////////////////////////////////////
// video
///////////////////////////////////////////////////////////////////////////////

void        _pico_video_free_layer (Pico_Layer_Video*);
Pico_Layer* _pico_mem_layer_video  (int cached, const char* path);

#endif
