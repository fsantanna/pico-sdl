#ifndef PICO_H
#define PICO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include "keys.h"
#include "events.h"
#include "colors.h"
#include "anchors.h"

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
#define PICO_DIM_PHY ((Pico_Abs_Dim) {500,500})
#define PICO_DIM_LOG ((Pico_Abs_Dim) {100,100})
#define PICO_HASH_BUK  128

/// @brief Asserts condition and shows SDL error on failure.
/// @param x condition to assert
#define pico_assert(x)  \
    if (!(x)) {         \
        fprintf(stderr,"%s\n",SDL_GetError()); assert(0 && "SDL ERROR"); \
    }

#define pico_assert_0(x) pico_assert((x) == 0)
#define pico_assert_X(x) pico_assert((x) != NULL)

typedef struct {
    float w;
    float h;
} SDL_FDim;

typedef SDL_Rect  Pico_Abs_Rect;
typedef SDL_Point Pico_Abs_Pos;

typedef struct {
    int w;
    int h;
} Pico_Abs_Dim;

// MODES:
// '!': raw (pixels)
// '%': pct (0.0-1.0)
// '#': tile (1-indexed grid coordinates)
// '*': mix  (TODO)
// '?': unk/err

typedef struct Pico_Rel_Rect {
    char mode;
    struct {
        float x, y;
        float w, h;
    };
    Pico_Anchor anchor;
} Pico_Rel_Rect;

typedef struct {
    char mode;
    struct {
        float w, h;
    };
} Pico_Rel_Dim;

typedef struct {
    char mode;
    struct {
        float x, y;
    };
    Pico_Anchor anchor;
} Pico_Rel_Pos;

typedef enum {
    PICO_STYLE_FILL, PICO_STYLE_STROKE
} PICO_STYLE;

typedef struct {
    Pico_Abs_Dim dim;
    int fps;
    int frame;
    int done;
} Pico_Video;

typedef enum {
    PICO_FLIP_NONE       = SDL_FLIP_NONE,
    PICO_FLIP_HORIZONTAL = SDL_FLIP_HORIZONTAL,
    PICO_FLIP_VERTICAL   = SDL_FLIP_VERTICAL,
    PICO_FLIP_BOTH       = SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL,
} PICO_FLIP;

typedef struct {
    int         angle;
    Pico_Anchor anchor;
} Pico_Rot;

typedef struct {
    Pico_Color    color;
    const char*   font;
    PICO_STYLE    style;
} Pico_Layer_Pencil;

typedef struct {
    unsigned char alpha;
    Pico_Color    color;
    PICO_FLIP     flip;
    int           grid;
    Pico_Rot      rotate;
} Pico_Layer_Effect;

typedef struct {
    int           clear;
    Pico_Abs_Dim  dim;
    Pico_Abs_Dim  tile;
    Pico_Rel_Rect dst;
    Pico_Rel_Rect src;
    Pico_Rel_Rect clip;
} Pico_Layer_Scene;

typedef struct {
    int          fs;
    int          show;
    const char*  title;
} Pico_Window;

/// @}

/// @defgroup Init
/// @brief Functions and values used in initialization.
/// @{

/// @brief Initializes and terminates pico.
/// @include init.c
/// @param on 1 to initialize, or 0 to terminate
void pico_init (int on);

/// @brief Pushes a quit event.
void pico_quit (void);

/// @}

/// @defgroup Input
/// @brief Event handling.
/// @{

/// @brief Stops the program until the given number of milliseconds have passed.
/// Equivalent to `pico_input_event_timeout(NULL, PICO_EVENT_NONE, ms)`.
/// @include delay.c
/// @param ms milliseconds to wait
/// @return elapsed time in milliseconds (delta time)
int pico_input_delay (int ms);

/// @brief Stops the program until a matching event occurs.
/// Equivalent to `pico_input_event_timeout(evt, type, -1)`.
/// @include event.c
/// @param evt where to save the event data, or NULL to ignore
/// @param type type of event to wait for (PICO_EVENT_ANY for any)
/// @return elapsed time in milliseconds (delta time)
/// @sa pico_input_event_timeout
int pico_input_event (Pico_Event* evt, int type);

/// @brief Stops the program until a matching event occurs or a timeout is reached.
/// All input functions delegate to this one.
/// Internal events (quit/exit, window resize, ctrl+zoom/scroll/grid) are
/// handled automatically and never forwarded.
/// On timeout, evt->type is set to PICO_EVENT_NONE.
/// @include event_timeout.c
/// @param evt where to save the event data, or NULL to ignore
/// @param type type of event to wait for (PICO_EVENT_ANY for any)
/// @param timeout time limit in milliseconds, or -1 to wait forever
/// @return elapsed time in milliseconds (delta time)
/// @sa pico_input_event
int pico_input_event_timeout (Pico_Event* evt, int type, int timeout);

/// @brief Blocks in an event loop until the window is closed.
/// Equivalent to `pico_input_event(NULL, PICO_EVENT_QUIT)`.
void pico_input_loop (void);

/// @}

/// @defgroup Output
/// @brief Draw primitives, play sounds, etc.
/// @{

/// @brief Clears screen with color set by @ref pico_set_effect_color.
void pico_output_clear (void);

/// @brief Draws an RGBA pixmap.
/// @param key layer key for caching (required)
/// @param dim pixmap dimensions in pixels
/// @param pixmap the RGBA pixel data
/// @param rect drawing rectangle (mode determines coordinate interpretation)
/// @sa pico_output_draw_image
void pico_output_draw_pixmap (const char* key, Pico_Abs_Dim dim,
                              const Pico_Color pixmap[],
                              Pico_Rel_Rect rect);

/// @brief Draws an image.
/// @param path path to the image file
/// @param rect target position and dimension (mode determines coordinates)
/// @sa pico_output_draw_pixmap
void pico_output_draw_image (const char* path, Pico_Rel_Rect rect);

/// @brief Draws a line.
/// @param p1 first endpoint position (mode determines coordinates)
/// @param p2 second endpoint position (mode determines coordinates)
void pico_output_draw_line (Pico_Rel_Pos p1, Pico_Rel_Pos p2);

/// @brief Draws a single pixel.
/// @param pos drawing position (mode determines coordinates)
void pico_output_draw_pixel (Pico_Rel_Pos pos);

/// @brief Draws a batch of pixels.
/// @param n number of positions
/// @param ps array of positions (mode determines coordinates)
void pico_output_draw_pixels (int n, const Pico_Rel_Pos* ps);

/// @brief Draws a layer onto the current layer.
/// @param key layer key (must exist)
/// @param rect target position and dimension (mode determines
///        coordinates); NULL uses the layer's scene.dst defaults
///        (full bounds in cur).
void pico_output_draw_layer (const char* key, const Pico_Rel_Rect* rect);

/// @brief Draws a rectangle.
/// @param rect rectangle to draw (mode determines coordinates)
void pico_output_draw_rect (Pico_Rel_Rect rect);

/// @brief Draws a triangle.
/// @param p1 first vertex position (mode determines coordinates)
/// @param p2 second vertex position (mode determines coordinates)
/// @param p3 third vertex position (mode determines coordinates)
void pico_output_draw_tri (Pico_Rel_Pos p1, Pico_Rel_Pos p2, Pico_Rel_Pos p3);

/// @brief Draws an ellipse.
/// @param rect bounding rectangle (mode determines coordinates)
void pico_output_draw_oval (Pico_Rel_Rect rect);

/// @brief Draws a polygon.
/// @param n number of vertices
/// @param ps array of vertex positions (mode determines coordinates)
void pico_output_draw_poly (int n, const Pico_Rel_Pos* ps);

/// @brief Draws text (shared caching by text content).
/// @param text text to draw
/// @param rect drawing rectangle (mode determines coordinates)
void pico_output_draw_text (const char* text, Pico_Rel_Rect rect);

/// @brief Draws text with explicit realm mode and layer key.
/// @param mode realm mode ('!' exclusive, '=' shared,
///             '~' replace)
/// @param key layer key
/// @param text text to draw
/// @param rect drawing rectangle (mode determines coordinates)
void pico_output_draw_text_mode (
    int mode,
    const char* key, const char* text,
    Pico_Rel_Rect rect
);

/// @brief Shows what has been drawn onto the screen.
/// Only does anything on expert mode.
/// @sa pico_set_expert
void pico_output_present (void);

/// @brief Composites the layer hierarchy (window's children, including
/// world) onto window.tex. Called automatically by pico_output_present
/// in non-expert mode.
void pico_output_draw_layers (void);

/// @brief Draws a video frame (all-in-one).
/// Auto-syncs to elapsed time internally.
/// @param path path to the Y4M video file
/// @param rect target position and dimension
/// @return 1 if frame drawn, or 0 at EOF
int pico_output_draw_video (const char* path, Pico_Rel_Rect rect);

/// @brief Takes a screenshot of a layer.
/// @param layer layer name to capture (NULL uses current layer)
/// @param path screenshot filepath (NULL uses timestamp in the name)
/// @param rect region to capture (NULL captures full layer)
/// @return the filepath of the screenshot
const char* pico_output_screenshot (const char* layer, const char* path, const Pico_Rel_Rect* rect);

/// @brief Plays a sound.
/// @param path path to the audio file
void pico_output_sound (const char* path);

/// @}

/// @defgroup State
/// @brief All getters and setters.
/// @{

// GET

/// @brief Gets the entire pencil state of the current layer.
/// @param pencil output struct populated with pencil state
/// @sa pico_set_pencil
Pico_Layer_Pencil pico_get_pencil       (void);
Pico_Color  pico_get_pencil_color (void);
const char* pico_get_pencil_font  (void);
PICO_STYLE  pico_get_pencil_style (void);

/// @brief Gets the state of expert mode.
/// @param fps optional pointer to receive fps value (NULL to ignore)
/// @return 1 if enabled, or 0 otherwise
/// @sa pico_set_expert
int pico_get_expert (int* fps);

/// @brief Gets the dimensions of the given image.
/// @param path image filepath
/// @param dim optional dim with w/h to complete (NULL returns raw dimensions)
/// @return absolute dimensions (missing w or h filled based on aspect ratio)
Pico_Abs_Dim pico_get_image (const char* path, Pico_Rel_Dim* dim);

/// @brief Gets current layer key.
/// @return layer key
/// @sa pico_set_layer
const char* pico_get_layer (void);

/// @brief Gets the entire effect state of the current layer.
/// @param effect output struct populated with effect state
/// @sa pico_set_effect
Pico_Layer_Effect pico_get_effect          (void);
unsigned char pico_get_effect_alpha    (void);
Pico_Color    pico_get_effect_color    (void);
PICO_FLIP     pico_get_effect_flip     (void);
int           pico_get_effect_grid     (void);
Pico_Rot      pico_get_effect_rotate   (void);

/// @brief Gets video properties.
/// @param path path to the Y4M video file
/// @param rect optional rect with w/h to complete (NULL ok)
/// @return video properties (dim, fps, frame, done)
/// @sa pico_set_video
/// @sa pico_get_image
Pico_Video pico_get_video (const char* path, Pico_Rel_Rect* rect);

///////////////////////////////////////////////////////////////////////////////

/// @brief Creates a layer from a pixmap (exclusive mode).
/// @param key layer key (must not be NULL or start with '/')
/// @param dim pixmap dimensions
/// @param pixels RGBA pixel data (must remain valid while layer
///               exists)
void pico_layer_pixmap (const char* up, const char* key,
                        Pico_Abs_Dim dim,
                        const Pico_Color* pixels);

/// @brief Creates a layer from a pixmap.
/// @param mode realm mode ('!' exclusive, '=' shared, '~' replace)
/// @param key layer key (must not be NULL or start with '/')
/// @param dim pixmap dimensions
/// @param pixels RGBA pixel data (must remain valid while layer
///               exists)
void pico_layer_pixmap_mode (int mode, const char* up, const char* key,
                             Pico_Abs_Dim dim,
                             const Pico_Color* pixels);

/// @brief Creates an empty layer (exclusive mode).
/// @param key   layer key (must not be NULL or start with '/')
/// @param clear initial scene.clear value
/// @param dim   layer dimensions; '%' resolves against parent
///              `up`'s scene.dim (or current layer if up==NULL)
void pico_layer_empty (
    const char* up, const char* key, int clear,
    Pico_Rel_Dim dim, Pico_Abs_Dim* tile
);

/// @brief Creates an empty layer.
/// @param mode  realm mode ('!' exclusive, '=' shared, '~' replace)
/// @param key   layer key (must not be NULL or start with '/')
/// @param clear initial scene.clear value
/// @param dim   layer dimensions; '%' resolves against parent
///              `up`'s scene.dim (or current layer if up==NULL)
void pico_layer_empty_mode (
    int mode, const char* up, const char* key, int clear,
    Pico_Rel_Dim dim, Pico_Abs_Dim* tile
);

/// @brief Creates a layer from an image file (exclusive mode).
/// @param key layer key (NULL uses "/image/<path>", otherwise
///            must not start with '/')
/// @param path path to the image file
void pico_layer_image (const char* up, const char* key, const char* path);

/// @brief Creates a layer from an image file.
/// @param mode realm mode ('!' exclusive, '=' shared, '~' replace)
/// @param key layer key (NULL uses "/image/<path>", otherwise
///            must not start with '/')
/// @param path path to the image file
void pico_layer_image_mode (int mode,
    const char* up, const char* key, const char* path);

/// @brief Creates a sub-layer (crop) from an existing layer
///        (exclusive mode).
/// Shares the parent's texture — no copy.
/// @param key layer key (must not be NULL)
/// @param parent parent layer key (must exist, must not be a
///               sub-layer)
/// @param crop source rectangle within the parent
void pico_layer_sub (const char* up, const char* key,
    const char* parent, const Pico_Rel_Rect* crop);

/// @brief Creates a sub-layer (crop) from an existing layer.
/// Shares the parent's texture — no copy.
/// @param mode realm mode ('!' exclusive, '=' shared, '~' replace)
/// @param key layer key (must not be NULL)
/// @param parent parent layer key (must exist, must not be a
///               sub-layer)
/// @param crop source rectangle within the parent
void pico_layer_sub_mode (int mode, const char* up, const char* key,
    const char* parent, const Pico_Rel_Rect* crop);

/// @brief Creates a layer from text (exclusive mode).
/// @param key layer key (must not be NULL or start with '/')
/// @param height text height in pixels
/// @param text the text to render
/// @note Uses current font and draw color
void pico_layer_text (const char* up, const char* key,
    int height, const char* text);

/// @brief Creates a layer from text.
/// @param mode realm mode ('!' exclusive, '=' shared, '~' replace)
/// @param key layer key (must not be NULL or start with '/')
/// @param height text height in pixels
/// @param text the text to render
/// @note Uses current font and draw color
void pico_layer_text_mode (int mode, const char* up, const char* key,
    int height, const char* text);

/// @brief Creates a video layer from a Y4M file (exclusive mode).
/// @param key layer key (NULL uses path, otherwise must not
///            start with '/')
/// @param path path to the Y4M video file
void pico_layer_video (const char* up, const char* key, const char* path);

/// @brief Creates a video layer from a Y4M file.
/// @param mode realm mode ('!' exclusive, '=' shared, '~' replace)
/// @param key layer key (NULL uses path, otherwise must not
///            start with '/')
/// @param path path to the Y4M video file
void pico_layer_video_mode (int mode,
    const char* up, const char* key, const char* path);

///////////////////////////////////////////////////////////////////////////////

/// @brief Gets the keyboard state (current modifier flags).
/// @return keyboard state with modifier flags (key is 0 when polling)
Pico_Keyboard pico_get_keyboard (void);

/// @brief Gets the mouse state.
/// @param layer layer name; NULL = cur
/// @param pos template: `pos->mode` ('!','%','#') and `pos->anchor` are
///            read on input; `pos->x` / `pos->y` are filled on output.
/// @return mouse state (mode + anchor + x/y + buttons) expressed in `layer`'s frame
/// @sa pico_set_mouse
Pico_Mouse pico_get_mouse (const char* layer, Pico_Rel_Pos* pos);

/// @brief Gets the amount of ticks that passed since pico was initialized.
/// @return elapsed time in milliseconds
Uint32 pico_get_now (void);

/// @brief Gets the dimensions of the given text (shared caching).
/// @param text text to measure
/// @param dim dim with h for font size (mode '!' or '%'),
///            w filled in
/// @return absolute dimensions
/// @sa pico_get_text_mode
Pico_Abs_Dim pico_get_text (const char* text, Pico_Rel_Dim* dim);

/// @brief Gets text dimensions with explicit realm mode and
///        layer key.
/// @param mode realm mode ('!' exclusive, '=' shared,
///             '~' replace)
/// @param key layer key
/// @param text text to measure
/// @param dim dim with h for font size (mode '!' or '%'),
///            w filled in
/// @return absolute dimensions
/// @sa pico_get_text
Pico_Abs_Dim pico_get_text_mode (
    int mode,
    const char* key, const char* text,
    Pico_Rel_Dim* dim
);

/// @brief Gets the entire scene state of the current layer.
/// @param scene output struct populated with scene state
/// @sa pico_set_scene
void          pico_get_scene       (Pico_Layer_Scene* scene);
Pico_Rel_Rect pico_get_scene_clip  (void);
Pico_Abs_Dim  pico_get_scene_dim   (void);
Pico_Rel_Rect pico_get_scene_dst   (void);
int           pico_get_scene_clear (void);
Pico_Rel_Rect pico_get_scene_src   (void);
Pico_Abs_Dim  pico_get_scene_tile  (void);

/// @brief Gets the entire window state.
/// @param win output struct populated with window state
/// @sa pico_set_window
Pico_Window  pico_get_window       (void);
int          pico_get_window_fs    (void);
int          pico_get_window_show  (void);
const char*  pico_get_window_title (void);

// SET

/// @brief Sets both window and world to the same dimensions.
/// @param dim dimensions for both window and world
/// @sa pico_set_window
/// @sa pico_set_scene_dim
void pico_set_dim (Pico_Rel_Dim dim);

/// @brief Sets the entire pencil state of the current layer.
/// @param pencil new pencil state
/// @sa pico_get_pencil
void pico_set_pencil       (Pico_Layer_Pencil pencil);
void pico_set_pencil_color (Pico_Color color);
void pico_set_pencil_font  (const char* path);
void pico_set_pencil_style (PICO_STYLE style);

/// @brief Toggles expert mode (manual present, frame pacing).
/// @param on 1 to enable, 0 to disable
/// @param fps frame rate hint (-1 = unlimited, 0 = block on event, N>0 = fixed FPS)
/// @return frame period in ms: -1 = block forever, 0 = immediate, N>0 = frame period
/// @sa pico_get_expert
int pico_set_expert (int on, int fps);

/// @brief Switches the current render-target layer.
/// @param key layer key
/// @return previous layer name
/// @sa pico_get_layer
const char* pico_set_layer (const char* key);

/// @brief Warps the mouse cursor to the given relative position.
/// Symmetric to @ref pico_get_mouse: a Pos returned by `pico_get_mouse`
/// (any mode) round-trips through `pico_set_mouse`.
/// @param layer layer name; NULL = cur. `pos` is interpreted in `layer`'s frame.
/// @param pos target position; `pos->mode` is one of '!' pixels,
///        '%' percentage, '#' tiles.
/// @sa pico_get_mouse
void pico_set_mouse (const char* layer, Pico_Rel_Pos pos);

/// @brief Sets the entire effect state of the current layer.
/// @param effect new effect state
/// @sa pico_get_effect
void pico_set_effect          (Pico_Layer_Effect effect);
void pico_set_effect_alpha    (unsigned char alpha);
void pico_set_effect_color    (Pico_Color color);
void pico_set_effect_flip     (PICO_FLIP flip);
void pico_set_effect_grid     (int on);
void pico_set_effect_rotate   (Pico_Rot rotate);

/// @brief Syncs a video layer to a target frame.
/// Supports forward and backward seeking.
/// @param key layer key (must exist as video layer)
/// @param frame target frame number
/// @return 1 if frame is valid, or 0 past EOF
/// @sa pico_get_video
int pico_set_video (const char* key, int frame);

/// @brief Sets the entire scene state of the current layer.
/// @param scene new scene state
/// @sa pico_get_scene
void pico_set_scene       (Pico_Layer_Scene scene);
void pico_set_scene_clip  (Pico_Rel_Rect clip);
void pico_set_scene_dim   (Pico_Rel_Dim dim);
void pico_set_scene_dst   (Pico_Rel_Rect dst);
void pico_set_scene_clear (int on);
void pico_set_scene_src   (Pico_Rel_Rect src);
void pico_set_scene_tile  (Pico_Abs_Dim tile);

/// @brief Sets the entire window state.
/// @param win new window state
/// @sa pico_get_window
void pico_set_window       (Pico_Window win);
void pico_set_window_fs    (int fs);
void pico_set_window_show  (int on);
void pico_set_window_title (const char* title);

/// @}

/// @defgroup Utils
/// @brief Utilities for users
/// @{

/// @brief Projects a dimension from `L_fr`'s frame into `L_to`'s frame.
/// Either layer name may be NULL (== cur). Each layer must be cur, an
/// ancestor, or a descendant of cur via `hier.up`. Siblings (neither
/// is ancestor of the other) project via cur in two steps.
/// @param L_to target layer name; NULL = cur
/// @param to result template (mode set on input); written in L_to's frame
/// @param L_fr source layer name; NULL = cur
/// @param fr dimension in L_fr's frame
void pico_cv_dim (
    const char* L_to, Pico_Rel_Dim* to,
    const char* L_fr, const Pico_Rel_Dim* fr
);

/// @brief Projects a position from `L_fr`'s frame into `L_to`'s frame.
/// Either layer name may be NULL (== cur). Siblings project via cur.
/// @param L_to target layer name; NULL = cur
/// @param to result template (mode/anchor set on input); written in L_to
/// @param L_fr source layer name; NULL = cur
/// @param fr position in L_fr's frame
void pico_cv_pos (
    const char* L_to, Pico_Rel_Pos* to,
    const char* L_fr, const Pico_Rel_Pos* fr
);

/// @brief Projects a rectangle from `L_fr`'s frame into `L_to`'s frame.
/// Either layer name may be NULL (== cur). Siblings project via cur.
/// @param L_to target layer name; NULL = cur
/// @param to result template (mode/anchor set on input); written in L_to
/// @param L_fr source layer name; NULL = cur
/// @param fr rectangle in L_fr's frame
void pico_cv_rect (
    const char* L_to, Pico_Rel_Rect* to,
    const char* L_fr, const Pico_Rel_Rect* fr
);

/// @brief Converts `in` into `out`'s mode/anchor (same current layer).
/// `out->mode` and `out->anchor` are honored; numeric fields are
/// rewritten to express the same absolute value in `out`'s mode.
void pico_cv_mode_dim  (Pico_Rel_Dim  in, Pico_Rel_Dim*  out);
void pico_cv_mode_pos  (Pico_Rel_Pos  in, Pico_Rel_Pos*  out);
void pico_cv_mode_rect (Pico_Rel_Rect in, Pico_Rel_Rect* out);

/// @brief Composes a child rect onto a parent rect.
/// Returns a flat rect (no chain) that resolves to the same absolute
/// coords as `in` interpreted relative to `out`. Mode and anchor of
/// `in` are preserved; numeric x/y/w/h are re-expressed within the
/// current scene.
/// @param out parent rectangle (relative to current scene)
/// @param in child rectangle (relative to `out`)
/// @return flat rectangle, no parent reference needed
Pico_Rel_Rect pico_in_rect (Pico_Rel_Rect out, Pico_Rel_Rect in);

/// @brief Composes a child position onto a parent rect.
/// Mode and anchor of `in` are preserved.
/// @param out parent rectangle (relative to current scene)
/// @param in child position (relative to `out`)
/// @return flat position, no parent reference needed
Pico_Rel_Pos pico_in_pos (Pico_Rel_Rect out, Pico_Rel_Pos in);

/// @brief Composes a child dimension onto a parent rect.
/// Mode of `in` is preserved.
/// @param out parent rectangle (relative to current scene)
/// @param in child dimension (relative to `out`)
/// @return flat dimension, no parent reference needed
Pico_Rel_Dim pico_in_dim (Pico_Rel_Rect out, Pico_Rel_Dim in);

/// @brief Checks if two points fall on the same pixel.
/// Each side can be in cur or in a descendant of cur. Both points
/// must be non-NULL. Comparison is on rounded integer pixel values.
/// @return 1 if rounded (x,y) of p1 equals that of p2, or 0 otherwise
/// @sa pico_vs_pos_rect
int pico_vs_pos_pos (
    const char* L1, Pico_Rel_Pos* p1,
    const char* L2, Pico_Rel_Pos* p2
);

/// @brief Checks if a point is inside a rectangle.
/// Each side can be expressed in cur (Lx=NULL) or in a descendant of
/// cur (Lx=layer name). With L2 set and r2=NULL, the named layer's
/// bounds (scene.dst in its parent) act as the rect. p1 must not be
/// NULL.
/// @return 1 if p1 is inside r2, or 0 otherwise
/// @sa pico_vs_pos_pos pico_vs_rect_rect
int pico_vs_pos_rect (
    const char* L1, Pico_Rel_Pos*  p1,
    const char* L2, Pico_Rel_Rect* r2
);

/// @brief Checks if a point is inside a rectangle (mirror of pos_rect).
/// Equivalent to pico_vs_pos_rect(L2, p2, L1, r1). r1 may be NULL with
/// L1 set (uses the named layer's bounds) or both NULL (uses cur's
/// bounds). p2 must not be NULL.
/// @return 1 if p2 is inside r1, or 0 otherwise
/// @sa pico_vs_pos_rect
int pico_vs_rect_pos (
    const char* L1, Pico_Rel_Rect* r1,
    const char* L2, Pico_Rel_Pos*  p2
);

/// @brief Checks if two rectangles overlap.
/// Each side can be expressed in cur (Lx=NULL) or in a descendant of
/// cur. With Lx set and the rect NULL, the named layer's bounds act
/// as the rect for that side.
/// @return 1 if r1 and r2 overlap, or 0 otherwise
/// @sa pico_vs_pos_rect
int pico_vs_rect_rect (
    const char* L1, Pico_Rel_Rect* r1,
    const char* L2, Pico_Rel_Rect* r2
);

/// @brief Makes a color darker by the specified percentage.
/// @param clr the original color
/// @param pct percentage to darken (0.0-1.0); negative values lighten
/// @return the darkened color
/// @sa pico_color_lighter
Pico_Color pico_color_darker (Pico_Color clr, float pct);

/// @brief Makes a color lighter by the specified percentage.
/// @param clr the original color
/// @param pct percentage to lighten (0.0-1.0); negative values darken
/// @return the lightened color
/// @sa pico_color_darker
Pico_Color pico_color_lighter (Pico_Color clr, float pct);

/// @brief Mixes two colors by averaging their RGB components.
/// @param c1 first color
/// @param c2 second color
/// @return the mixed color
/// @sa pico_color_darker
/// @sa pico_color_lighter
Pico_Color pico_color_mix (Pico_Color c1, Pico_Color c2);

/// @brief Returns a copy of the color with alpha set to the given value.
/// @param clr the original color
/// @param a alpha value (0-255)
/// @return copy of clr with .a = a
Pico_Color pico_color_alpha (Pico_Color clr, Uint8 a);

/// @brief Converts a hex integer to a color.
/// @param hex 0xRRGGBB (24-bit, alpha=0xFF) or 0xRRGGBBAA (32-bit)
/// @return the corresponding color
/// @sa pico_color_darker
/// @sa pico_color_lighter
Pico_Color pico_color_hex (uint32_t hex);

/// @}

#ifdef __cplusplus
}
#endif

#endif // PICO_H

