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

#define pico_assert(x) if (!(x)) { fprintf(stderr,"%s\n",SDL_GetError()); assert(0 && "SDL ERROR"); }

#define SDL_ANY 0

/// @brief Initializes and terminates pico.
/// @param on: 1 to initialize and 0 to terminate
void pico_init (int on);
int pico_event_from_sdl (SDL_Event* e, int xp);
int pico_is_point_in_rect (SDL_Point pt, SDL_Rect r);
SDL_Point pico_pct_to_pos (float x, float y);
SDL_Point pico_pct_to_pos_x (SDL_Rect r, float x, float y);

/// @defgroup Input
/// @brief Event handling.
/// @{

void pico_input_delay         (int ms);
void pico_input_event         (SDL_Event* evt, int type);
int  pico_input_event_ask     (SDL_Event* evt, int type);
int  pico_input_event_timeout (SDL_Event* evt, int type, int timeout);

/// @}

/// @defgroup Output
/// @brief Draw images and primitives, play sounds, etc.
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
/// @param text: the text to be drawn
void pico_output_draw_text (SDL_Point pos, const char* text);

/// @brief Shows the user what has been drawn to the screen since its last call.
void pico_output_present (void);

/// @brief Plays a sound.
/// @param path: path to the audio file
void pico_output_sound (const char* path);

void pico_output_write       (const char* text);
void pico_output_writeln     (const char* text);

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

SDL_Point pico_get_image_size    (const char* file);
SDL_Point pico_get_size          (void);
SDL_Point pico_get_size_external (void);
SDL_Point pico_get_size_internal (void);
Uint32    pico_get_ticks         (void);

/// @brief Sets the anchor of objects that will be drawn.
/// @param h Horizontal anchor. X axis.
/// @param v Vertical anchor. Y axis.
/// @b Example
/// @code
/// pico_set_anchor(PICO_LEFT, PICO_TOP);
/// @endcode
void pico_set_anchor        (Pico_HAnchor h, Pico_VAnchor v);
void pico_set_blend         (SDL_BlendMode mode);
void pico_set_color_clear   (SDL_Color color);
void pico_set_color_draw    (SDL_Color color);
void pico_set_cursor        (SDL_Point pos);
void pico_set_font          (const char* file, int h);
void pico_set_grid          (int on);
void pico_set_image_crop    (SDL_Rect crop);
void pico_set_image_size    (SDL_Point size);
void pico_set_pan           (SDL_Point pos);
void pico_set_size          (SDL_Point size);
void pico_set_size_external (SDL_Point phy);
void pico_set_size_internal (SDL_Point log);
void pico_set_show          (int on);
void pico_set_style         (Pico_Style style);
void pico_set_title         (const char* title);

/// @}


#ifdef __cplusplus
}
#endif

#endif // PICO_H

