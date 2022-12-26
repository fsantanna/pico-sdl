#include <assert.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#define PICO_TITLE "pico-SDL"
#define PICO_WIN   510
#define PICO_HASH  128

#define pico_assert(x) if (!(x)) { fprintf(stderr,"%s\n",SDL_GetError()); assert(0 && "SDL ERROR"); }

#define SDL_ANY 0

typedef enum {
    Left=1, Center, Right
} Pico_HAnchor;

typedef enum {
    Bottom=1, Middle, Top
} Pico_VAnchor;

void pico_init (int on);
int pico_event_from_sdl (SDL_Event* e, int xp);
int pico_isPointVsRect (SDL_Point pt, SDL_Rect r);

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
void pico_output_draw_text  (SDL_Point pos, char* text);
void pico_output_present    (void);
void pico_output_write      (char* text);
void pico_output_writeln    (char* text);

void _pico_output_draw_image_cache (SDL_Point pos, char* path, int cache);

// STATE
void pico_state_get_size        (SDL_Point* size);
void pico_state_get_size_image  (char* file, SDL_Point* size);
void pico_state_set_anchor      (Pico_HAnchor h, Pico_VAnchor v);
void pico_state_set_auto        (int on);
void pico_state_set_color_clear (SDL_Color color);
void pico_state_set_color_draw  (SDL_Color color);
void pico_state_set_cursor      (SDL_Point pos);
void pico_state_set_image_crop  (SDL_Rect crop);
void pico_state_set_font        (char* file, int h);
void pico_state_set_grid        (int on);
void pico_state_set_pan         (SDL_Point pos);
void pico_state_set_size        (SDL_Point size);
void pico_state_set_size_image  (SDL_Point size);
void pico_state_set_size_pixel  (SDL_Point size);
void pico_state_set_title       (char* title);
