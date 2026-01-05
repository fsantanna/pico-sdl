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
#define PICO_DIM_PHY ((Pico_Dim) {500,500})
#define PICO_DIM_LOG ((Pico_Dim) {100,100})
#define PICO_HASH  128

typedef SDL_Point Pico_Pos;
typedef SDL_Rect  Pico_Rect;
typedef SDL_Point Pico_Anchor;
typedef SDL_Point Pico_Flip;

#define PICO_LEFT   0
#define PICO_CENTER 50
#define PICO_RIGHT  100
#define PICO_TOP    0
#define PICO_MIDDLE 50
#define PICO_BOTTOM 100

typedef enum {
    PICO_FILL, PICO_STROKE
} PICO_STYLE;

typedef enum {
    PICO_MOUSE_BUTTON_NONE   = 0,
    PICO_MOUSE_BUTTON_LEFT   = SDL_BUTTON_LEFT,
    PICO_MOUSE_BUTTON_MIDDLE = SDL_BUTTON_MIDDLE,
    PICO_MOUSE_BUTTON_RIGHT  = SDL_BUTTON_RIGHT
} PICO_MOUSE_BUTTON;

#define PICO_ANCHOR_LEFT   0
#define PICO_ANCHOR_CENTER 0.5
#define PICO_ANCHOR_RIGHT  1
#define PICO_ANCHOR_TOP    0
#define PICO_ANCHOR_MIDDLE 0.5
#define PICO_ANCHOR_BOTTOM 1

#define PICO_ANCHOR_C  \
    ((Pico_Pct) { PICO_ANCHOR_CENTER, PICO_ANCHOR_MIDDLE })
#define PICO_ANCHOR_NW \
    ((Pico_Pct) { PICO_ANCHOR_LEFT,   PICO_ANCHOR_TOP    })
#define PICO_ANCHOR_N  \
    ((Pico_Pct) { PICO_ANCHOR_CENTER, PICO_ANCHOR_TOP    })
#define PICO_ANCHOR_NE \
    ((Pico_Pct) { PICO_ANCHOR_RIGHT,  PICO_ANCHOR_TOP    })
#define PICO_ANCHOR_E  \
    ((Pico_Pct) { PICO_ANCHOR_RIGHT,  PICO_ANCHOR_MIDDLE })
#define PICO_ANCHOR_SE \
    ((Pico_Pct) { PICO_ANCHOR_RIGHT,  PICO_ANCHOR_BOTTOM })
#define PICO_ANCHOR_S  \
    ((Pico_Pct) { PICO_ANCHOR_CENTER, PICO_ANCHOR_BOTTOM })
#define PICO_ANCHOR_SW \
    ((Pico_Pct) { PICO_ANCHOR_LEFT,   PICO_ANCHOR_BOTTOM })
#define PICO_ANCHOR_W  \
    ((Pico_Pct) { PICO_ANCHOR_LEFT,   PICO_ANCHOR_MIDDLE })

typedef struct {
    Uint8 r;
    Uint8 g;
    Uint8 b;
} Pico_Color;

typedef struct {
    int w;
    int h;
} Pico_Dim;

typedef struct {
    float x;
    float y;
} Pico_Pct;

typedef struct Pico_Rect_Pct {
    float x, y;
    float w, h;
    Pico_Pct anchor;
    struct Pico_Rect_Pct* up;
} Pico_Rect_Pct;

typedef struct {
    float x, y;
    Pico_Pct anchor;
    struct Pico_Rect_Pct* up;
} Pico_Pos_Pct;

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
/// @param rect drawing rectangle in logical pixels
/// @param buffer the RGBA image data
/// @param dim image dimensions
/// @sa pico_output_draw_buffer_pct
/// @sa pico_output_draw_image_raw
void pico_output_draw_buffer_raw (const Pico_Rect rect, const Pico_Color buffer[], Pico_Dim dim);

/// @brief Draws an RGB image buffer using percentage-based coordinates.
/// @param rect image target position and dimensions
/// @param buffer the RGB image data
/// @param dim image source dimensions
/// @sa pico_output_draw_buffer_raw
/// @sa pico_output_draw_image_pct
void pico_output_draw_buffer_pct (const Pico_Rect_Pct* rect, const Pico_Color buffer[], Pico_Dim dim);

/// @brief Draws an image using absolute coordinates.
/// @param rect image target position and dimension
/// @param path path to the image file
/// @sa pico_output_draw_image_pct
/// @sa pico_output_draw_buffer_raw
void pico_output_draw_image_raw (Pico_Rect rect, const char* path);

/// @brief Draws an image using percentage-based coordinates.
/// @param rect image target position and dimension
/// @param path path to the image file
/// @sa pico_output_draw_image_raw
/// @sa pico_output_draw_buffer_pct
void pico_output_draw_image_pct (const Pico_Rect_Pct* rect, const char* path);

/// @brief Draws a line using absolute coordinates.
/// @param p1 first endpoint position
/// @param p2 second endpoint position
/// @sa pico_output_draw_line_pct
void pico_output_draw_line_raw (Pico_Pos p1, Pico_Pos p2);

/// @brief Draws a line using percentage-based coordinates.
/// @param p1 first endpoint position
/// @param p2 second endpoint position
/// @sa pico_output_draw_line_raw
void pico_output_draw_line_pct (Pico_Pos_Pct* p1, Pico_Pos_Pct* p2);

/// @brief Draws a single pixel using absolute coordinates.
/// @param pos drawing position
/// @sa pico_output_draw_pixel_pct
void pico_output_draw_pixel_raw (Pico_Pos pos);

/// @brief Draws a single pixel using percentage-based coordinates.
/// @param pos drawing position
/// @sa pico_output_draw_pixel_raw
void pico_output_draw_pixel_pct (Pico_Pos_Pct* pos);

/// @brief Draws a batch of pixels.
/// @param ps array of coordinates
/// @param n number of coordinates
void pico_output_draw_pixels (const Pico_Pos* ps, int n);

/// @brief Draws a rectangle using absolute coordinates.
/// @param rect rectangle to draw
/// @sa pico_output_draw_rect_pct
void pico_output_draw_rect_raw (Pico_Rect rect);

/// @brief Draws a rectangle using percentage-based coordinates.
/// @param rect rectangle to draw
/// @sa pico_output_draw_rect_raw
void pico_output_draw_rect_pct (const Pico_Rect_Pct* rect);

/// @brief Draws a triangle using absolute coordinates.
/// @param p1 first vertex position
/// @param p2 second vertex position
/// @param p3 third vertex position
/// @sa pico_output_draw_tri_pct
void pico_output_draw_tri_raw (Pico_Pos p1, Pico_Pos p2, Pico_Pos p3);

/// @brief Draws a triangle using percentage-based coordinates.
/// @param p1 first vertex position
/// @param p2 second vertex position
/// @param p3 third vertex position
/// @sa pico_output_draw_tri_raw
void pico_output_draw_tri_pct (const Pico_Pos_Pct* p1, const Pico_Pos_Pct* p2, const Pico_Pos_Pct* p3);

/// @brief Draws an ellipse using absolute coordinates.
/// @param rect bounding rectangle
/// @sa pico_output_draw_oval_pct
void pico_output_draw_oval_raw (Pico_Rect rect);

/// @brief Draws an ellipse using percentage-based coordinates.
/// @param rect bounding rectangle as percentages (0.0-1.0)
/// @sa pico_output_draw_oval_raw
void pico_output_draw_oval_pct (const Pico_Rect_Pct* rect);

/// @brief Draws a polygon using absolute coordinates.
/// @param ps array of vertex positions
/// @param n number of vertices
/// @sa pico_output_draw_poly_pct
void pico_output_draw_poly_raw (const Pico_Pos* ps, int n);

/// @brief Draws a polygon using percentage-based coordinates.
/// @param ps array of vertex coordinates
/// @param n number of vertices
/// @sa pico_output_draw_poly_raw
void pico_output_draw_poly_pct (const Pico_Pos_Pct* ps, int n);

/// @brief Draws text using absolute coordinates.
/// @param rect drawing rectangle
/// @param text text to draw
/// @sa pico_output_draw_text_pct
void pico_output_draw_text_raw (Pico_Rect rect, const char* text);

/// @brief Draws text using percentage-based coordinates.
/// @param rect drawing rectangle
/// @param text text to draw
/// @sa pico_output_draw_text_raw
void pico_output_draw_text_pct (Pico_Rect_Pct* rect, const char* text);

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

/// @brief Gets the cropping applied to objects when drawing them.
Pico_Rect pico_get_crop (void);

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
int pico_get_key (PICO_KEY key);

/// @brief Gets the mouse state using raw coordinates.
/// @param pos pointer to retrieve pointer position (may be NULL)
/// @param button which button state to retrieve
/// @return state of specified button
/// @sa pico_get_mouse_pct
int pico_get_mouse_raw (Pico_Pos* pos, int button);

/// @brief Gets the mouse state using percentage-based coordinates.
/// @param pos pointer to retrieve pointer position (may be NULL)
/// @param button which button state to retrieve
/// @return state of specified button
/// @sa pico_get_mouse_raw
int pico_get_mouse_pct (Pico_Pos_Pct* pos, int button);

/// @brief Gets the rotation angle of objects (in degrees).
int pico_get_rotate (void);

/// @brief Gets the dimensions of the given image.
/// @param file image filepath
Pico_Dim pico_get_dim_image (const char* file);

/// @brief Gets the dimensions of the given text.
/// @param text text to measure
int pico_get_text_width (int h, const char* text);

/// @brief Gets the visibility state of the window.
int pico_get_show (void);

/// @brief Gets the drawing style.
PICO_STYLE pico_get_style (void);

/// @brief Gets the amount of ticks that passed since pico was initialized.
Uint32 pico_get_ticks (void);

/// @brief Gets the aplication title.
const char* pico_get_title (void);

/// @brief Gets the current view configuration. NULL arguments are ignored.
/// @param window_fullscreen pointer to retrieve fullscreen state
/// @param window pointer to retrieve window dimensions
/// @param window_target pointer to retrieve window target region
/// @param world pointer to retrieve world/logical dimensions
/// @param world_source pointer to retrieve world source region
/// @param world_clip pointer to retrieve world clipping region
/// @sa pico_set_view_raw
/// @sa pico_set_view_pct
void pico_get_view (
    int* window_fullscreen,
    Pico_Dim* window,
    Pico_Rect* window_target,
    Pico_Dim* world,
    Pico_Rect* world_source,
    Pico_Rect* world_clip
);

// SET

/// @brief Sets the alpha transparency for drawing operations.
/// @param a alpha value (0: transparent; 255: opaque)
void pico_set_alpha (int a);

/// @brief Changes the reference to position objects (center, topleft, etc).
/// @include anchor.c
/// @param anchor anchor for the x and y axis
void pico_set_anchor_pos (Pico_Anchor anchor);

/// @brief Changes the reference to rotate objects (center, topleft, etc).
/// @include anchor.c
/// @param anchor anchor for the x and y axis
void pico_set_anchor_rotate (Pico_Anchor anchor);

/// @brief Changes the color used to clear the screen.
/// @param color new color
void pico_set_color_clear (Pico_Color color);

/// @brief Changes the color used to draw objects.
/// @param color new color
void pico_set_color_draw (Pico_Color color);

/// @brief Changes the cropping that is applied to images before drawing them.
/// @param crop cropping region, which may have 0 area to disable cropping
void pico_set_crop (Pico_Rect crop);

/// @brief Toggles the expert mode.
/// @param on 1 to enable it, or 0 to disable it
void pico_set_expert (int on);

/// @brief Sets the flipping state of objects.
void pico_set_flip (Pico_Flip flip);

/// @brief Changes the font used to draw texts.
/// @param file path to font file
void pico_set_font (const char* file);

/// @brief Toggles a grid on top of logical pixels.
/// @param on 1 to show it, or 0 to hide it
void pico_set_grid (int on);

/// @brief Sets the rotation angle of objects (in degrees).
void pico_set_rotate (int angle);

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
/// @param window_fullscreen 1 to enable fullscreen, or 0 to disable it
/// @param window window dimensions in pixels
/// @param window_target target region within window
/// @param world world/logical dimensions
/// @param world_source source region within world
/// @param world_clip clipping region within world
/// @sa pico_set_view_pct
/// @sa pico_get_view
void pico_set_view_raw (
    int window_fullscreen,
    Pico_Dim* window,
    Pico_Rect* window_target,
    Pico_Dim* world,
    Pico_Rect* world_source,
    Pico_Rect* world_clip
);

/// @brief Sets the view configuration using percentage-based dimensions. NULL arguments are ignored.
/// @param window_fullscreen 1 to enable fullscreen, or 0 to disable it
/// @param window window dimensions as percentages
/// @param window_target target region within window
/// @param world world/logical dimensions as percentages
/// @param world_source source region within world
/// @param world_clip clipping region within world
/// @sa pico_set_view_raw
/// @sa pico_get_view
void pico_set_view_pct (
    int            window_fullscreen,
    Pico_Pct*      window,
    void*          window_target_todo,
    Pico_Pct*      world,
    Pico_Rect_Pct* world_source,
    Pico_Rect_Pct* world_clip
);

/// @}

/// @defgroup Utils
/// @brief Utilities for users
/// @{

/// @brief Converts a percentage-based rectangle to absolute coordinates.
/// @param r percentage-based rectangle (0.0-1.0)
/// @return absolute rectangle in logical pixels
/// @sa pico_cv_rect_pct_raw_ext
Pico_Rect pico_cv_rect_pct_raw (const Pico_Rect_Pct* r);

/// @brief Converts a percentage-based rectangle to absolute coordinates relative to a reference rectangle.
/// @param r percentage-based rectangle (0.0-1.0)
/// @param ref reference rectangle to use as basis
/// @return absolute rectangle in logical pixels
/// @sa pico_cv_rect_pct_raw
Pico_Rect pico_cv_rect_pct_raw_ext (const Pico_Rect_Pct* r, Pico_Rect ref);

/// @brief Converts a percentage-based position to absolute coordinates.
/// @param p percentage-based position (0.0-1.0)
/// @return absolute position in logical pixels
/// @sa pico_cv_pos_pct_raw_ext
Pico_Pos pico_cv_pos_pct_raw (const Pico_Pos_Pct* p);

/// @brief Converts a percentage-based position to absolute coordinates relative to a reference rectangle.
/// @param p percentage-based position (0.0-1.0)
/// @param ref reference rectangle to use as basis
/// @return absolute position in logical pixels
/// @sa pico_cv_pos_pct_raw
Pico_Pos pico_cv_pos_pct_raw_ext (const Pico_Pos_Pct* p, Pico_Rect ref);


/// @brief Asserts condition and shows SDL error on failure.
/// @param x condition to assert
#define pico_assert(x) if (!(x)) { fprintf(stderr,"%s\n",SDL_GetError()); assert(0 && "SDL ERROR"); }

/// @brief Checks if a point is inside a rectangle using absolute coordinates.
/// Assumes that both primitives use the same anchor.
/// @param pos point in logical pixels
/// @param rect rectangle in logical pixels
/// @return 1 if pos is inside rect, or 0 otherwise
/// @sa pico_vs_pos_rect_pct
int pico_vs_pos_rect_raw (Pico_Pos pos, Pico_Rect rect);

/// @brief Checks if a point is inside a rectangle using percentage-based coordinates.
/// Assumes that both primitives use the same anchor.
/// @param pos point as percentages (0.0-1.0)
/// @param rect rectangle as percentages (0.0-1.0)
/// @return 1 if pos is inside rect, or 0 otherwise
/// @sa pico_vs_pos_rect_raw
int pico_vs_pos_rect_pct (Pico_Pos_Pct* pos, Pico_Rect_Pct* rect);

/// @brief Checks if two rectangles overlap using absolute coordinates.
/// Assumes that both rectangles use the same anchor.
/// @param r1 first rectangle in logical pixels
/// @param r2 second rectangle in logical pixels
/// @return 1 if r1 and r2 overlap, or 0 otherwise
/// @sa pico_vs_rect_rect_pct
int pico_vs_rect_rect_raw (Pico_Rect r1, Pico_Rect r2);

/// @brief Checks if two rectangles overlap using percentage-based coordinates.
/// Assumes that both rectangles use the same anchor.
/// @param r1 first rectangle as percentages (0.0-1.0)
/// @param r2 second rectangle as percentages (0.0-1.0)
/// @return 1 if r1 and r2 overlap, or 0 otherwise
/// @sa pico_vs_rect_rect_raw
int pico_vs_rect_rect_pct (Pico_Rect_Pct* r1, Pico_Rect_Pct* r2);

/// @}

#ifdef __cplusplus
}
#endif

#endif // PICO_H

