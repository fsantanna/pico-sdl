// TODO Code examples for all functions.

#ifndef PICO_H
#define PICO_H

#include <stdio.h>
#include <assert.h>
#include <SDL2/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PICO_TITLE "pico-SDL"
#define PICO_PHY_X 640
#define PICO_PHY_Y 360
#define PICO_LOG_X  64
#define PICO_LOG_Y  36
#define PICO_HASH  128

#define SDL_ANY 0

/// @example init.c
/// @example delay.c
/// @example event.c
/// @example event_timeout.c
/// @example event_loop.c

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
/// @param type type of event to wait for (SDL_EventType), where 0 means any type
/// @sa pico_input_event_ask
/// @sa pico_input_event_timeout
void pico_input_event (SDL_Event* evt, int type);

/// @brief Checks if an event has occured.
/// @param evt where to save the event data, or NULL to ignore
/// @param type type of event to check the occurence (SDL_EventType), where 0 means any type
/// @return 1 if the given type of event has occurred, or 0 otherwise
/// @sa pico_input_event
/// @sa pico_input_event_timeout
int  pico_input_event_ask (SDL_Event* evt, int type);

/// @brief Stops the program until an event occurs or a timeout is reached.
/// @include event_timeout.c
/// @param evt where to save the event data, or NULL to ignore
/// @param type type of event to wait for (SDL_EventType), where 0 means any type
/// @param timeout time limit to wait for events in milliseconds
/// @return 1 if the given type of event has occurred, or 0 otherwise
/// @sa pico_input_event
/// @sa pico_input_event_ask
int  pico_input_event_timeout (SDL_Event* evt, int type, int timeout);

/// @}

/// @defgroup Output
/// @brief Draw images and primitives, play sounds, etc.
/// @{

/// @brief Clears screen with color set by @ref pico_set_color_clear.
void pico_output_clear (void);

/// @brief Draws an image.
/// This function uses caching, so the file is actually loaded only once.
/// @param pos drawing position
/// @param path path to the image file
void pico_output_draw_image (SDL_Point pos, const char* path);

/// @brief Draws a line segment.
/// @param p1 first point
/// @param p2 second point
void pico_output_draw_line (SDL_Point p1, SDL_Point p2);

/// @brief Draws a single pixel.
/// @param pos drawing position
void pico_output_draw_pixel (SDL_Point pos);

/// @brief Draws a batch of pixels.
/// @param apos array of positions
/// @param count amount of pixels to draw
void pico_output_draw_pixels (const SDL_Point* apos, int count);

/// @brief Draws a rectangle.
/// @param rect rectangle to draw
void pico_output_draw_rect (SDL_Rect rect);

/// @brief Draws an ellipse.
/// @param rect bounds of the ellipse
void pico_output_draw_oval (SDL_Rect rect);

/// @brief Draws text. The string can't be empty.
/// @param pos drawing position
/// @param text text to draw
void pico_output_draw_text (SDL_Point pos, const char* text);

/// @brief Shows what has been drawn onto the screen.
void pico_output_present (void);

/// @brief Plays a sound.
/// This function uses caching, so the file is actually loaded only once.
/// @param path path to the audio file
void pico_output_sound (const char* path);

// TODO Document me
void pico_output_write (const char* text);

// TODO Document me
void pico_output_writeln (const char* text);

void _pico_output_draw_image_cache (SDL_Point pos, const char* path, int cache);
void _pico_output_sound_cache (const char* path, int cache);

/// @}

/// @defgroup State
/// @brief All getters and setters.
/// @{

typedef enum {
    PICO_FILL, PICO_STROKE
} Pico_Style;

typedef enum {
    PICO_LEFT=1, PICO_CENTER, PICO_RIGHT
} Pico_HAnchor;

typedef enum {
    PICO_BOTTOM=1, PICO_MIDDLE, PICO_TOP
} Pico_VAnchor;

/// @brief Returns the size of a given image.
/// @param file path to image file
SDL_Point pico_get_image_size (const char* file);

/// @brief Returns the physical window size.
/// If it's different than the logical window size, the program crashes.
/// @sa pico_get_size_external
/// @sa pico_get_size_internal
SDL_Point pico_get_size (void);

/// @brief Returns the physical window size.
/// @sa pico_get_size_internal
/// @sa pico_get_size
SDL_Point pico_get_size_external (void);

/// @brief Returns the logical window size.
/// @sa pico_get_size_external
/// @sa pico_get_size
SDL_Point pico_get_size_internal (void);

/// @brief Returns the amount of ticks that passed since pico was initialized.
Uint32 pico_get_ticks (void);

// TODO Document me better
/// @brief Changes the coordinate system (anchor) of objects to draw.
/// @include anchor.c
/// @param h x-axis anchor
/// @param v y-axis anchor
void pico_set_anchor (Pico_HAnchor h, Pico_VAnchor v);

/// @brief Changes the color used to clear the screen.
/// @param color new color
/// @sa pico_output_clear
void pico_set_color_clear (SDL_Color color);

/// @brief Changes the color used to draw objects.
/// @param color new color
void pico_set_color_draw (SDL_Color color);

// TODO Document me
void pico_set_cursor (SDL_Point pos);

/// @brief Changes the font used to draw texts.
/// @param file path to font file
/// @param h size of the font
void pico_set_font (const char* file, int h);

/// @brief Toggles a grid on top of logical pixels.
/// @param on 1 to show it, or 0 to hide it
void pico_set_grid (int on);

/// @brief Changes the cropping that is applied to images before drawing them.
/// @param crop cropping region, which may have 0 area to disable cropping
void pico_set_image_crop (SDL_Rect crop);

/// @brief Changes what size images should be when drawn.
/// @param size new size, which may be (0, 0) to disable resizing
void pico_set_image_size (SDL_Point size);

/// @brief Changes the point of view on the logical window.
/// @param pos new point of view
void pico_set_pan (SDL_Point pos);

/// @brief Changes the physical window size and the logical window size.
/// @param size new size
/// @sa pico_set_size_external
/// @sa pico_set_size_internal
void pico_set_size (SDL_Point size);

/// @brief Changes the physical window size.
/// @param phy new physical size
/// @sa pico_set_size_internal
/// @sa pico_set_size
void pico_set_size_external (SDL_Point phy);

/// @brief Changes the logical window size.
/// @param log new logical size
/// @sa pico_set_size_external
/// @sa pico_set_size
void pico_set_size_internal (SDL_Point log);

/// @brief Toggles the aplication window visibility.
/// @param on 1 to show, or 0 to hide
void pico_set_show (int on);

/// @brief Changes the style used to draw objects.
/// @param style new style to use
void pico_set_style (Pico_Style style);

/// @brief Changes the aplication title
/// @param title new title to set
void pico_set_title (const char* title);

/// @}

/// @defgroup Utils
/// @brief Utilities for users
/// @{

/// @brief Asserts condition and shows SDL error on failure
/// @param x condition to assert
#define pico_assert(x) if (!(x)) { fprintf(stderr,"%s\n",SDL_GetError()); assert(0 && "SDL ERROR"); }

/// @brief Checks if a point is inside a rectangle.
/// @param pt point
/// @param r rectangle
/// @return 1 if pt is inside r, or 0 otherwise
int pico_is_point_in_rect (SDL_Point pt, SDL_Rect r);

// TODO Document me
SDL_Point pico_pct_to_pos (float x, float y);

// TODO Document me
SDL_Point pico_pct_to_pos_x (SDL_Rect r, float x, float y);

/// @}

#ifdef __cplusplus
}
#endif

#endif // PICO_H

