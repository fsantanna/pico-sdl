#include <assert.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define _TITLE_ "pico-SDL"
#define _WIN_ 510
#define pico_assert(x) if (!(x)) { fprintf(stderr,"%s\n",SDL_GetError()); assert(0 && "SDL ERROR"); }

#define SDL_ANY 0

typedef unsigned char u8;

typedef struct {
    int _1, _2;
} Pico_2i;

typedef struct {
    int _1, _2, _3, _4;
} Pico_4i;

typedef enum {
    PICO_DELAY,
    PICO_EVENT,
    PICO_EVENT_TIMEOUT
} PICO_INPUT;

typedef enum {
    PICO_CLEAR,
    PICO_DRAW,
    PICO_SET,
    PICO_WRITE,
    PICO_WRITELN
} PICO_OUTPUT;

typedef enum {
    PICO_COLOR,
    PICO_CURSOR,
    PICO_FONT,
    PICO_SIZE,
    PICO_TITLE
} PICO_OUTPUT_SET;

typedef enum {
    PICO_COLOR_CLEAR,
    PICO_COLOR_DRAW
} PICO_OUTPUT_SET_COLOR;

typedef enum {
    PICO_PIXEL,
    PICO_TEXT
} PICO_OUTPUT_DRAW;

typedef struct {
    PICO_INPUT sub;
    union {
        int Delay;
        struct {
            int type;
            SDL_Event* ret;
        } Event;
        struct {
            int type;
            int timeout;
            SDL_Event* ret;
        } Event_Timeout;
    };
} Pico_Input;

typedef struct {
    PICO_OUTPUT sub;
    union {
        struct {
            PICO_OUTPUT_SET sub;
            union {
                struct {
                    PICO_OUTPUT_SET_COLOR sub;
                    union {
                        Pico_4i Clear;
                        Pico_4i Draw;
                    };
                } Color;
                struct {
                    char* file;
                    int height;
                } Font;
                struct {
                    Pico_2i win;
                    Pico_2i log;
                } Size;
                char* Title;
                Pico_2i Cursor;
            };
        } Set;
        struct {
            PICO_OUTPUT_DRAW sub;
            union {
                Pico_2i Pixel;
                struct {
                    Pico_2i pos;
                    const char* txt;
                } Text;
            };
        } Draw;
        char* Write;
        char* WriteLn;
    };
} Pico_Output;

void pico_init   ();
int  pico_input  (Pico_Input inp);
void pico_output (Pico_Output out);
