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
#define PICO_HASH_TTL  1000

/// @brief Asserts condition and shows SDL error on failure.
/// @param x condition to assert
#define pico_assert(x)  \
    if (!(x)) {         \
        fprintf(stderr,"%s\n",SDL_GetError()); assert(0 && "SDL ERROR"); \
    }

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
    struct Pico_Rel_Rect* up;
} Pico_Rel_Rect;

typedef struct {
    char mode;
    struct {
        float w, h;
    };
    struct Pico_Rel_Rect* up;
} Pico_Rel_Dim;

typedef struct {
    char mode;
    struct {
        float x, y;
    };
    Pico_Anchor anchor;
    struct Pico_Rel_Rect* up;
} Pico_Rel_Pos;

typedef enum {
    PICO_STYLE_FILL, PICO_STYLE_STROKE
} PICO_STYLE;

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
/// @include delay.c
/// @param ms milliseconds to wait
void pico_input_delay (int ms);

/// @brief Stops the program until an event occurs.
/// @include event.c
/// @param evt where to save the event data, or NULL to ignore
/// @param type type of event to wait for (Pico_EventType)
/// @sa pico_input_event_ask
/// @sa pico_input_event_timeout
void pico_input_event (Pico_Event* evt, int type);

/// @brief Checks if an event has occured.
/// @param evt where to save the event data, or NULL to ignore
/// @param type type of event to check the occurence (Pico_EventType)
/// @return 1 if the given type of event has occurred, or 0 otherwise
/// @sa pico_input_event
/// @sa pico_input_event_timeout
int  pico_input_event_ask (Pico_Event* evt, int type);

/// @brief Stops the program until an event occurs or a timeout is reached.
/// @include event_timeout.c
/// @param evt where to save the event data, or NULL to ignore
/// @param type type of event to wait for (Pico_EventType)
/// @param timeout time limit to wait for (milliseconds)
/// @return 1 if the given type of event has occurred, or 0 otherwise
/// @sa pico_input_event
/// @sa pico_input_event_ask
int  pico_input_event_timeout (Pico_Event* evt, int type, int timeout);

/// @}

/// @defgroup Output
/// @brief Draw primitives, play sounds, etc.
/// @{

/// @brief Clears screen with color set by @ref pico_set_color_clear.
void pico_output_clear (void);

/// @brief Draws an RGBA image buffer.
/// @param name layer name for caching (required)
/// @param dim image dimensions in pixels
/// @param buffer the RGBA image data
/// @param rect drawing rectangle (mode determines coordinate interpretation)
/// @sa pico_output_draw_image
void pico_output_draw_buffer (const char* name, Pico_Abs_Dim dim,
                              const Pico_Color_A buffer[],
                              const Pico_Rel_Rect* rect);

/// @brief Draws an image.
/// @param path path to the image file
/// @param rect target position and dimension (mode determines coordinates)
/// @sa pico_output_draw_buffer
void pico_output_draw_image (const char* path, Pico_Rel_Rect* rect);

/// @brief Draws a line.
/// @param p1 first endpoint position (mode determines coordinates)
/// @param p2 second endpoint position (mode determines coordinates)
void pico_output_draw_line (Pico_Rel_Pos* p1, Pico_Rel_Pos* p2);

/// @brief Draws a single pixel.
/// @param pos drawing position (mode determines coordinates)
void pico_output_draw_pixel (Pico_Rel_Pos* pos);

/// @brief Draws a batch of pixels.
/// @param n number of positions
/// @param ps array of positions (mode determines coordinates)
void pico_output_draw_pixels (int n, const Pico_Rel_Pos* ps);

/// @brief Draws a layer onto the current layer.
/// @param name layer name (must exist)
/// @param rect target position and dimension (mode determines coordinates)
void pico_output_draw_layer (const char* name, Pico_Rel_Rect* rect);

/// @brief Draws a rectangle.
/// @param rect rectangle to draw (mode determines coordinates)
void pico_output_draw_rect (Pico_Rel_Rect* rect);

/// @brief Draws a triangle.
/// @param p1 first vertex position (mode determines coordinates)
/// @param p2 second vertex position (mode determines coordinates)
/// @param p3 third vertex position (mode determines coordinates)
void pico_output_draw_tri (Pico_Rel_Pos* p1, Pico_Rel_Pos* p2, Pico_Rel_Pos* p3);

/// @brief Draws an ellipse.
/// @param rect bounding rectangle (mode determines coordinates)
void pico_output_draw_oval (Pico_Rel_Rect* rect);

/// @brief Draws a polygon.
/// @param n number of vertices
/// @param ps array of vertex positions (mode determines coordinates)
void pico_output_draw_poly (int n, const Pico_Rel_Pos* ps);

/// @brief Draws text.
/// @param text text to draw
/// @param rect drawing rectangle (mode determines coordinates)
void pico_output_draw_text (const char* text, Pico_Rel_Rect* rect);

/// @brief Shows what has been drawn onto the screen.
/// Only does anything on expert mode.
/// @sa pico_set_expert
void pico_output_present (void);

/// @brief Takes a screenshot.
/// @param path screenshot filepath (NULL uses timestamp in the name)
/// @param r region to capture (NULL captures full screen)
/// @return the filepath of the screenshot
const char* pico_output_screenshot (const char* path, const Pico_Rel_Rect* r);

/// @brief Plays a sound.
/// @param path path to the audio file
void pico_output_sound (const char* path);

/// @}

/// @defgroup State
/// @brief All getters and setters.
/// @{

// GET

/// @brief Gets the color set to clear the screen.
/// @return the current clear color
Pico_Color pico_get_color_clear (void);

/// @brief Gets the color set to draw.
/// @return the current draw color
Pico_Color pico_get_color_draw (void);

/// @brief Gets the cropping applied to objects when drawing them.
/// @return the current crop rectangle
Pico_Abs_Rect pico_get_crop (void);

/// @brief Gets the state of expert mode.
/// @return 1 if enabled, or 0 otherwise
int pico_get_expert (void);

/// @brief Gets the font used to draw texts.
/// @return path to the current font file
const char* pico_get_font (void);

/// @brief Gets the dimensions of the given image.
/// @param path image filepath
/// @param dim optional dim with w/h to complete (NULL returns raw dimensions)
/// @return absolute dimensions (missing w or h filled based on aspect ratio)
Pico_Abs_Dim pico_get_image (const char* path, Pico_Rel_Dim* dim);

/// @brief Gets the state of a key.
/// @param key key constant
/// @return 1 if key is pressed, or 0 otherwise
int pico_get_key (PICO_KEY key);

/// @brief Gets current layer name.
/// @return layer name (NULL = main layer)
const char* pico_get_layer (void);

/// @brief Creates an empty layer.
/// @param name layer name (must not be NULL or start with '/')
/// @param dim layer dimensions
/// @return the layer name
const char* pico_layer_empty (const char* name, Pico_Abs_Dim dim);

/// @brief Creates a layer from an image file.
/// @param name layer name (NULL uses "/image/<path>", otherwise must not
///             start with '/')
/// @param path path to the image file
/// @return the layer name
const char* pico_layer_image (const char* name, const char* path);

/// @brief Creates a layer from a pixel buffer.
/// @param name layer name (NULL uses "/buffer/<pointer>", otherwise must not
///             start with '/')
/// @param dim buffer dimensions
/// @param pixels RGBA pixel data (must remain valid while layer exists)
/// @return the layer name
const char* pico_layer_buffer (const char* name, Pico_Abs_Dim dim,
                               const Pico_Color_A* pixels);

/// @brief Creates a layer from text.
/// @param name layer name (NULL auto-generates from font/height/color/text,
///             otherwise must not start with '/')
/// @param height text height in pixels
/// @param text the text to render
/// @return the layer name
/// @note Uses current font (pico_set_font) and draw color (pico_set_color_draw)
const char* pico_layer_text (const char* name, int height, const char* text);

/// @brief Gets the mouse state.
/// @param pos where to save the mouse position (mode determines coordinate
///            system: '!' for pixels, '%' for percentage, '#' for tiles)
/// @param button mouse button to check (1=left, 2=middle, 3=right), or 0 for any
/// @return 1 if the specified button is pressed, or 0 otherwise
int pico_get_mouse (Pico_Rel_Pos* pos, int button);

/// @brief Gets the visibility state of the window.
/// @return 1 if visible, or 0 otherwise
int pico_get_show (void);

/// @brief Gets the drawing style.
/// @return PICO_STYLE_FILL or PICO_STYLE_STROKE
PICO_STYLE pico_get_style (void);

/// @brief Gets the dimensions of the given text.
/// @param text text to measure
/// @param dim dim with h for font size (mode '!' or '%'), w filled in
/// @return absolute dimensions
Pico_Abs_Dim pico_get_text (const char* text, Pico_Rel_Dim* dim);

/// @brief Gets the amount of ticks that passed since pico was initialized.
/// @return elapsed time in milliseconds
Uint32 pico_get_ticks (void);

/// @brief Gets the current view configuration. NULL arguments are ignored.
/// @param grid pointer to retrieve grid state
/// @param dst pointer to retrieve window target region
/// @param dim pointer to retrieve world/logical dimensions
/// @param src pointer to retrieve world source region
/// @param clip pointer to retrieve world clipping region
/// @param tile pointer to retrieve tile dimensions in pixels
/// @sa pico_set_view
void pico_get_view (
    int* grid,
    Pico_Abs_Dim* dim,
    Pico_Rel_Rect* target,
    Pico_Rel_Rect* source,
    Pico_Rel_Rect* clip,
    Pico_Abs_Dim* tile
);

/// @brief Gets window properties. NULL arguments are ignored.
/// @param title pointer to retrieve window title
/// @param fs pointer to retrieve fullscreen state
/// @param dim pointer to retrieve window dimensions
void pico_get_window (const char** title, int* fs, Pico_Abs_Dim* dim);

// SET

/// @brief Sets the alpha transparency for drawing operations.
/// @param a alpha value (0: transparent; 255: opaque)
void pico_set_alpha (int a);

/// @brief Changes the color used to clear the screen.
/// @param color new color
void pico_set_color_clear (Pico_Color color);

/// @brief Changes the color used to draw objects.
/// @param color new color
void pico_set_color_draw (Pico_Color color);

/// @brief Changes the cropping that is applied to images, texts and buffers
///        before drawing them.
/// @param crop cropping region ({0,0,0,0} to disable)
void pico_set_crop (Pico_Abs_Rect crop);

/// @brief Toggles the expert mode.
/// @param on 1 to enable it, or 0 to disable it
void pico_set_expert (int on);

/// @brief Changes the font used to draw texts.
/// @param path path to font file
void pico_set_font (const char* path);

/// @brief Switches to a layer.
/// @param name layer name (NULL = main layer, must exist)
void pico_set_layer (const char* name);

/// @brief Toggles the application window visibility.
/// @param on 1 to show, or 0 to hide
void pico_set_show (int on);

/// @brief Sets the drawing style.
/// @param style new style
void pico_set_style (PICO_STYLE style);

/// @brief Sets the view configuration. NULL arguments are ignored.
/// @param grid 1 to show grid, 0 to hide, or -1 to keep unchanged
/// @param dim world/logical dimensions (mode '#' = tiles, otherwise pixels)
/// @param target target region within window
/// @param source source region within world
/// @param clip clipping region within world
/// @param tile tile size in pixels (required when dim mode is '#')
/// @sa pico_get_view
void pico_set_view (
    int grid,
    Pico_Rel_Dim*  dim,
    Pico_Rel_Rect* target,
    Pico_Rel_Rect* source,
    Pico_Rel_Rect* clip,
    Pico_Abs_Dim*  tile
);

/// @brief Sets window properties. NULL/(-1) arguments are ignored.
/// @param title window title (NULL to keep current)
/// @param fs fullscreen: 1=enable, 0=disable, -1=unchanged
/// @param dim window dimensions (NULL to keep current)
void pico_set_window (const char* title, int fs, Pico_Rel_Dim* dim);

/// @brief Sets both window and world to the same dimensions.
/// @param dim dimensions for both window and world
void pico_set_dim (Pico_Rel_Dim* dim);

// PUSH / POP

/// @brief Saves the current drawing state onto a stack.
/// Saves: alpha, angle, colors, crop, font, style, layer.
/// @sa pico_pop
void pico_push (void);

/// @brief Restores the drawing state from the stack.
/// @sa pico_push
void pico_pop (void);

/// @}

/// @defgroup Utils
/// @brief Utilities for users
/// @{

/// @brief Converts a relative dimension to absolute coordinates.
/// @param dim relative dimension to convert
/// @param base reference rectangle (NULL uses world dimensions)
/// @return absolute dimension in logical pixels
/// @sa pico_cv_rect_rel_abs
Pico_Abs_Dim pico_cv_dim_rel_abs (Pico_Rel_Dim* dim, Pico_Abs_Rect* base);

/// @brief Converts a relative position to absolute coordinates.
/// @param pos relative position to convert
/// @param base reference rectangle to use as basis (NULL uses world dimensions)
/// @return absolute position in logical pixels
/// @sa pico_cv_rect_rel_abs
Pico_Abs_Pos pico_cv_pos_rel_abs (const Pico_Rel_Pos* pos, Pico_Abs_Rect* base);

/// @brief Converts a relative rectangle to absolute coordinates.
/// @param rect relative rectangle to convert
/// @param base reference rectangle to use as basis (NULL uses world dimensions)
/// @return absolute rectangle in logical pixels
/// @sa pico_cv_pos_rel_abs
Pico_Abs_Rect pico_cv_rect_rel_abs (const Pico_Rel_Rect* rect, Pico_Abs_Rect* base);

/// @brief Converts an absolute position to relative coordinates.
/// @param fr absolute position to convert
/// @param to relative position template (mode, anchor, up must be set)
/// @param base reference rectangle (NULL uses world dimensions)
/// @sa pico_cv_pos_rel_abs
void pico_cv_pos_abs_rel (const Pico_Abs_Pos* fr,
                          Pico_Rel_Pos* to,
                          Pico_Abs_Rect* base);

/// @brief Converts a relative position to another relative mode.
/// @param fr relative position to convert
/// @param to relative position template (mode, anchor, up must be set)
/// @param base reference rectangle (NULL uses world dimensions)
/// @sa pico_cv_pos_abs_rel
void pico_cv_pos_rel_rel (const Pico_Rel_Pos* fr,
                          Pico_Rel_Pos* to,
                          Pico_Abs_Rect* base);

/// @brief Converts an absolute rectangle to relative coordinates.
/// @param fr absolute rectangle to convert
/// @param to relative rectangle template (mode, anchor, up must be set)
/// @param base reference rectangle (NULL uses world dimensions)
/// @sa pico_cv_rect_rel_abs
void pico_cv_rect_abs_rel (const Pico_Abs_Rect* fr,
                           Pico_Rel_Rect* to,
                           Pico_Abs_Rect* base);

/// @brief Converts a relative rectangle to another relative mode.
/// @param fr relative rectangle to convert
/// @param to relative rectangle template (mode, anchor, up must be set)
/// @param base reference rectangle (NULL uses world dimensions)
/// @sa pico_cv_rect_abs_rel
void pico_cv_rect_rel_rel (const Pico_Rel_Rect* fr,
                           Pico_Rel_Rect* to,
                           Pico_Abs_Rect* base);

/// @brief Checks if a point is inside a rectangle.
/// @param pos point to test (mode determines coordinates)
/// @param rect rectangle to test against (mode determines coordinates)
/// @return 1 if pos is inside rect, or 0 otherwise
/// @sa pico_vs_rect_rect
int pico_vs_pos_rect (Pico_Rel_Pos* pos, Pico_Rel_Rect* rect);

/// @brief Checks if two rectangles overlap.
/// @param r1 first rectangle (mode determines coordinates)
/// @param r2 second rectangle (mode determines coordinates)
/// @return 1 if r1 and r2 overlap, or 0 otherwise
/// @sa pico_vs_pos_rect
int pico_vs_rect_rect (Pico_Rel_Rect* r1, Pico_Rel_Rect* r2);

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

/// @}

#ifdef __cplusplus
}
#endif

#endif // PICO_H

