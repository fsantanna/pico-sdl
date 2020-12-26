#include <assert.h>
#include <SDL2/SDL.h>

#define _TITLE_ "pico-SDL"
#define _WIN_ 510
#define pico_assert(x) if (!(x)) { fprintf(stderr,"%s\n",SDL_GetError()); assert(0 && "SDL ERROR"); }

typedef struct {
    int v1, v2;
} Pico_2i;

typedef struct {
    int v1, v2, v3, v4;
} Pico_4i;

typedef enum {
    PICO_DELAY,
    PICO_EVENT
} PICO_INPUT;

typedef enum {
    PICO_CLEAR,
    PICO_DRAW,
    PICO_SET,
    PICO_UPDATE
} PICO_OUTPUT;

typedef enum {
    PICO_COLOR_BG,
    PICO_COLOR_FG,
    PICO_SIZE,
    PICO_TITLE
} PICO_OUTPUT_SET;

typedef enum {
    PICO_PIXEL
} PICO_OUTPUT_DRAW;

typedef struct {
    PICO_INPUT sub;
    union {
        int Delay;      // INPUT_DELAY
        struct {        // INPUT_EVENT
            int type;
            int timeout;
            SDL_Event* ret;
        } Event;
    };
} Pico_Input;

typedef struct {
    PICO_OUTPUT sub;
    union {
        struct {
            PICO_OUTPUT_SET sub;
            union {
                Pico_4i Color_BG;
                Pico_4i Color_FG;
                struct {
                    int win_w, win_h, log_w, log_h;
                } Size;
                char* Title;
            };
        } Set;
        struct {
            PICO_OUTPUT_DRAW sub;
            union {
                Pico_2i Pixel;
            };
        } Draw;
    };
} Pico_Output;

void pico_init   ();
int  pico_input  (Pico_Input inp);
void pico_output (Pico_Output out);
