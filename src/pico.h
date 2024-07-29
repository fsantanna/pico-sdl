#include <stdio.h>
#include <assert.h>
#include <SDL2/SDL.h>

#define PICO_TITLE "pico-SDL"
#define PICO_PHY_X 640
#define PICO_PHY_Y 360
#define PICO_LOG_X  64
#define PICO_LOG_Y  36
#define PICO_HASH  128

#define pico_assert(x) if (!(x)) { fprintf(stderr,"%s\n",SDL_GetError()); assert(0 && "SDL ERROR"); }

#define SDL_ANY 0

typedef enum {
    Fill, Stroke
} Pico_Style;

typedef enum {
    Left=1, Center, Right
} Pico_HAnchor;

typedef enum {
    Bottom=1, Middle, Top
} Pico_VAnchor;

void pico_init (int on);
int pico_event_from_sdl (SDL_Event* e, int xp);
int pico_is_point_in_rect (SDL_Point pt, SDL_Rect r);
SDL_Point pico_pct_to_pos (float x, float y);
SDL_Point pico_pct_to_pos_x (SDL_Rect r, float x, float y);

// INPUT
void pico_input_delay         (int ms);
void pico_input_event         (SDL_Event* evt, int type);
int  pico_input_event_ask     (SDL_Event* evt, int type);
int  pico_input_event_timeout (SDL_Event* evt, int type, int timeout);

// OUTPUT
void pico_output_clear      (void);
void pico_output_draw_image (SDL_Point pos, char* path);
void pico_output_draw_line  (SDL_Point p1, SDL_Point p2);
void pico_output_draw_pixel (SDL_Point pos);
void pico_output_draw_rect  (SDL_Rect rect);
void pico_output_draw_oval  (SDL_Rect rect);
void pico_output_draw_text  (SDL_Point pos, char* text);
void pico_output_present    (void);
void pico_output_sound      (char* path);
void pico_output_write      (char* text);
void pico_output_writeln    (char* text);

void _pico_output_draw_image_cache (SDL_Point pos, char* path, int cache);
void _pico_output_sound_cache (char* path, int cache);

// STATE

SDL_Point pico_get_image_size    (char* file);
SDL_Point pico_get_size_external (void);
SDL_Point pico_get_size_internal (void);
Uint32    pico_get_ticks         (void);

void pico_set_anchor        (Pico_HAnchor h, Pico_VAnchor v);
void pico_set_color_clear   (SDL_Color color);
void pico_set_color_draw    (SDL_Color color);
void pico_set_cursor        (SDL_Point pos);
void pico_set_font          (char* file, int h);
void pico_set_grid          (int on);
void pico_set_image_crop    (SDL_Rect crop);
void pico_set_image_size    (SDL_Point size);
void pico_set_pan           (SDL_Point pos);
void pico_set_size_external (SDL_Point phy);
void pico_set_size_internal (SDL_Point log);
void pico_set_show          (int on);
void pico_set_style         (Pico_Style style);
void pico_set_title         (char* title);
