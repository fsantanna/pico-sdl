/// @file pico.h
/// @brief Contains @ref Input, @ref Output and @ref State modules.

// TODO: Code examples for all functions.

#ifndef PICO_H
#define PICO_H

#include <stdio.h>
#include <assert.h>
#include <SDL2/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

// TODO: Document me
#define pico_assert(x) if (!(x)) { fprintf(stderr,"%s\n",SDL_GetError()); assert(0 && "SDL ERROR"); }

#define SDL_ANY 0

/// @defgroup Init
/// @brief Functions and default values used in initialization. Include @ref pico.h.
/// @{

/// @brief Default window title
#define PICO_TITLE "pico-SDL"

/// @brief Default physical window width.
#define PICO_PHY_X 640

/// @brief Default physical window height.
#define PICO_PHY_Y 360

/// @brief Default logical window width.
#define PICO_LOG_X  64

/// @brief Default logical window height.
#define PICO_LOG_Y  36

/// @brief Default amount of buckets of internal hash object.
/// @sa Hash
#define PICO_HASH  128

/// @brief Initializes and terminates pico.
/// @param on: 1 to initialize, or 0 to terminate
///
/// @include init.c
void pico_init (int on);

/// @}

// TODO: Document me
int pico_event_from_sdl (SDL_Event* e, int xp);

// TODO: Document me
int pico_is_point_in_rect (SDL_Point pt, SDL_Rect r);

// TODO: Document me
SDL_Point pico_pct_to_pos (float x, float y);

// TODO: Document me
SDL_Point pico_pct_to_pos_x (SDL_Rect r, float x, float y);

/// @defgroup Input
/// @brief Event handling. Include @ref pico.h.
/// @{

// TODO: Document me
void pico_input_delay (int ms);

// TODO: Document me
void pico_input_event (SDL_Event* evt, int type);

// TODO: Document me
int  pico_input_event_ask (SDL_Event* evt, int type);

// TODO: Document me
int  pico_input_event_timeout (SDL_Event* evt, int type, int timeout);

/// @}

/// @defgroup Output
/// @brief Draw images and primitives, play sounds, etc. Include @ref pico.h.
/// @{

/// @brief Clears screen with color set by @ref pico_set_color_clear.
void pico_output_clear (void);

/// @brief Draws an image.
/// @param pos: position where to draw the image
/// @param path: path to the image file
void pico_output_draw_image (SDL_Point pos, const char* path);

/// @brief Draws a line.
/// @param p1: first point
/// @param p2: second point
void pico_output_draw_line (SDL_Point p1, SDL_Point p2);

/// @brief Draws a single pixel.
/// @param pos: the position of the pixel in game coordinates
void pico_output_draw_pixel (SDL_Point pos);

/// @brief Draws a batch of pixels.
/// @param apos: array of pixels
/// @param count: amount of pixels to draw
void pico_output_draw_pixels (const SDL_Point* apos, int count);

/// @brief Draws a rectangle.
/// @param rect: the rectangle to draw
void pico_output_draw_rect (SDL_Rect rect);

/// @brief Draws an ellipse that fits exatcly inside the rectangle.
/// @param rect: bounds of the ellipse
void pico_output_draw_oval (SDL_Rect rect);

/// @brief Draws text. The string can't be empty.
/// @param pos: the position to draw the text
/// @param text: the text to draw
void pico_output_draw_text (SDL_Point pos, const char* text);

/// @brief Shows what has been drawn to the screen since last call to @ref pico_output_clear
void pico_output_present (void);

/// @brief Plays a sound.
/// @param path: path to the audio file
void pico_output_sound (const char* path);

// TODO: Document me
void pico_output_write (const char* text);

// TODO: Document me
void pico_output_writeln (const char* text);

void _pico_output_draw_image_cache (SDL_Point pos, const char* path, int cache);
void _pico_output_sound_cache (const char* path, int cache);

/// @}

/// @defgroup State
/// @brief All getters and setters. Include @ref pico.h.
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
/// @param file: path to image file
SDL_Point pico_get_image_size (const char* file);

/// @brief Returns the physical window size. If it's different than the
/// logical window size, the program crashes.
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

// TODO: Document me better
/// @brief Changes the anchor of objects that will be drawn.
/// @param h Horizontal anchor. X axis.
/// @param v Vertical anchor. Y axis.
void pico_set_anchor (Pico_HAnchor h, Pico_VAnchor v);

/// @brief Changes the blend modes.
/// @param mode: new blend mode
void pico_set_blend (SDL_BlendMode mode);

/// @brief Changes the color used to clear the screen.
/// @param color: new color to use
/// @sa pico_output_clear
void pico_set_color_clear (SDL_Color color);

/// @brief Changes the color used to draw objects.
/// @param color: new color to use
void pico_set_color_draw (SDL_Color color);

// TODO: Document me
void pico_set_cursor (SDL_Point pos);

/// @brief Changes the font that pico will use to draw texts.
/// @param file: path to font file
/// @param h: point size of the font
void pico_set_font (const char* file, int h);

/// @brief Shows or hides quadrangular grid in the game canvas.
/// @param on: 1 to show it, or 0 to hide it
void pico_set_grid (int on);

/// @brief Selects a section of the image to draw.
/// @param crop: section to select
void pico_set_image_crop (SDL_Rect crop);

// TODO: Document me
void pico_set_image_size (SDL_Point size);

// TODO: Document me
void pico_set_pan (SDL_Point pos);

/// @brief Changes the physical window size and the logical window size.
/// @param size: new size to set
/// @sa pico_set_size_external
/// @sa pico_set_size_internal
void pico_set_size (SDL_Point size);

/// @brief Changes the physical window size.
/// @param phy: new physical size to set
/// @sa pico_set_size_internal
/// @sa pico_set_size
void pico_set_size_external (SDL_Point phy);

/// @brief Changes the logical window size.
/// @param log: new logical size to set
/// @sa pico_set_size_external
/// @sa pico_set_size
void pico_set_size_internal (SDL_Point log);

/// @brief Shows or hides the aplication window.
/// @param on: 1 to show, or 0 to hide
void pico_set_show (int on);

/// @brief Changes the style used to draw objects.
/// @param style: new style to use
void pico_set_style (Pico_Style style);

/// @brief Changes the aplication title
/// @param title: new title to set
void pico_set_title (const char* title);

/// @}

#ifdef __cplusplus
}
#endif

#endif // PICO_H

