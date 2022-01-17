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
    Left, Center, Right
} HAnchor;

typedef enum {
    Top, Middle, Bottom
} VAnchor;

typedef enum {
    // INPUT

    PICO_DELAY,
    PICO_EVENT,
    PICO_EVENT_TIMEOUT,

    // OUTPUT

    PICO_CLEAR,

    PICO_DRAW_LINE,
    PICO_DRAW_PIXEL,
    PICO_DRAW_RECT,
    PICO_DRAW_TEXT,

    PICO_GET_SIZE,

    PICO_PRESENT,

    PICO_SET_ANCHOR,
    PICO_SET_AUTO,
    PICO_SET_COLOR_CLEAR,
    PICO_SET_COLOR_DRAW,
    PICO_SET_CURSOR,
    PICO_SET_FONT,
    PICO_SET_GRID,
    PICO_SET_PAN,
    PICO_SET_SIZE,
    PICO_SET_TITLE,
    PICO_SET_ZOOM,

    PICO_WRITE,
    PICO_WRITELN
} PICO_IO;

typedef struct {
    PICO_IO tag;
    union {
        // INPUT

        int Delay;
        struct {
            int type;
            SDL_Event* ret;
        } Event;
        struct {
            int type;
            SDL_Event* ret;
            int timeout;
        } Event_Timeout;

        // OUTPUT

        struct {
            Pico_2i p1;
            Pico_2i p2;
        } Draw_Line;
        Pico_2i Draw_Pixel;
        struct {
            Pico_2i pos;
            Pico_2i size;
        } Draw_Rect;
        struct {
            Pico_2i pos;
            const char* txt;
        } Draw_Text;

        Pico_2i* Get_Size;

        Pico_2i Set_Anchor;
        int     Set_Auto;
        Pico_4i Set_Color_Clear;
        Pico_4i Set_Color_Draw;
        Pico_2i Set_Cursor;
        int     Set_Grid;
        struct {
            char* file;
            int height;
        } Set_Font;
        Pico_2i Set_Pan;
        Pico_2i Set_Size;
        char* Set_Title;
        Pico_2i Set_Zoom;

        char* Write;
        char* WriteLn;
    };
} Pico_IO;

void pico_open   ();
void pico_close  ();
int  pico_input  (Pico_IO inp);
void pico_output (Pico_IO out);
