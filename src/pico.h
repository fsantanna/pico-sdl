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
#define PICO_DIM_WINDOW ((Pico_Dim) {640,360})
#define PICO_DIM_WORLD  ((Pico_Dim) { 64, 36})
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
#define PICO_VIEWPORT_RESET ((Pico_Rect) {0,0,0,0})

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
int pico_get_key (PICO_KEY key);

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
Pico_Dim pico_get_dim_window (void);

/// @brief Gets the world dimensions.
Pico_Dim pico_get_dim_world (void);

/// @brief Gets the visibility state of the window.
int pico_get_show (void);

/// @brief Gets the drawing style.
PICO_STYLE pico_get_style (void);

/// @brief Gets the amount of ticks that passed since pico was initialized.
Uint32 pico_get_ticks (void);

/// @brief Gets the aplication title.
const char* pico_get_title (void);

/// @brief Gets the current viewport.
Pico_Rect pico_get_viewport (void);

/// @brief Gets the zoom factor.
Pico_Pct pico_get_zoom (void);

// SET

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

/// @brief Sets the position of the text cursor.
/// @param pos new cursor position
/// @sa pico_output_write
/// @sa pico_output_writeln
void pico_set_cursor (Pico_Pos pos);

/// @brief Sets the window dimensions.
/// @param dim new dimensions
void pico_set_dim_window (Pico_Dim dim);

/// @brief Sets the world dimensions.
/// @param dim new dimensions
void pico_set_dim_world (Pico_Dim dim);

/// @brief Toggles the expert mode.
/// @param on 1 to enable it, or 0 to disable it
void pico_set_expert (int on);

/// @brief Sets the flipping state of objects.
void pico_set_flip (Pico_Flip flip);

/// @brief Changes the font used to draw texts.
/// @param file path to font file
/// @param h size of the font
void pico_set_font (const char* file, int h);

/// @brief Toggles fullscreen mode.
/// @param on 1 to enable it, or 0 to disable it
void pico_set_fullscreen (int on);

/// @brief Toggles a grid on top of logical pixels.
/// @param on 1 to show it, or 0 to hide it
void pico_set_grid (int on);

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

/// @brief Sets the viewport for further drawing operations.
/// @param rect viewport rectangle relative to position anchor
void pico_set_viewport (Pico_Rect rect);

/// @param pct new factor
void pico_set_zoom (Pico_Pct pct);

/// @}

/// @defgroup Utils
/// @brief Utilities for users
/// @{

/// @brief Asserts condition and shows SDL error on failure.
/// @param x condition to assert
#define pico_assert(x) if (!(x)) { fprintf(stderr,"%s\n",SDL_GetError()); assert(0 && "SDL ERROR"); }

/// @brief Returns the dimensions relative to the screen.
/// @param pct percentage (may be out of [0,100])
/// @sa pico_dim_ext
Pico_Dim pico_dim (Pico_Pct pct);

/// @brief Returns the dimensions relative to the given rectangle.
/// @param pct percentage (may be out of [0,100])
/// @param dim the reference rectangle
/// @sa pico_dim
Pico_Dim pico_dim_ext (Pico_Pct pct, Pico_Dim d);

/// @brief Returns a coordinate relative to the screen rectangle.
/// @param pct percentage (may be out of [0,100])
/// @sa pico_pos_ext
Pico_Pos pico_pos (Pico_Pct pct);

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

/// @brief Returns a rectangle relative to the screen rectangle.
/// @param pos percentage (may be out of [0,100])
/// @param dim percentage (may be out of [0,100])
/// @sa pico_rect_ext
Pico_Rect pico_rect (Pico_Pct pos, Pico_Pct dim);

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

