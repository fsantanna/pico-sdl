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

#define PICO_TITLE "pico-SDL"
#define PICO_DIM_PHY ((Pico_Dim) {640,360})
#define PICO_DIM_LOG ((Pico_Dim) { 64, 36})
#define PICO_HASH  128

/// @example init.c
/// @example delay.c
/// @example event.c
/// @example event_timeout.c
/// @example event_loop.c

/// @defgroup Types
/// @brief TODO.
/// @{

typedef SDL_Point Pico_Pos;
typedef SDL_Point Pico_Dim;
typedef SDL_Rect  Pico_Rect;
typedef SDL_Color Pico_Color;
typedef SDL_Point Pico_Anchor;
typedef SDL_Point Pico_Flip;

#define PICO_LEFT   0
#define PICO_CENTER 50
#define PICO_RIGHT  100
#define PICO_TOP    0
#define PICO_MIDDLE 50
#define PICO_BOTTOM 100

typedef enum PICO_STYLE {
    PICO_FILL, PICO_STROKE
} PICO_STYLE;

typedef struct Pico_Size {
    Pico_Dim phy;
    Pico_Dim log;
} Pico_Size;

#define PICO_SIZE_KEEP       ((Pico_Dim) {0,0})
#define PICO_SIZE_FULLSCREEN ((Pico_Dim) {0,1})

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

/// @brief Stops the program until a given number of milliseconds have passed.
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
/// @param timeout time limit to wait for events in milliseconds
/// @return 1 if the given type of event has occurred, or 0 otherwise
/// @sa pico_input_event
/// @sa pico_input_event_ask
int  pico_input_event_timeout (Pico_Event* evt, int type, int timeout);

/// @}

/// @defgroup Output
/// @brief Draw images and primitives, play sounds, etc.
/// @{

/// @brief Clears screen with color set by @ref pico_set_color_clear.
void pico_output_clear (void);

/// @brief Draws an image that is managed by the user.
/// @param pos drawing postion
/// @param buffer the image
/// @param size size of the image
/// @sa pico_output_draw_image
/// @sa pico_output_draw_image_ext
void pico_output_draw_buffer (Pico_Pos pos, const Pico_Color buffer[], Pico_Dim size);

/// @brief Draws an image.
/// This function uses caching, so the file is actually loaded only once.
/// @param pos drawing position
/// @param path path to the image file
/// @sa pico_output_draw_buffer
/// @sa pico_output_draw_image_ext
void pico_output_draw_image (Pico_Pos pos, const char* path);

/// @brief Draws an image with the specified size.
/// This function uses caching, so the file is actually loaded only once.
/// @param pos drawing position
/// @param path path to the image file
/// @param size image size
/// @sa pico_output_draw_buffer
/// @sa pico_output_draw_image
void pico_output_draw_image_ext (Pico_Pos pos, const char* path, Pico_Dim size);

/// @brief Draws a line segment.
/// @param p1 first point
/// @param p2 second point
void pico_output_draw_line (Pico_Pos p1, Pico_Pos p2);

/// @brief Draws a single pixel.
/// @param pos drawing position
void pico_output_draw_pixel (Pico_Pos pos);

/// @brief Draws a batch of pixels.
/// @param apos array of positions
/// @param count amount of pixels to draw
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

/// @brief Draws a generic polygon.
/// @param apos array of vertices
/// @param count amount of vertices
void pico_output_draw_poly (const Pico_Pos* apos, int count);

/// @brief Draws text.
/// @param pos drawing position
/// @param text text to draw
/// @sa pico_output_draw_text_ext
void pico_output_draw_text (Pico_Pos pos, const char* text);

/// @brief Draws text with the specified size.
/// @param pos drawing position
/// @param text text to draw
/// @param size image size
/// @sa pico_output_draw_text
void pico_output_draw_text_ext (Pico_Pos pos, const char* text, Pico_Dim size);

/// @brief Shows what has been drawn onto the screen.
void pico_output_present (void);

/// @brief Takes a screenshot.
/// @param path screenshot filepath (NULL uses timestamp in the name)
/// @return The filepath of the screenshot.
const char* pico_output_screenshot (const char* path);

/// @brief Takes a screenshot from a specific portion of the screen.
/// @param path screenshot filepath (NULL uses timestamp in the name)
/// @param r region to screenshot, in logical pixels
/// @return The filepath of the screenshot.
const char* pico_output_screenshot_ext (const char* path, Pico_Rect r);

/// @brief Plays a sound.
/// This function uses caching, so the file is actually loaded only once.
/// @param path path to the audio file
void pico_output_sound (const char* path);

/// @brief Draws text with an internal cursor as reference, like in text editors.
/// The cursor position updates to (x + len_text * FNT_SIZE, y).
/// @param text text to draw
/// @sa pico_set_cursor
/// @sa pico_output_writeln
void pico_output_write (const char* text);

/// @brief Draws a line of text with an internal cursor as reference, like in text editors.
/// The cursor position updates to (x, y + FNT_SIZE).
/// @param text text to draw
/// @sa pico_set_cursor
/// @sa pico_output_write
void pico_output_writeln (const char* text);

/// @}

/// @defgroup State
/// @brief All getters and setters.
/// @{

// GET

/// @brief Gets the reference to draw objects (center, topleft, etc).
Pico_Anchor pico_get_anchor_draw (void);

/// @brief Gets the reference to draw objects (center, topleft, etc).
Pico_Anchor pico_get_anchor_rotate (void);

/// @brief Gets the color set to clear the screen.
/// @sa pico_output_clear
/// @sa pico_set_color_clear
Pico_Color pico_get_color_clear (void);

/// @brief Gets the color set to draw.
/// @sa pico_set_color_draw
Pico_Color pico_get_color_draw (void);

/// @brief Gets the cropping applied to images before drawing them.
Pico_Rect pico_get_crop (void);

/// @brief Gets the position of the text cursor.
/// @sa pico_output_write
/// @sa pico_output_writeln
Pico_Pos pico_get_cursor (void);

/// @brief Checks the state of expert mode.
int pico_get_expert (void);

/// @brief Gets the flipping value used to draw objects.
/// @sa Pico_Flip
Pico_Flip pico_get_flip (void);

/// @brief Gets the font used to draw texts.
const char* pico_get_font (void);

/// @brief Checks the state of the logical pixel grid.
int pico_get_grid (void);

/// @brief Gets the rotation angle of objects (in degrees).
int pico_get_rotate (void);

/// @brief TODO
Pico_Dim pico_get_scale (void);

/// @brief TODO
Pico_Pos pico_get_scroll (void);

/// @brief Gets the physical and logical window size.
Pico_Size pico_get_size (void);

/// @brief Gets the size of a given image.
/// @param file path to image file
Pico_Dim pico_get_size_image (const char* file);

/// @brief Gets the size of a given text.
/// @param text text to measure
Pico_Dim pico_get_size_text (const char* text);

/// @brief Checks if the aplication window is visible.
int pico_get_show (void);

/// @brief Gets the drawing style.
PICO_STYLE pico_get_style (void);

/// @brief Gets the amount of ticks that passed since pico was initialized.
Uint32 pico_get_ticks (void);

/// @brief Gets the aplication title.
const char* pico_get_title (void);

/// @brief TODO
Pico_Dim pico_get_zoom (void);

// SET

/// @brief Changes the reference to draw objects (center, topleft, etc).
/// @include anchor.c
/// @param h x-axis anchor
/// @param v y-axis anchor
void pico_set_anchor_draw (Pico_Anchor anchor);

/// @brief Changes the reference to rotate objects (center, topleft, etc).
/// @include anchor.c
/// @param h x-axis anchor
/// @param v y-axis anchor
void pico_set_anchor_rotate (Pico_Anchor anchor);

/// @brief Changes the color used to clear the screen.
/// @param color new color
/// @sa pico_output_clear
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

/// @brief Toggles the expert mode.
/// @param on 1 to enable it, or 0 to disable it
void pico_set_expert (int on);

// TODO: document me
void pico_set_flip (Pico_Flip flip);

/// @brief Changes the font used to draw texts.
/// @param file path to font file
/// @param h size of the font
void pico_set_font (const char* file, int h);

/// @brief Toggles a grid on top of logical pixels.
/// @param on 1 to show it, or 0 to hide it
void pico_set_grid (int on);

/// @brief Sets the rotation angle of objects (in degrees).
void pico_set_rotate (int angle);

void pico_set_scale (Pico_Dim scale);

/// @brief Changes the point of view on the logical window.
/// @param pos new point of view
void pico_set_scroll (Pico_Pos pos);

/// @brief Changes the physical and logical window sizes.
/// @param phy new physical size
/// @param log new logical size
/// @sa pico_get_size
void pico_set_size (Pico_Dim phy, Pico_Dim log);

/// @brief Toggles the aplication window visibility.
/// @param on 1 to show, or 0 to hide
void pico_set_show (int on);

/// @brief Changes the drawing style
/// @param style new style
void pico_set_style (PICO_STYLE style);

/// @brief Changes the aplication title
/// @param title new title
void pico_set_title (const char* title);

/// @brief TODO
/// @param TODO
void pico_set_zoom (Pico_Dim zoom);

/// @}

/// @defgroup Utils
/// @brief Utilities for users
/// @{

/// @brief Asserts condition and shows SDL error on failure
/// @param x condition to assert
#define pico_assert(x) if (!(x)) { fprintf(stderr,"%s\n",SDL_GetError()); assert(0 && "SDL ERROR"); }

/// @brief Returns a size relative to the screen size.
/// @param x percentage that may go out of [0,100]
/// @param y percentage that may go out of [0,100]
/// @sa pico_dim_ext
Pico_Dim pico_dim (int x, int y);

/// @brief Returns a size relative to the given rectangle's size.
/// @param r the reference rectangle
/// @param x percentage that may go out of [0,100]
/// @param y percentage that may go out of [0,100]
/// @sa pico_dim
Pico_Dim pico_dim_ext (Pico_Dim d, int x, int y);

/// @brief Checks if a point is inside a rectangle.
/// Assumes that both primitives use the same anchor.
/// @param pt point
/// @param r rectangle
/// @return 1 if pt is inside r, or 0 otherwise
int pico_pos_vs_rect (Pico_Pos pt, Pico_Rect r);

/// @brief Checks if a point is inside a rectangle.
/// @param pt point
/// @param ap anchor for pt
/// @param r rectangle
/// @param ar anchor for r
/// @return 1 if pt is inside r, or 0 otherwise
int pico_pos_vs_rect_ext (Pico_Pos pt, Pico_Anchor ap, Pico_Rect r, Pico_Anchor ar);

/// @brief Returns a coordinate relative to the screen rectangle.
/// @param x percentage that may go out of [0,100]
/// @param y percentage that may go out of [0,100]
/// @sa pico_pos_ext
Pico_Pos pico_pos (int x, int y);

/// @brief Returns a coordinate relative to the given rectangle's position.
/// @param r the reference rectangle
/// @param x percentage that may go out of [0,100]
/// @param y percentage that may go out of [0,100]
/// @sa pico_pos
Pico_Pos pico_pos_ext (Pico_Rect r, int x, int y);

/// @brief Checks if two rectangles overlap.
/// Assumes that both rectangles use the same anchor.
/// @param r1 rectangle 1
/// @param r2 rectangle 2
/// @return 1 if r1 and r2 overlap, or 0 otherwise
/// @sa pico_rect_vs_rect_ext
int pico_rect_vs_rect (Pico_Rect r1, Pico_Rect r2);

/// @brief Checks if two rectangles with different anchors overlap.
/// @param r1 rectangle 1
/// @param a1 anchor for r1
/// @param r2 rectangle 2
/// @param a1 anchor for r2
/// @return 1 if r1 and r2 overlap, or 0 otherwise
/// @sa pico_pos_vs_rect
int pico_rect_vs_rect_ext (Pico_Rect r1, Pico_Anchor a1, Pico_Rect r2, Pico_Anchor a2);

/// @}

#ifdef __cplusplus
}
#endif

#endif // PICO_H

