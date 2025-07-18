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

typedef enum {
    PICO_FILL, PICO_STROKE
} Pico_Style;

typedef struct Pico_Anchor {
    enum { PICO_LEFT=1, PICO_CENTER, PICO_RIGHT } x;
    enum { PICO_BOTTOM=1, PICO_MIDDLE, PICO_TOP } y;
} Pico_Anchor;

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

/// TODO
void pico_output_draw_buffer (Pico_Pos pos, const Pico_Color buffer[], Pico_Dim size);


/// @brief Draws an image.
/// This function uses caching, so the file is actually loaded only once.
/// @param pos drawing position
/// @param path path to the image file
void pico_output_draw_image (Pico_Pos pos, const char* path);

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

/// @brief Draws an ellipse.
/// @param rect bounds of the ellipse
void pico_output_draw_oval (Pico_Rect rect);

/// @brief Draws text. The string can't be empty.
/// @param pos drawing position
/// @param text text to draw
void pico_output_draw_text (Pico_Pos pos, const char* text);

/// @brief Shows what has been drawn onto the screen.
void pico_output_present (void);

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

/// TODO
Pico_Color pico_get_color_draw (void);

/// @brief Returns the size of a given image.
/// @param file path to image file
Pico_Dim pico_get_image_size (const char* file);

/// @brief TODO
Pico_Pos pico_get_scroll (void);

/// @brief Returns the physical and logical window size.
Pico_Size pico_get_size (void);

/// @brief TODO
Pico_Style pico_get_style (void);

/// @brief Returns the amount of ticks that passed since pico was initialized.
Uint32 pico_get_ticks (void);

/// @brief TODO
Pico_Dim pico_get_zoom (void);

/// @brief Changes the reference point used to draw objects (center, topleft, etc).
/// @include anchor.c
/// @param h x-axis anchor
/// @param v y-axis anchor
void pico_set_anchor (Pico_Anchor anchor);

/// @brief Changes the color used to clear the screen.
/// @param color new color
/// @sa pico_output_clear
void pico_set_color_clear (Pico_Color color);

/// @brief Changes the color used to draw objects.
/// @param color new color
void pico_set_color_draw (Pico_Color color);

/// @brief Sets the position of the text cursor.
/// @param pos new cursor position
/// @sa pico_output_write
/// @sa pico_output_writeln
void pico_set_cursor (Pico_Pos pos);

/// @brief Changes the font used to draw texts.
/// @param file path to font file
/// @param h size of the font
void pico_set_font (const char* file, int h);

/// @brief Toggles the expert mode.
/// @param on 1 to enable it, or 0 to disable it
void pico_set_expert (int on);

/// @brief Toggles a grid on top of logical pixels.
/// @param on 1 to show it, or 0 to hide it
void pico_set_grid (int on);

/// @brief Changes the cropping that is applied to images before drawing them.
/// @param crop cropping region, which may have 0 area to disable cropping
void pico_set_image_crop (Pico_Rect crop);

/// @brief Changes what size images should be when drawn.
/// @param size new size, which may be (0, 0) to disable resizing
void pico_set_image_size (Pico_Dim size);

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

/// @brief Changes the style used to draw objects.
/// @param style new style to use
void pico_set_style (Pico_Style style);

/// @brief Changes the aplication title
/// @param title new title to set
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

/// @brief Checks if a point is inside a rectangle
/// @param pt point
/// @param r rectangle
/// @return 1 if pt is inside r, or 0 otherwise
int pico_is_point_in_rect (Pico_Pos pt, Pico_Rect r);

/// @brief Returns a screen coordinate based on percentage values.
/// @param x an integer ranging from 0 to 100
/// @param y an integer ranging from 0 to 100
/// @sa pico_pos_ext
Pico_Pos pico_pos (int x, int y);

/// @brief Returns a screen coordinate based on percentage values.
// The coordinate returned will be inside the given rectangle.
/// @param r the reference rectangle
/// @param x an integer ranging from 0 to 100
/// @param y an integer ranging from 0 to 100
/// @sa pico_pos
Pico_Pos pico_pos_ext (Pico_Rect r, int x, int y);

/// @}

#ifdef __cplusplus
}
#endif

#endif // PICO_H

