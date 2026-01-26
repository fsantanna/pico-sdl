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
// '!': raw
// '%': pct
// '#': tile (TODO)
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

/// @brief Draws an RGBA image buffer using absolute coordinates.
/// @param dim image dimensions
/// @param buffer the RGBA image data
/// @param rect drawing rectangle in logical pixels
/// @sa pico_output_draw_buffer
/// @sa pico_output_draw_image
void pico_output_draw_buffer (Pico_Abs_Dim dim, const Pico_Color_A buffer[], const Pico_Rel_Rect* rect);

/// @brief Draws an image using absolute coordinates.
/// @param path path to the image file
/// @param rect image target position and dimension
/// @sa pico_output_draw_buffer
void pico_output_draw_image (const char* path, Pico_Rel_Rect* rect);

/// @brief Draws a line using absolute coordinates.
/// @param p1 first endpoint position
/// @param p2 second endpoint position
void pico_output_draw_line (Pico_Rel_Pos* p1, Pico_Rel_Pos* p2);

/// @brief Draws a single pixel using absolute coordinates.
/// @param pos drawing position
void pico_output_draw_pixel (Pico_Rel_Pos* pos);

/// @brief Draws a batch of pixels using absolute coordinates.
/// @param n number of coordinates
/// @param ps array of coordinates
void pico_output_draw_pixels (int n, const Pico_Rel_Pos* ps);

/// @brief Draws a rectangle using absolute coordinates.
/// @param rect rectangle to draw
void pico_output_draw_rect (Pico_Rel_Rect* rect);

/// @brief Draws a triangle using absolute coordinates.
/// @param p1 first vertex position
/// @param p2 second vertex position
/// @param p3 third vertex position
void pico_output_draw_tri (Pico_Rel_Pos* p1, Pico_Rel_Pos* p2, Pico_Rel_Pos* p3);

/// @brief Draws an ellipse using absolute coordinates.
/// @param rect bounding rectangle
void pico_output_draw_oval (Pico_Rel_Rect* rect);

/// @brief Draws a polygon using absolute coordinates.
/// @param n number of vertices
/// @param ps array of vertex positions
void pico_output_draw_poly (int n, const Pico_Rel_Pos* ps);

/// @brief Draws text using absolute coordinates.
/// @param text text to draw
/// @param rect drawing rectangle
void pico_output_draw_text (const char* text, Pico_Rel_Rect* rect);

/// @brief Shows what has been drawn onto the screen.
/// Only does anything on expert mode.
/// @sa pico_set_expert
void pico_output_present (void);

/// @brief Takes a screenshot of the full physical screen.
/// @param path screenshot filepath (NULL uses timestamp in the name)
/// @return The filepath of the screenshot.
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
Pico_Color pico_get_color_clear (void);

/// @brief Gets the color set to draw.
Pico_Color pico_get_color_draw (void);

/// @brief Gets the cropping applied to objects when drawing them.
Pico_Abs_Rect pico_get_crop (void);

/// @brief Gets the state of expert mode.
/// @return 1 if enabled, or 0 otherwise
int pico_get_expert (void);

/// @brief Gets the font used to draw texts.
const char* pico_get_font (void);

/// @brief Gets the state of fullscreen mode.
/// @return 1 if enabled, or 0 otherwise
int pico_get_fullscreen (void);

/// @brief Gets the dimensions of the given image.
/// @param path image filepath
/// @param dim optional dim with w/h to complete (NULL returns raw dimensions)
/// @return absolute dimensions (missing w or h filled based on aspect ratio)
Pico_Abs_Dim pico_get_image (const char* path, Pico_Rel_Dim* dim);

/// @brief Gets the state of a key.
/// @param key key constant
/// @return 1 if key is pressed, or 0 otherwise
int pico_get_key (PICO_KEY key);

int pico_get_mouse (Pico_Rel_Pos* pos, int button);

/// @brief Gets the visibility state of the window.
int pico_get_show (void);

/// @brief Gets the drawing style.
PICO_STYLE pico_get_style (void);

/// @brief Gets the dimensions of the given text.
/// @param text text to measure
/// @param dim dim with h for font size (mode '!' or '%'), w filled in
/// @return absolute dimensions
Pico_Abs_Dim pico_get_text (const char* text, Pico_Rel_Dim* dim);

/// @brief Gets the amount of ticks that passed since pico was initialized.
Uint32 pico_get_ticks (void);

/// @brief Gets the aplication title.
const char* pico_get_title (void);

/// @brief Gets the current view configuration. NULL arguments are ignored.
/// @param grid pointer to retrieve grid state
/// @param window_fullscreen pointer to retrieve fullscreen state
/// @param window pointer to retrieve window dimensions
/// @param window_target pointer to retrieve window target region
/// @param world pointer to retrieve world/logical dimensions
/// @param world_source pointer to retrieve world source region
/// @param world_clip pointer to retrieve world clipping region
/// @sa pico_set_view
void pico_get_view (
    int* grid,
    int* window_fullscreen,
    Pico_Abs_Dim* window,
    Pico_Rel_Rect* window_target,
    Pico_Abs_Dim* world,
    Pico_Rel_Rect* world_source,
    Pico_Rel_Rect* world_clip
);

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
/// @param path path to font path
void pico_set_font (const char* path);

/// @brief Toggles the aplication window visibility.
/// @param on 1 to show, or 0 to hide
void pico_set_show (int on);

/// @brief Sets the drawing style.
/// @param style new style
void pico_set_style (PICO_STYLE style);

/// @brief Sets the aplication title.
/// @param title new title
void pico_set_title (const char* title);

/// @brief Sets the view configuration using absolute dimensions. NULL arguments are ignored.
/// @param grid 1 to show grid, 0 to hide, or -1 to keep unchanged
/// @param window_fullscreen 1 to enable fullscreen, 0 to disable, or -1 to keep unchanged
/// @param window window dimensions in pixels
/// @param window_target target region within window
/// @param world world/logical dimensions
/// @param world_source source region within world
/// @param world_clip clipping region within world
/// @sa pico_get_view
void pico_set_view (
    int window_grid,
    int window_fullscreen,
    Pico_Rel_Dim*  window,
    Pico_Rel_Rect* window_target,
    Pico_Rel_Dim*  world,
    Pico_Rel_Rect* world_source,
    Pico_Rel_Rect* world_clip
);

/// @}

/// @defgroup Utils
/// @brief Utilities for users
/// @{

/// @brief Converts a percentage-based position to absolute coordinates relative to a reference rectangle.
/// @param p percentage-based position (0.0-1.0)
/// @param ref reference rectangle to use as basis
/// @return absolute position in logical pixels
/// @sa pico_cv_rect_rel_ab
Pico_Abs_Pos pico_cv_pos_rel_abs (const Pico_Rel_Pos* pos, Pico_Abs_Rect* ref);

/// @brief Converts a percentage-based rectangle to absolute coordinates relative to a reference rectangle.
/// @param r percentage-based rectangle (0.0-1.0)
/// @param ref reference rectangle to use as basis
/// @return absolute rectangle in logical pixels
/// @sa pico_cv_pos_rel_abs
Pico_Abs_Rect pico_cv_rect_rel_abs (const Pico_Rel_Rect* rect, Pico_Abs_Rect* ref);

/// @brief Checks if a point is inside a rectangle using absolute coordinates.
/// Assumes that both primitives use the same anchor.
/// @param pos point in logical pixels
/// @param rect rectangle in logical pixels
/// @return 1 if pos is inside rect, or 0 otherwise
/// @sa pico_vs_rect_rect
int pico_vs_pos_rect (Pico_Rel_Pos* pos, Pico_Rel_Rect* rect);

/// @brief Checks if two rectangles overlap using absolute coordinates.
/// Assumes that both rectangles use the same anchor.
/// @param r1 first rectangle in logical pixels
/// @param r2 second rectangle in logical pixels
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

/// @}

#ifdef __cplusplus
}
#endif

#endif // PICO_H

