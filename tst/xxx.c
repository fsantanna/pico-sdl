#ifndef PICO_H
#define PICO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <assert.h>
#include <SDL2/SDL.h>

/// @example init.c
/// @example delay.c
/// @example event.c
/// @example event_timeout.c
/// @example event_loop.c

/// @defgroup Types
/// @brief Types, Enums, and Defines.
/// @{
///
#define PICO_TITLE "pico-SDL"
#define PICO_DIM_PHY ((Pico_Dim) {640,360})
#define PICO_DIM_LOG ((Pico_Dim) { 64, 36})
#define PICO_HASH  128

typedef SDL_Point Pico_Pos;
typedef SDL_Point Pico_Dim;
typedef SDL_Rect  Pico_Rect;
typedef SDL_Color Pico_Color;
typedef SDL_Point Pico_Anchor;
typedef SDL_Point Pico_Flip;
typedef SDL_Point Pico_Pct;

#define PICO_LEFT   0
#define PICO_CENTER 50
#define PICO_RIGHT  100
#define PICO_TOP    0
#define PICO_MIDDLE 50
#define PICO_BOTTOM 100

typedef enum PICO_STYLE {
    PICO_FILL, PICO_STROKE
} PICO_STYLE;

typedef enum PICO_MOUSE_BUTTON {
    PICO_MOUSE_BUTTON_NONE   = 0,
    PICO_MOUSE_BUTTON_LEFT   = SDL_BUTTON_LEFT,
    PICO_MOUSE_BUTTON_MIDDLE = SDL_BUTTON_MIDDLE,
    PICO_MOUSE_BUTTON_RIGHT  = SDL_BUTTON_RIGHT
} PICO_MOUSE_BUTTON;

#define PICO_DIM_KEEP ((Pico_Dim) {0,0})
#define PICO_CLIP_RESET ((Pico_Rect) {0,0,0,0})

/// @}

/// @defgroup Init
/// @brief Functions and values used in initialization.
/// @{

/// @brief Initializes and terminates pico.
/// @include init.c
/// @param on 1 to initialize, or 0 to terminate
void pico_init (int on);

/// @}

/// @}

/// @defgroup Output
/// @brief Draw primitives, play sounds, etc.
/// @{

/// @brief Clears screen with color set by @ref pico_set_color_clear.
void pico_output_clear (void);

/// @brief Draws an RGBA image that is managed by the user.
/// @param pos drawing coordinate
/// @param buffer the RGBA image
/// @param dim image dimensions
/// @sa pico_output_draw_image
/// @sa pico_output_draw_image_ext
void pico_output_draw_buffer (Pico_Pos pos, const Pico_Color buffer[], Pico_Dim dim);

/// @brief Draws an image.
/// @param pos drawing coordinate
/// @param path path to the image file
/// @sa pico_output_draw_buffer
/// @sa pico_output_draw_image_ext
void pico_output_draw_image (Pico_Pos pos, const char* path);

/// @brief Draws an image with the specified dimensions.
/// @param pos drawing coordinate
/// @param path path to the image file
/// @param dim image dimensions
/// @sa pico_output_draw_buffer
/// @sa pico_output_draw_image
void pico_output_draw_image_ext (Pico_Pos pos, const char* path, Pico_Dim dim);

/// @brief Draws a line.
/// @param p1 first point
/// @param p2 second point
void pico_output_draw_line (Pico_Pos p1, Pico_Pos p2);

/// @brief Draws a single pixel.
/// @param pos drawing coordinate
void pico_output_draw_pixel (Pico_Pos pos);

/// @brief Draws a batch of pixels.
/// @param apos array of coordinates
/// @param count amount of coordinates
void pico_output_draw_pixels (const Pico_Pos* apos, int count);

/// @brief Draws a rectangle.
/// @param rect rectangle to draw
void pico_output_draw_rect (Pico_Rect rect);

/// @brief Draws a triangle with a right angle at bottom-left.
/// @param rect bounds of the triangle
void pico_output_draw_tri (Pico_Rect rect);

/// @brief Draws an ellipse.
/// @param rect bounds of the ellipse
void pico_output_draw_oval (Pico_Rect rect);

/// @brief Draws a polygon.
/// @param apos array of coordinates
/// @param count amount of coordinates
void pico_output_draw_poly (const Pico_Pos* apos, int count);

/// @brief Draws text.
/// @param pos drawing coordinate
/// @param text text to draw
/// @sa pico_output_draw_text_ext
void pico_output_draw_text (Pico_Pos pos, const char* text);

/// @brief Draws text with the specified dim.
/// @param pos drawing coordinate
/// @param text text to draw
/// @param dim text dimensions
/// @sa pico_output_draw_text
void pico_output_draw_text_ext (Pico_Pos pos, const char* text, Pico_Dim dim);

void pico_output_draw_fmt (Pico_Pos pos, const char* fmt, ...);

void pico_output_draw_fmt_ext (Pico_Pos pos, Pico_Dim dim, const char* fmt, ...);

/// @brief Shows what has been drawn onto the screen.
/// Only does anything on expert mode.
/// @sa pico_set_expert
void pico_output_present (void);

/// @brief Takes a screenshot.
/// @param path screenshot filepath (NULL uses timestamp in the name)
/// @return The filepath of the screenshot.
/// @sa pico_output_screenshot_ext
const char* pico_output_screenshot (const char* path);

/// @brief Takes a screenshot from a specific portion of the screen.
/// @param path screenshot filepath (NULL uses timestamp in the name)
/// @param r region to screenshot, in logical coordinates
/// @return The filepath of the screenshot.
/// @sa pico_output_screenshot
const char* pico_output_screenshot_ext (const char* path, Pico_Rect r);

/// @brief Plays a sound.
/// @param path path to the audio file
void pico_output_sound (const char* path);

/// @brief Draws text with an internal cursor as reference, like in text editors.
/// The cursor position updates to (x + len_text * FNT_SIZE, y).
/// @param text text to draw
void pico_output_write (const char* text);

/// @brief Draws a line of text with an internal cursor as reference, like in text editors.
/// The cursor position updates to (x, y + FNT_SIZE).
/// @param text text to draw
void pico_output_writeln (const char* text);

/// @}

/// @defgroup State
/// @brief All getters and setters.
/// @{

// GET

/// @brief Gets the origin used to position objects (center, topleft, etc).
/// @sa pico_get_anchor_rotate
Pico_Anchor pico_get_anchor_pos (void);

/// @brief Gets the origin used to rotate objects (center, topleft, etc).
/// @sa pico_get_anchor_pos
Pico_Anchor pico_get_anchor_rotate (void);

/// @brief Gets the color set to clear the screen.
Pico_Color pico_get_color_clear (void);

/// @brief Gets the color set to draw.
Pico_Color pico_get_color_draw (void);

/// @brief Gets the current clipping region.
Pico_Rect pico_get_clip (void);

/// @brief Gets the cropping applied to objects when drawing them.
Pico_Rect pico_get_crop (void);

/// @brief Gets the position of the text cursor.
/// @sa pico_output_write
/// @sa pico_output_writeln
Pico_Pos pico_get_cursor (void);

/// @brief Gets the state of expert mode.
/// @return 1 if enabled, or 0 otherwise
int pico_get_expert (void);

/// @brief Gets the flipping state of objects.
Pico_Flip pico_get_flip (void);

/// @brief Gets the font used to draw texts.
const char* pico_get_font (void);

/// @brief Gets the state of fullscreen mode.
/// @return 1 if enabled, or 0 otherwise
int pico_get_fullscreen (void);

/// @brief Gets the state of grid mode.
/// @return 1 if enabled, or 0 otherwise
int pico_get_grid (void);

/// @brief Gets the state of a key.
/// @param key key constant
/// @return 1 if key is pressed, or 0 otherwise

/// @brief Gets the mouse state.
/// @param pos pointer to retrieve pointer position (may be NULL)
/// @param button which button state to retrieve
/// @return state of specified button
int pico_get_mouse (Pico_Pos* pos, int button);

/// @brief Gets the rotation angle of objects (in degrees).
int pico_get_rotate (void);

/// @brief Gets the scaling factor of objects (percentage).
Pico_Pct pico_get_scale (void);

/// @brief Gets the point of view on the logical window.
Pico_Pos pico_get_scroll (void);

/// @brief Gets the dimensions of the given image.
/// @param file image filepath
Pico_Dim pico_get_dim_image (const char* file);

/// @brief Gets the dimensions of the given text.
/// @param text text to measure
Pico_Dim pico_get_dim_text (const char* text);

/// @brief Gets the window dimensions.
Pico_Dim pico_get_dim_phy (void);

/// @brief Gets the logical dimensions.
Pico_Dim pico_get_dim_log (void);

/// @brief Gets the visibility state of the window.
int pico_get_show (void);

/// @brief Gets the drawing style.
PICO_STYLE pico_get_style (void);

/// @brief Gets the amount of ticks that passed since pico was initialized.
Uint32 pico_get_ticks (void);

/// @brief Gets the aplication title.
const char* pico_get_title (void);

/// @brief Gets the zoom factor.
Pico_Pct pico_get_zoom (void);

// SET

void pico_set_alpha (int alpha);

/// @brief Changes the reference to position objects (center, topleft, etc).
/// @include anchor.c
/// @param anchor anchor for the x and y axis
void pico_set_anchor_pos (Pico_Anchor anchor);

/// @brief Changes the reference to rotate objects (center, topleft, etc).
/// @include anchor.c
/// @param anchor anchor for the x and y axis
void pico_set_anchor_rotate (Pico_Anchor anchor);

/// @brief Changes the clipping area of drawing operations.
/// @param clip clipping region (passing PICO_CLIP_RESET disables it).
void pico_set_clip (Pico_Rect clip);

/// @brief Changes the color used to clear the screen.
/// @param color new color
void pico_set_color_clear (Pico_Color color);

void pico_set_context (char* name);

/// @brief Changes the color used to draw objects.
/// @param color new color
void pico_set_color_draw (Pico_Color color);

/// @brief Changes the cropping that is applied to images before drawing them.
/// @param crop cropping region, which may have 0 area to disable cropping
void pico_set_crop (Pico_Rect crop);

/// @brief Sets the position of the text cursor.
/// @param pos new cursor position
/// @sa pico_output_write
/// @sa pico_output_writeln
void pico_set_cursor (Pico_Pos pos);

/// @brief Sets the window dimensions.
/// @param dim new dimensions
void pico_set_dim_phy (Pico_Dim dim);

/// @brief Sets the logical dimensions.
/// @param dim new dimensions
void pico_set_dim_log (Pico_Dim dim);

/// @brief Toggles the expert mode.
/// @param on 1 to enable it, or 0 to disable it
void pico_set_expert (int on);

/// @brief Sets the flipping state of objects.
void pico_set_flip (Pico_Flip flip);

/// @brief Toggles fullscreen mode.
/// @param on 1 to enable it, or 0 to disable it
void pico_set_fullscreen (int on);

/// @brief Toggles a grid on top of logical pixels.
/// @param on 1 to show it, or 0 to hide it
void pico_set_grid (int on);

void pico_set_pos_phy (Pico_Pos pos);

/// @brief Sets the rotation angle of objects (in degrees).
void pico_set_rotate (int angle);

/// @brief Sets the scaling factor of objects
/// @param scale new scaling for x and y axis (percentage)
void pico_set_scale (Pico_Pct scale);

/// @brief Sets the point of view on the logical window.
/// @param pos new point of view
void pico_set_scroll (Pico_Pos pos);

/// @brief Toggles the aplication window visibility.
/// @param on 1 to show, or 0 to hide
void pico_set_show (int on);

/// @brief Sets the drawing style.
/// @param style new style
void pico_set_style (PICO_STYLE style);

/// @brief Sets the aplication title.
/// @param title new title
void pico_set_title (const char* title);

/// @param pct new factor
void pico_set_zoom (Pico_Pct pct);

/// @}

/// @defgroup Utils
/// @brief Utilities for users
/// @{

/// @brief Asserts condition and shows SDL error on failure.
/// @param x condition to assert
#define pico_assert(x) if (!(x)) { fprintf(stderr,"%s\n",SDL_GetError()); assert(0 && "SDL ERROR"); }

/// @brief Returns the dimensions relative to the logical screen.
/// @param pct percentage (may be out of [0,100])
/// @sa pico_dim_phy
/// @sa pico_dim_ext
Pico_Dim pico_dim_log (Pico_Pct pct);

/// @brief Returns the dimensions relative to the physical screen.
/// @param pct percentage (may be out of [0,100])
/// @sa pico_dim_log
/// @sa pico_dim_ext
Pico_Dim pico_dim_phy (Pico_Pct pct);

/// @brief Returns the dimensions relative to the given rectangle.
/// @param pct percentage (may be out of [0,100])
/// @param dim the reference rectangle
/// @sa pico_dim
Pico_Dim pico_dim_ext (Pico_Pct pct, Pico_Dim d);

/// @brief Returns a coordinate relative to the logical screen rectangle.
/// @param pct percentage (may be out of [0,100])
/// @sa pico_pos_phy
/// @sa pico_pos_ext
Pico_Pos pico_pos_log (Pico_Pct pct);

/// @brief Returns a coordinate relative to the physical screen rectangle.
/// @param pct percentage (may be out of [0,100])
/// @sa pico_pos_log
/// @sa pico_pos_ext
Pico_Pos pico_pos_phy (Pico_Pct pct);

/// @brief Returns a coordinate relative to the given rectangle's position.
/// @param pct percentage (may be out of [0,100])
/// @param r the reference rectangle
/// @param anc anchor for r
/// @sa pico_pos
Pico_Pos pico_pos_ext (Pico_Pct pct, Pico_Rect r, Pico_Anchor anc);

/// @brief Checks if a point is inside a rectangle.
/// Assumes that both primitives use the same anchor.
/// @param pt point
/// @param r rectangle
/// @return 1 if pt is inside r, or 0 otherwise
int pico_pos_vs_rect (Pico_Pos pt, Pico_Rect r);

/// @brief Checks if a point is inside a rectangle.
/// @param pt point
/// @param r rectangle
/// @param ap anchor for pt
/// @param ar anchor for r
/// @return 1 if pt is inside r, or 0 otherwise
int pico_pos_vs_rect_ext (Pico_Pos pt, Pico_Rect r, Pico_Anchor ap, Pico_Anchor ar);

/// @brief Returns a rectangle relative to the logical screen rectangle.
/// @param pos percentage (may be out of [0,100])
/// @param dim percentage (may be out of [0,100])
/// @sa pico_rect_phy
/// @sa pico_rect_ext
Pico_Rect pico_rect_log (Pico_Pct pos, Pico_Pct dim);

/// @brief Returns a rectangle relative to the physical screen rectangle.
/// @param pos percentage (may be out of [0,100])
/// @param dim percentage (may be out of [0,100])
/// @sa pico_rect_log
/// @sa pico_rect_ext
Pico_Rect pico_rect_phy (Pico_Pct pos, Pico_Pct dim);

/// @brief Returns a rectangle relative to the given rectangle.
/// @param pos percentage (may be out of [0,100])
/// @param dim percentage (may be out of [0,100])
/// @param r the reference rectangle
/// @param anc anchor for r
/// @sa pico_rect
Pico_Rect pico_rect_ext (Pico_Pct pos, Pico_Pct dim, Pico_Rect r, Pico_Anchor anc);

/// @brief Checks if two rectangles overlap.
/// Assumes that both rectangles use the same anchor.
/// @param r1 rectangle 1
/// @param r2 rectangle 2
/// @return 1 if r1 and r2 overlap, or 0 otherwise
/// @sa pico_rect_vs_rect_ext
int pico_rect_vs_rect (Pico_Rect r1, Pico_Rect r2);

/// @brief Checks if two rectangles with different anchors overlap.
/// @param r1 rectangle 1
/// @param r2 rectangle 2
/// @param a1 anchor for r1
/// @param a2 anchor for r2
/// @return 1 if r1 and r2 overlap, or 0 otherwise
/// @sa pico_pos_vs_rect
int pico_rect_vs_rect_ext (Pico_Rect r1, Pico_Rect r2, Pico_Anchor a2, Pico_Anchor a1);

/// @}

#ifdef __cplusplus
}
#endif

#endif // PICO_H


#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <stdarg.h>

#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include <assert.h>
#include <string.h>

#ifndef PICO_HASH_H
#define PICO_HASH_H

#include <stdlib.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct pico_hash;

typedef struct pico_hash pico_hash;

pico_hash* pico_hash_create (size_t num_buckets);
void pico_hash_destroy (pico_hash* table);
int pico_hash_add (pico_hash* table, const char* key, void* value);
int pico_hash_rem (pico_hash* table, const char* key);
void* pico_hash_get (pico_hash* table, const char* key);

#ifdef __cplusplus
}
#endif

#endif // PICO_HASH_H

typedef struct pico_hash_pair {
    char *key;
    void *value;
    struct pico_hash_pair *next;
} pico_hash_pair;

typedef struct pico_hash {
    pico_hash_pair **buckets;
    size_t num_buckets;
} pico_hash;

pico_hash* pico_hash_create (size_t num_buckets) {
    pico_hash *table = (pico_hash*)malloc(sizeof(pico_hash));
    if (table == NULL) {
        return NULL;
    }

    table->num_buckets = num_buckets;
    table->buckets = (pico_hash_pair**)calloc(table->num_buckets, sizeof(pico_hash_pair *));
    if (table->buckets == NULL) {
        free(table);
        return NULL;
    }

    return table;
}

void pico_hash_destroy (pico_hash *table) {
    pico_hash_pair *pair;
    pico_hash_pair *tmp;
    for (size_t i = 0; i < table->num_buckets; i++) {
        pair = table->buckets[i];
        while (pair != NULL) {
            tmp = pair;
            pair = pair->next;
            free(tmp->key);
            free(tmp);
        }
    }
    free(table->buckets);
    free(table);
}

size_t _pico_hash_hash (const char *str, size_t num_buckets) {
    size_t hash = 0;
    for (const char *p = str; *p != '\0'; p++) {
        hash = (hash << 5) + hash + *p;
    }
    return hash % num_buckets;
}

int pico_hash_add (pico_hash *table, const char *key, void *value) {
    size_t index = _pico_hash_hash(key, table->num_buckets);

    // Check if the key already exists in the hash table
    pico_hash_pair *pair = table->buckets[index];
    while (pair != NULL) {
        if (strcmp(pair->key, key) == 0) {
            // Key already exists in the hash table, update the value
            pair->value = value;
            return 1;
        }
        pair = pair->next;
    }

    // Key does not exist in the hash table, add a new key-value pair
    pair = (pico_hash_pair*)malloc(sizeof(pico_hash_pair));
    if (pair == NULL) {
        return 0;
    }

    pair->key = strdup(key);
    if (pair->key == NULL) {
        free(pair);
        return 0;
    }

    pair->value = value;
    pair->next = table->buckets[index];
    table->buckets[index] = pair;

    return 1;
}

int pico_hash_rem (pico_hash* table, const char *key) {
    size_t index = _pico_hash_hash(key, table->num_buckets);

    // Search for the key-value pair in the linked list at the appropriate index
    pico_hash_pair *pair = table->buckets[index];
    pico_hash_pair *prev = NULL;
    while (pair != NULL) {
        if (strcmp(pair->key, key) == 0) {
            // Key found, remove the key-value pair from the linked list
            if (prev == NULL) {
                // Key-value pair is at the head of the linked list
                table->buckets[index] = pair->next;
            } else {
                // Key-value pair is not at the head of the linked list
                prev->next = pair->next;
            }
            free(pair->key);
            free(pair);
            return 1;
        }
        prev = pair;
        pair = pair->next;
    }

    // Key not found in the hash table
    return 0;
}

void* pico_hash_get (pico_hash* table, const char* key) {
  size_t index = _pico_hash_hash(key, table->num_buckets);

  // Search for the key-value pair in the linked list at the appropriate index
  pico_hash_pair *pair = table->buckets[index];
  while (pair != NULL) {
    if (strcmp(pair->key, key) == 0) {
      // Key found, return the value
      return pair->value;
    }
    pair = pair->next;
  }

  // Key not found in the hash table, return NULL
  return NULL;
}

#define SDL_ANY PICO_ANY
#define MAX(x,y) ((x) > (y) ? (x) : (y))

static SDL_Window*  WIN;
static int FS = 0;          // fullscreen pending (ignore RESIZED event)

#define REN (SDL_GetRenderer(WIN))

static pico_hash* _pico_hash;

typedef struct Pico_Ctx {
    int          alpha;
    struct {
        Pico_Dim phy;
        Pico_Dim log;
    }            dim;
    int          grid;
    char*        name;
    Pico_Pos     pos;
    Pico_Pos     scroll;
    SDL_Texture* tex;
    Pico_Pct     zoom;
} Pico_Ctx;

Pico_Ctx _ctx = {
    0xFF,
    { PICO_DIM_PHY, PICO_DIM_LOG },
    1,
    NULL,
    {0, 0},
    {0, 0},
    NULL,
    {100, 100},
};

static struct {
    struct {
        Pico_Anchor pos;
        Pico_Anchor rotate;
    } anchor;
    int angle;
    Pico_Rect clip;
    struct {
        Pico_Color clear;
        Pico_Color draw;
    } color;
    Pico_Rect crop;
    Pico_Ctx* ctx;
    struct {
        int x;
        Pico_Pos cur;
    } cursor;
    int expert;
    Pico_Flip flip;
    struct {
        TTF_Font* ttf;
        int h;
    } font;
    int fullscreen;
    PICO_STYLE style;
    Pico_Pct scale;
} S = {
    { {PICO_CENTER, PICO_MIDDLE}, {PICO_CENTER, PICO_MIDDLE} },
    0,
    {0, 0, 0, 0},
    { {0x00,0x00,0x00,0xFF}, {0xFF,0xFF,0xFF,0xFF} },
    {0, 0, 0, 0},
    &_ctx,
    {0, {0,0}},
    0,
    {0, 0},
    {NULL, 0},
    0,
    PICO_FILL,
    {100, 100},
};

static int _noclip () {
    return (S.clip.w == PICO_CLIP_RESET.w) ||
           (S.clip.h == PICO_CLIP_RESET.h);
}

static Pico_Dim _zoom (Pico_Ctx* ctx) {
    return (Pico_Dim) {
        ctx->dim.log.x * ctx->zoom.x / 100,
        ctx->dim.log.y * ctx->zoom.y / 100,
    };
}

static int _anchor_x_ext (int x, int w, int a) {
    return x - (a*w)/100;
}

static int _anchor_y_ext (int y, int h, int a) {
    return y - (a*h)/100;
}

static int _anchor_x (int x, int w) {
    return _anchor_x_ext(x, w, S.anchor.pos.x);
}

static int _anchor_y (int y, int h) {
    return _anchor_y_ext(y, h, S.anchor.pos.y);
}

static int X (int x, int w) {
    return _anchor_x(x,w) - S.ctx->scroll.x;
}

static int Y (int y, int h) {
    return _anchor_y(y,h) - S.ctx->scroll.y;
}

// UTILS

Pico_Dim pico_dim_log (Pico_Pct pct) {
    return pico_dim_ext(pct, S.ctx->dim.log);
}

Pico_Dim pico_dim_phy (Pico_Pct pct) {
    return pico_dim_ext(pct, S.ctx->dim.phy);
}

Pico_Dim pico_dim_ext (Pico_Pct pct, Pico_Dim d) {
    assert(0 <= pct.x && 0 <= pct.y && "negative dimentions");
    return (Pico_Dim){ (pct.x*d.x)/100, (pct.y*d.y)/100};
}

Pico_Pos pico_pos_log (Pico_Pct pct) {
    return pico_pos_ext (
        pct,
        (Pico_Rect){ 0, 0, S.ctx->dim.log.x, S.ctx->dim.log.y },
        (Pico_Anchor) {PICO_LEFT, PICO_TOP}
    );
}

Pico_Pos pico_pos_phy (Pico_Pct pct) {
    return pico_pos_ext (
        pct,
        (Pico_Rect){ 0, 0, S.ctx->dim.phy.x, S.ctx->dim.phy.y },
        (Pico_Anchor) {PICO_LEFT, PICO_TOP}
    );
}

Pico_Pos pico_pos_ext (Pico_Pct pct, Pico_Rect r, Pico_Anchor anc) {
    return (Pico_Pos) {
        _anchor_x_ext(r.x,r.w,anc.x) + (pct.x*r.w)/100,
        _anchor_y_ext(r.y,r.h,anc.y) + (pct.y*r.h)/100
    };
}

int pico_pos_vs_rect (Pico_Pos pt, Pico_Rect r) {
    return pico_pos_vs_rect_ext(pt, r, S.anchor.pos, S.anchor.pos);
}

int pico_pos_vs_rect_ext (Pico_Pos pt, Pico_Rect r, Pico_Anchor ap, Pico_Anchor ar) {
    return pico_rect_vs_rect_ext((Pico_Rect){pt.x, pt.y, 1, 1}, r, ap, ar);
}

Pico_Rect pico_rect_log (Pico_Pct pos, Pico_Pct dim) {
    return pico_rect_ext (
        pos, dim,
        (Pico_Rect){ 0, 0, S.ctx->dim.log.x, S.ctx->dim.log.y},
        (Pico_Anchor) {PICO_LEFT, PICO_TOP}
    );
}

Pico_Rect pico_rect_phy (Pico_Pct pos, Pico_Pct dim) {
    return pico_rect_ext (
        pos, dim,
        (Pico_Rect){ 0, 0, S.ctx->dim.phy.x, S.ctx->dim.phy.y },
        (Pico_Anchor) {PICO_LEFT, PICO_TOP}
    );
}

Pico_Rect pico_rect_ext (Pico_Pct pos, Pico_Pct dim, Pico_Rect r, Pico_Anchor anc) {
    Pico_Pos xy = {
        _anchor_x_ext(r.x,r.w,anc.x) + (pos.x*r.w)/100,
        _anchor_y_ext(r.y,r.h,anc.y) + (pos.y*r.h)/100
    };
    Pico_Dim wh = pico_dim_ext(dim, (Pico_Dim){r.w,r.h});
    return (Pico_Rect) { xy.x,xy.y, wh.x,wh.y };
}

int pico_rect_vs_rect (Pico_Rect r1, Pico_Rect r2) {
    return pico_rect_vs_rect_ext(r1, r2, S.anchor.pos, S.anchor.pos);
}

int pico_rect_vs_rect_ext (Pico_Rect r1, Pico_Rect r2, Pico_Anchor a1, Pico_Anchor a2) {
    assert(S.angle == 0 && "rotation angle != 0");
    r1.x = _anchor_x_ext(r1.x, r1.w, a1.x);
    r1.y = _anchor_y_ext(r1.y, r1.h, a1.y);
    r2.x = _anchor_x_ext(r2.x, r2.w, a2.x);
    r2.y = _anchor_y_ext(r2.y, r2.h, a2.y);
    return SDL_HasIntersection(&r1, &r2);
}

// INIT

void pico_init (int on) {
    if (on) {
        _pico_hash = pico_hash_create(PICO_HASH);
        pico_assert(0 == SDL_Init(SDL_INIT_VIDEO));
        WIN = SDL_CreateWindow (
            PICO_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            S.ctx->dim.phy.x, S.ctx->dim.phy.y,
            (SDL_WINDOW_SHOWN /*| SDL_WINDOW_RESIZABLE*/)
        );
        pico_assert(WIN != NULL);

        SDL_CreateRenderer(WIN, -1, SDL_RENDERER_ACCELERATED/*|SDL_RENDERER_PRESENTVSYNC*/);
        //SDL_CreateRenderer(WIN, -1, SDL_RENDERER_SOFTWARE);

        pico_assert(REN != NULL);
        SDL_SetRenderDrawBlendMode(REN, SDL_BLENDMODE_BLEND);

        TTF_Init();
        Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 1024);

        pico_set_zoom(S.ctx->zoom);
        pico_output_clear();

        SDL_PumpEvents();
        SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
        SDL_SetWindowResizable(WIN, 1);
    } else {
        if (S.font.ttf != NULL) {
            TTF_CloseFont(S.font.ttf);
        }
        Mix_CloseAudio();
        TTF_Quit();
        SDL_DestroyRenderer(REN);
        SDL_DestroyWindow(WIN);
        SDL_Quit();
        pico_hash_destroy(_pico_hash);
    }
}

// INPUT

// OUTPUT

static void _pico_output_present (int force, Pico_Ctx* ctx);

void pico_output_clear (void) {
    SDL_SetRenderDrawColor (REN,
        S.color.clear.r,
        S.color.clear.g,
        S.color.clear.b,
        S.color.clear.a
    );
    if (_noclip()) {
        SDL_RenderClear(REN);
    } else {
        SDL_Rect r;
        SDL_RenderGetClipRect(REN, &r);
        SDL_RenderFillRect(REN, &r);
    }
    SDL_SetRenderDrawColor (REN,
        S.color.draw.r,
        S.color.draw.g,
        S.color.draw.b,
        S.color.draw.a
    );
    _pico_output_present(0, S.ctx);
}

static SDL_Texture* _draw_aux (int w, int h) {
    SDL_Texture* aux = SDL_CreateTexture (
        REN, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
        w, h
    );
    SDL_SetTextureBlendMode(aux, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(REN, aux);
    SDL_SetRenderDrawColor(REN, 0, 0, 0, 0);   // transparent
    SDL_RenderClear(REN);
    SDL_RenderFillRect(REN, NULL);
    SDL_SetRenderDrawColor (REN,
        S.color.draw.r,
        S.color.draw.g,
        S.color.draw.b,
        S.color.draw.a
    );
    return aux;
}

static void _pico_output_draw_tex (Pico_Pos pos, SDL_Texture* tex, Pico_Dim dim);

void pico_output_draw_buffer (Pico_Pos pos, const Pico_Color buffer[], Pico_Dim dim) {
    SDL_Surface* sfc = SDL_CreateRGBSurfaceWithFormatFrom (
        (void*)buffer, dim.x, dim.y,
        32, 4*dim.x, SDL_PIXELFORMAT_RGBA32
    );
    SDL_Texture *aux = SDL_CreateTextureFromSurface(REN, sfc);
    _pico_output_draw_tex(pos, aux, dim);
    SDL_FreeSurface(sfc);
    SDL_DestroyTexture(aux);
}

static void _pico_output_draw_tex (Pico_Pos pos, SDL_Texture* tex, Pico_Dim dim) {
    Pico_Rect rct;
    SDL_QueryTexture(tex, NULL, NULL, &rct.w, &rct.h);

    Pico_Rect crp = S.crop;
    if (S.crop.w == 0) {
        crp.w = rct.w;
    }
    if (S.crop.h == 0) {
        crp.h = rct.h;
    }

    if (dim.x==PICO_DIM_KEEP.x && dim.y==PICO_DIM_KEEP.y) {
        // normal image size
        rct.w = crp.w;  // (or copy from crop)
        rct.h = crp.h;  // (or copy from crop)
    } else if (dim.x == 0) {
        // adjust w based on h
        rct.w = rct.w * (dim.y / (float)rct.h);
        rct.h = dim.y;
    } else if (dim.y == 0) {
        // adjust h based on w
        rct.h = rct.h * (dim.x / (float)rct.w);
        rct.w = dim.x;
    } else {
        rct.w = dim.x;
        rct.h = dim.y;
    }

    // SCALE
    rct.w = (S.scale.x*rct.w)/100; // * GRAPHICS_SET_SCALE_W;
    rct.h = (S.scale.y*rct.h)/100; // * GRAPHICS_SET_SCALE_H;

    // ANCHOR / PAN
    rct.x = X(pos.x, rct.w);
    rct.y = Y(pos.y, rct.h);

    // ROTATE
    Pico_Pos rot = {
        (S.anchor.rotate.x*rct.w)/100,
        (S.anchor.rotate.y*rct.h)/100
    };

    SDL_RenderCopyEx(REN, tex,
        &crp, &rct,
        S.angle + (S.flip.x && S.flip.y ? 180 : 0),
        &rot,
        S.flip.y ? SDL_FLIP_VERTICAL : (S.flip.x ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE)
    );
    _pico_output_present(0, S.ctx);
}

void pico_output_draw_image (Pico_Pos pos, const char* path) {
    pico_output_draw_image_ext(pos, path, PICO_DIM_KEEP);
}

void pico_output_draw_image_ext (Pico_Pos pos, const char* path, Pico_Dim dim) {
    SDL_Texture* tex = (SDL_Texture*)pico_hash_get(_pico_hash, path);
    if (tex == NULL) {
        tex = IMG_LoadTexture(REN, path);
        pico_hash_add(_pico_hash, path, tex);
    }
    pico_assert(tex != NULL);
    _pico_output_draw_tex(pos, tex, dim);
}

// TODO: Test me for flip and rotate
void pico_output_draw_line (Pico_Pos p1, Pico_Pos p2) {
    Pico_Pos pos = {
        _anchor_x(SDL_min(p1.x,p2.x),1),
        _anchor_y(SDL_min(p1.y,p2.y),1)
    };
    SDL_Rect clip;
    SDL_RenderGetClipRect(REN, &clip);
    SDL_Texture* aux = _draw_aux (
        SDL_abs(p1.x-p2.x) + 1,
        SDL_abs(p1.y-p2.y) + 1
    );
    SDL_RenderDrawLine(REN, p1.x-pos.x,p1.y-pos.y, p2.x-pos.x,p2.y-pos.y);
    SDL_SetRenderTarget(REN, S.ctx->tex);
    SDL_RenderSetClipRect(REN, &clip);
    Pico_Anchor anc = S.anchor.pos;
    S.anchor.pos = (Pico_Anchor){PICO_LEFT, PICO_TOP};
    _pico_output_draw_tex(pos, aux, PICO_DIM_KEEP);
    S.anchor.pos = anc;
    SDL_DestroyTexture(aux);
}

void pico_output_draw_pixel (Pico_Pos pos) {
    SDL_RenderDrawPoint(REN, X(pos.x,1), Y(pos.y,1) );
    _pico_output_present(0, S.ctx);
}

void pico_output_draw_pixels (const Pico_Pos* apos, int count) {
    Pico_Pos vec[count];
    for (int i=0; i<count; i++) {
        vec[i].x = X(apos[i].x,1);
        vec[i].y = Y(apos[i].y,1);
    }
    SDL_RenderDrawPoints(REN, vec, count);
    _pico_output_present(0, S.ctx);
}

// TODO: Test me for flip and rotate
void pico_output_draw_rect (Pico_Rect rect) {
    Pico_Pos pos = {rect.x, rect.y};
    SDL_Rect clip;
    SDL_RenderGetClipRect(REN, &clip);
    SDL_Texture* aux = _draw_aux(rect.w, rect.h);
    rect.x = 0;
    rect.y = 0;
    switch (S.style) {
        case PICO_FILL:
            SDL_RenderFillRect(REN, &rect);
            break;
        case PICO_STROKE:
            SDL_RenderDrawRect(REN, &rect);
            break;
    }
    SDL_SetRenderTarget(REN, S.ctx->tex);
    SDL_RenderSetClipRect(REN, &clip);
    _pico_output_draw_tex(pos, aux, PICO_DIM_KEEP);
    SDL_DestroyTexture(aux);
}

// TODO: Test me for flip and rotate
void pico_output_draw_tri (Pico_Rect rect) {
    Pico_Pos pos = {rect.x, rect.y};
    SDL_Rect clip;
    SDL_RenderGetClipRect(REN, &clip);
    SDL_Texture* aux = _draw_aux(rect.w, rect.h);
    switch (S.style) {
        case PICO_FILL:
            filledTrigonRGBA (REN,
                0, 0,
                0, rect.h - 1,
                rect.w - 1, rect.h - 1,
                S.color.draw.r, S.color.draw.g, S.color.draw.b, S.color.draw.a
            );
            break;
        case PICO_STROKE:
            trigonRGBA (REN,
                0, 0,
                0, rect.h - 1,
                rect.w - 1, rect.h - 1,
                S.color.draw.r, S.color.draw.g, S.color.draw.b, S.color.draw.a
            );
            break;
    }
    SDL_SetRenderTarget(REN, S.ctx->tex);
    SDL_RenderSetClipRect(REN, &clip);
    _pico_output_draw_tex(pos, aux, PICO_DIM_KEEP);
    SDL_DestroyTexture(aux);
}

// TODO: Test me for flip and rotate
void pico_output_draw_oval (Pico_Rect rect) {
    Pico_Pos pos = {rect.x, rect.y};
    SDL_Rect clip;
    SDL_RenderGetClipRect(REN, &clip);
    SDL_Texture* aux = _draw_aux(rect.w, rect.h);
    switch (S.style) {
        case PICO_FILL:
            filledEllipseRGBA (REN,
                rect.w/2, rect.h/2, rect.w/2, rect.h/2,
                S.color.draw.r, S.color.draw.g, S.color.draw.b, S.color.draw.a
            );
            break;
        case PICO_STROKE:
            ellipseRGBA (REN,
                rect.w/2, rect.h/2, rect.w/2, rect.h/2,
                S.color.draw.r, S.color.draw.g, S.color.draw.b, S.color.draw.a
            );
            break;
    }
    SDL_SetRenderTarget(REN, S.ctx->tex);
    SDL_RenderSetClipRect(REN, &clip);
    _pico_output_draw_tex(pos, aux, PICO_DIM_KEEP);
    SDL_DestroyTexture(aux);
}

void pico_output_draw_poly (const Pico_Pos* apos, int count) {
    Sint16 ax[count], ay[count];
    int minx = INT_MAX, maxx = INT_MIN, miny = INT_MAX, maxy = INT_MIN;
    for (int i = 0; i < count; i++) {
        ax[i] = apos[i].x;
        ay[i] = apos[i].y;
        minx = SDL_min(ax[i], minx);
        maxx = SDL_max(ax[i], maxx);
        miny = SDL_min(ay[i], miny);
        maxy = SDL_max(ay[i], maxy);
    }
    for (int i = 0; i < count; i++) {
        ax[i] -= minx;
        ay[i] -= miny;
    }

    Pico_Pos pos = {
        _anchor_x(minx,1),
        _anchor_y(miny,1)
    };
    SDL_Rect clip;
    SDL_RenderGetClipRect(REN, &clip);
    SDL_Texture* aux = _draw_aux(maxx-minx+1, maxy-miny+1);
    switch (S.style) {
        case PICO_FILL:
            filledPolygonRGBA(REN,
                ax, ay, count,
                S.color.draw.r, S.color.draw.g, S.color.draw.b, S.color.draw.a
            );
            break;
        case PICO_STROKE:
            polygonRGBA(REN,
                ax, ay, count,
                S.color.draw.r, S.color.draw.g, S.color.draw.b, S.color.draw.a
            );
            break;
    }
    SDL_SetRenderTarget(REN, S.ctx->tex);
    SDL_RenderSetClipRect(REN, &clip);
    Pico_Anchor anc = S.anchor.pos;
    S.anchor.pos = (Pico_Anchor){PICO_LEFT, PICO_TOP};
    _pico_output_draw_tex(pos, aux, PICO_DIM_KEEP);
    S.anchor.pos = anc;
    SDL_DestroyTexture(aux);
}

void pico_output_draw_text (Pico_Pos pos, const char* text) {
    pico_output_draw_text_ext(pos, text, PICO_DIM_KEEP);
}

void pico_output_draw_text_ext (Pico_Pos pos, const char* text, Pico_Dim dim) {
    if (text[0] == '\0') return;

    pico_assert(S.font.ttf != NULL);
    SDL_Surface* sfc = TTF_RenderText_Blended(S.font.ttf, text, S.color.draw);
    pico_assert(sfc != NULL);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(REN, sfc);
    pico_assert(tex != NULL);
    _pico_output_draw_tex(pos, tex, dim);
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(sfc);
}

static void pico_output_draw_fmt_va (Pico_Pos pos, const char* fmt, Pico_Dim dim, va_list args) {
    static char text[1024];
    vsprintf(text, fmt, args);
    pico_output_draw_text_ext(pos, text, dim);
}

void pico_output_draw_fmt (Pico_Pos pos, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    pico_output_draw_fmt_va(pos, fmt, PICO_DIM_KEEP, args);
    va_end(args);
}

void pico_output_draw_fmt_ext (Pico_Pos pos, Pico_Dim dim, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    pico_output_draw_fmt_va(pos, fmt, dim, args);
    va_end(args);
}

static void _show_grid (Pico_Ctx* ctx) {
    if (!ctx->grid) return;

    SDL_SetRenderDrawColor(REN, 0x77, 0x77, 0x77, 0x77);

    Pico_Dim Z = _zoom(ctx);

    if ((ctx->dim.phy.x % Z.x == 0) && (Z.x < ctx->dim.phy.x)) {
        for (int i=0; i<=ctx->dim.phy.x; i+=(ctx->dim.phy.x/Z.x)) {
            SDL_RenderDrawLine(REN, ctx->pos.x+i, ctx->pos.y, ctx->pos.x+i, ctx->dim.phy.y);
        }
    }

    if ((ctx->dim.phy.y % Z.y == 0) && (Z.y < ctx->dim.phy.y)) {
        for (int j=0; j<=ctx->dim.phy.y; j+=(ctx->dim.phy.y/Z.y)) {
            SDL_RenderDrawLine(REN, ctx->pos.x, ctx->pos.y+j, ctx->dim.phy.x, ctx->pos.y+j);
        }
    }

    Pico_Color c = S.color.draw;
    SDL_SetRenderDrawColor(REN, c.r, c.g, c.b, c.a);
}

static void _pico_output_present (int force, Pico_Ctx* ctx) {
    if (S.expert && !force) return;

    SDL_Rect clip;
    SDL_RenderGetClipRect(REN, &clip);

    SDL_Texture* up = (ctx == &_ctx) ? NULL : _ctx.tex;

    SDL_SetRenderTarget(REN, up);
    SDL_RenderSetLogicalSize(REN, _ctx.dim.phy.x, _ctx.dim.phy.y);

    if (ctx->name == NULL) {
        SDL_SetRenderDrawColor(REN, 0x77, 0x77, 0x77, 0x77);
        SDL_RenderClear(REN);
        Pico_Color c = S.color.draw;
        SDL_SetRenderDrawColor(REN, c.r, c.g, c.b, c.a);
    }

    SDL_Rect dst = { ctx->pos.x, ctx->pos.y, ctx->dim.phy.x, ctx->dim.phy.y };
    SDL_SetTextureAlphaMod(ctx->tex, ctx->alpha);
printf(">>> [%d] %d\n", ctx==&_ctx, ctx->alpha);
    SDL_RenderCopy(REN, ctx->tex, NULL, &dst);
    //SDL_SetTextureAlphaMod(ctx->tex, 0xFF);

    if (ctx->name == NULL) {
        _show_grid(ctx);
    }

    if (ctx->name == NULL) {
        SDL_RenderPresent(REN);
    }

    if (ctx->name != NULL) {
        _pico_output_present(force, &_ctx);
    }

    Pico_Dim Z = _zoom(ctx);
    SDL_RenderSetLogicalSize(REN, Z.x, Z.y);
    SDL_SetRenderTarget(REN, ctx->tex);
    SDL_RenderSetClipRect(REN, &clip);
}

void pico_output_present (void) {
    _pico_output_present(1, &_ctx);
}

static void _pico_output_sound_cache (const char* path, int cache) {
    Mix_Chunk* mix = NULL;

    if (cache) {
        mix = (Mix_Chunk*)pico_hash_get(_pico_hash, path);
        if (mix == NULL) {
            mix = Mix_LoadWAV(path);
            pico_hash_add(_pico_hash, path, mix);
        }
    } else {
        mix = Mix_LoadWAV(path);
    }
    pico_assert(mix != NULL);

    Mix_PlayChannel(-1, mix, 0);

    if (!cache) {
        Mix_FreeChunk(mix);
    }
}

const char* pico_output_screenshot (const char* path) {
    Pico_Dim Z = _zoom(S.ctx);
    return pico_output_screenshot_ext(
        path,
        (Pico_Rect){0,0,Z.x,Z.y}
    );
}

const char* pico_output_screenshot_ext (const char* path, Pico_Rect r) {
    const char* ret;
    if (path != NULL) {
        ret = path;
    } else {
        static char _path_[32] = "";
        time_t ts = time(NULL);
        struct tm* ti = localtime(&ts);
        assert(strftime(_path_,32,"pico-sdl-%Y%m%d-%H%M%S.png",ti) == 28);
        ret = _path_;
    }

    void* buf = malloc(4*r.w*r.h);
    SDL_RenderReadPixels(REN, &r, SDL_PIXELFORMAT_RGBA32, buf, 4*r.w);
    SDL_Surface *sfc = SDL_CreateRGBSurfaceWithFormatFrom(buf, r.w, r.h, 32, 4*r.w,
                                                          SDL_PIXELFORMAT_RGBA32);
    assert(IMG_SavePNG(sfc,ret)==0 && "saving screenshot");
    free(buf);
    SDL_FreeSurface(sfc);
    return ret;
}

void pico_output_sound (const char* path) {
    _pico_output_sound_cache(path, 1);
}

static void _pico_output_write_aux (const char* text, int isln) {
    if (strlen(text) == 0) {
        if (isln) {
            S.cursor.cur.x = S.cursor.x;
            S.cursor.cur.y += S.font.h;
        }
        return;
    }

    pico_assert(S.font.ttf != NULL);
    SDL_Surface* sfc = TTF_RenderText_Blended (
        S.font.ttf, text,
        (Pico_Color) { S.color.draw.r, S.color.draw.g,
                       S.color.draw.b, S.color.draw.a }
    );
    pico_assert(sfc != NULL);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(REN, sfc);
    pico_assert(tex != NULL);

    int w, h;
    TTF_SizeText(S.font.ttf, text, &w,&h);
    Pico_Rect rct = { X(S.cursor.cur.x,0),Y(S.cursor.cur.y,0), w,h };
    SDL_RenderCopy(REN, tex, NULL, &rct);
    _pico_output_present(0, S.ctx);

    S.cursor.cur.x += w;
    if (isln) {
        S.cursor.cur.x = S.cursor.x;
        S.cursor.cur.y += S.font.h;
    }

    SDL_DestroyTexture(tex);
    SDL_FreeSurface(sfc);
}

void pico_output_write (const char* text) {
    _pico_output_write_aux(text, 0);
}

void pico_output_writeln (const char* text) {
    _pico_output_write_aux(text, 1);
}


// STATE

// GET

Pico_Anchor pico_get_anchor_pos (void) {
    return S.anchor.pos;
}

Pico_Anchor pico_get_anchor_rotate (void) {
    return S.anchor.rotate;
}

Pico_Rect pico_get_clip (void) {
    return S.clip;
}

Pico_Color pico_get_color_clear (void) {
    return S.color.clear;
}

Pico_Color pico_get_color_draw (void) {
    return S.color.draw;
}

Pico_Pos pico_get_cursor (void) {
    return S.cursor.cur;
}

int pico_get_expert (void) {
    return S.expert;
}

Pico_Flip pico_get_flip (void) {
    return S.flip;
}

const char* pico_get_font (void) {
    return TTF_FontFaceFamilyName(S.font.ttf);
}

int pico_get_fullscreen (void) {
    return S.fullscreen;
}

int pico_get_grid (void) {
    return S.ctx->grid;
}

int pico_get_mouse (Pico_Pos* pos, int button) {
    Pico_Pos local;
    if (pos == NULL) {
        pos = &local;
    }

    Uint32 masks = SDL_GetMouseState(&pos->x, &pos->y);
    if (button == 0) {
        masks = 0;
    }

    // TODO: bug in SDL?
    // https://discourse.libsdl.org/t/sdl-getmousestate-and-sdl-rendersetlogicalsize/20288/7
    Pico_Dim Z = _zoom(S.ctx);
    pos->x = pos->x * Z.x / S.ctx->dim.phy.x;
    pos->y = pos->y * Z.y / S.ctx->dim.phy.y;
    pos->x += S.ctx->scroll.x;
    pos->y += S.ctx->scroll.y;

    return masks & SDL_BUTTON(button);
}

Pico_Rect pico_get_crop (void) {
    return S.crop;
}

int pico_get_rotate (void) {
    return S.angle;
}

Pico_Pct pico_get_scale (void) {
    return S.scale;
}

Pico_Pos pico_get_scroll (void) {
    return S.ctx->scroll;
}

Pico_Dim pico_get_dim_image (const char* file) {
    SDL_Texture* tex = (SDL_Texture*)pico_hash_get(_pico_hash, file);
    if (tex == NULL) {
        tex = IMG_LoadTexture(REN, file);
        pico_hash_add(_pico_hash, file, tex);
    }
    pico_assert(tex != NULL);

    Pico_Dim dim;
    SDL_QueryTexture(tex, NULL, NULL, &dim.x, &dim.y);
    return dim;
}

Pico_Dim pico_get_dim_text (const char* text) {
    if (text[0] == '\0') {
        return (Pico_Dim){0, 0};
    }

    SDL_Surface* sfc = TTF_RenderText_Blended(S.font.ttf, text,
                                              (Pico_Color){0,0,0,255});
    pico_assert(sfc != NULL);
    Pico_Dim dim = {sfc->w, sfc->h};
    SDL_FreeSurface(sfc);
    return dim;
}

Pico_Dim pico_get_dim_phy (void) {
    return S.ctx->dim.phy;
}

Pico_Dim pico_get_dim_log (void) {
    return S.ctx->dim.log;
}

int pico_get_show (void) {
    return SDL_GetWindowFlags(WIN) & SDL_WINDOW_SHOWN;
}

PICO_STYLE pico_get_style (void) {
    return S.style;
}

Uint32 pico_get_ticks (void) {
    return SDL_GetTicks();
}

const char* pico_get_title (void) {
    return SDL_GetWindowTitle(WIN);
}

Pico_Pct pico_get_zoom (void) {
    return S.ctx->zoom;
}

// SET

void pico_set_alpha (int alpha) {
    S.ctx->alpha = alpha;
    _pico_output_present(0, S.ctx);
}

void pico_set_anchor_pos (Pico_Anchor anchor) {
    S.anchor.pos = anchor;
}

void pico_set_anchor_rotate (Pico_Anchor anchor) {
    S.anchor.rotate = anchor;
}

void pico_set_clip (Pico_Rect clip) {
    S.clip = clip;
    if (_noclip()) {
        Pico_Dim dim = _zoom(S.ctx);
        clip.w = dim.x;
        clip.h = dim.y;
    } else {
        clip.x = X(clip.x, clip.w);
        clip.y = Y(clip.y, clip.h);
    }
    SDL_RenderSetClipRect(REN, &clip);
}

void pico_set_color_clear (Pico_Color color) {
    S.color.clear = color;
}

void pico_set_color_draw  (Pico_Color color) {
    S.color.draw = color;
    SDL_SetRenderDrawColor (REN,
        S.color.draw.r,
        S.color.draw.g,
        S.color.draw.b,
        S.color.draw.a
    );
}

void pico_set_context (char* name) {
    if (name == NULL) {
        S.ctx = &_ctx;
    } else {
        Pico_Ctx* ctx = (Pico_Ctx*)pico_hash_get(_pico_hash, name);
        if (ctx == NULL) {
            ctx = malloc(sizeof(Pico_Ctx));
            *ctx = (Pico_Ctx) {
                0xFF,
                { {0,0}, {0,0} },
                1,
                name,
                {0, 0},
                {0, 0},
                NULL,
                {100, 100},
            };
            pico_hash_add(_pico_hash, name, ctx);
        }
        pico_assert(ctx != NULL);
        S.ctx = ctx;
    }
}

void pico_set_crop (Pico_Rect crop) {
    S.crop = crop;
}

void pico_set_cursor (Pico_Pos pos) {
    S.cursor.cur = pos;
    S.cursor.x   = pos.x;
}

void pico_set_dim_phy (Pico_Dim dim) {
    S.ctx->dim.phy = dim;
    if (S.ctx->name != NULL) {
        return;
    }

    assert(!S.fullscreen);
    SDL_SetWindowSize(WIN, dim.x, dim.y);
    Pico_Dim new = _zoom(S.ctx);
    SDL_Rect clip = { 0, 0, new.x, new.y };
    SDL_RenderSetClipRect(REN, &clip);
}

void pico_set_dim_log (Pico_Dim dim) {
    S.ctx->dim.log = dim;
    pico_set_zoom(S.ctx->zoom);
}

void pico_set_expert (int on) {
    S.expert = on;
}

void pico_set_flip (Pico_Flip flip) {
    S.flip = flip;
}

void pico_set_fullscreen (int on) {
    static Pico_Dim _old;
    if ((on && S.fullscreen) || (!on && !S.fullscreen)) {
        return;
    }
    FS = 1;

    Pico_Dim new;
    if (on) {
        _old = S.ctx->dim.phy;
        pico_assert(0 == SDL_SetWindowFullscreen(WIN, SDL_WINDOW_FULLSCREEN_DESKTOP));
        SDL_Delay(50);    // TODO: required for some reason
        SDL_GetWindowSize(WIN, &new.x, &new.y);
    } else {
        pico_assert(0 == SDL_SetWindowFullscreen(WIN, 0));
        new = _old;
    }

    S.fullscreen = 0;           // cannot set_dim_win with fullscreen on
    pico_set_dim_phy(new);
    S.fullscreen = on;
}

void pico_set_grid (int on) {
    S.ctx->grid = on;
    _pico_output_present(0, &_ctx);
}

void pico_set_pos_phy (Pico_Pos pos) {
    if (S.ctx->name == NULL) {
        assert(0 && "TODO: window position");
    }
    S.ctx->pos = (Pico_Pos) {
        _anchor_x(pos.x, S.ctx->dim.phy.x),
        _anchor_y(pos.y, S.ctx->dim.phy.y),
    };
}

void pico_set_rotate (int angle) {
    S.angle = angle;
}

void pico_set_scale (Pico_Pct scale) {
    // TODO: checks???
    S.scale = scale;
}

void pico_set_scroll (Pico_Pos pos) {
    S.ctx->scroll = pos;
}

void pico_set_show (int on) {
    if (on) {
        SDL_ShowWindow(WIN);
        _pico_output_present(0, &_ctx);
    } else {
        SDL_HideWindow(WIN);
    }
}

void pico_set_style (PICO_STYLE style) {
    S.style = style;
}

void pico_set_title (const char* title) {
    SDL_SetWindowTitle(WIN, title);
}

void pico_set_zoom (Pico_Pct pct) {
    Pico_Dim old = _zoom(S.ctx);
    S.ctx->zoom = pct;
    Pico_Dim new = _zoom(S.ctx);
    
    int dx = new.x - old.x;
    int dy = new.y - old.y;

    pico_set_scroll ((Pico_Pos) {
        S.ctx->scroll.x - (dx * S.anchor.pos.x / 100),
        S.ctx->scroll.y - (dy * S.anchor.pos.y / 100),
    });

    SDL_DestroyTexture(S.ctx->tex);
    S.ctx->tex = SDL_CreateTexture (
        REN, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
        new.x, new.y
    );
    pico_assert(S.ctx->tex != NULL);
    SDL_SetTextureBlendMode(S.ctx->tex, SDL_BLENDMODE_BLEND);
    SDL_RenderSetLogicalSize(REN, new.x, new.y);
    SDL_SetRenderTarget(REN, S.ctx->tex);

    // TODO: need to init w/ explicit SetClip to save w/h
    //       do not pass NULL, GetClip would also return w=0,h=0
    SDL_Rect clip = { 0, 0, new.x, new.y };
    SDL_RenderSetClipRect(REN, &clip);
}

int main (void) {
    pico_init(1);
    pico_set_title("Context");

    puts("rect pos=30, dim=50");
    pico_output_clear();
    Pico_Rect r1 = pico_rect_phy (
        (Pico_Pct) {30, 30},
        (Pico_Pct) {50, 50}
    );
    pico_set_context("rect");
    pico_set_dim_phy((Pico_Dim){r1.w,r1.h});
    pico_set_dim_log((Pico_Dim){r1.w/10,r1.h/10});
    pico_set_pos_phy((Pico_Pos){r1.x,r1.y});

    pico_set_color_clear((Pico_Color){0xFF, 0xFF, 0xFF, 0xFF});
    pico_output_clear();
    SDL_Delay(500);

    puts("red centered under white");
    Pico_Rect r2 = pico_rect_log (
        (Pico_Pct){50, 50},
        (Pico_Pct){50, 50}
    );
    pico_set_color_draw((Pico_Color){0xFF,0x00,0x00,0xFF});
    pico_output_draw_rect(r2);
    SDL_Delay(500);

    pico_set_context(NULL);
    pico_output_clear();
    pico_set_context("rect");
    pico_set_alpha(0x88);
    SDL_Delay(500);
    pico_set_alpha(0x88);
    SDL_Delay(500);
    pico_set_alpha(0x88);
    SDL_Delay(500);

    return 0;
}
