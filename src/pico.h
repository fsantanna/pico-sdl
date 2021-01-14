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
    // INPUT

    PICO_DELAY,
    PICO_EVENT,
    PICO_EVENT_TIMEOUT,

    // OUTPUT

    PICO_CLEAR,

    PICO_DRAW_PIXEL,
    PICO_DRAW_TEXT,

    PICO_GET_SIZE,

    PICO_SET_COLOR_CLEAR,
    PICO_SET_COLOR_DRAW,
    PICO_SET_CURSOR,
    PICO_SET_FONT,
    PICO_SET_SIZE,
    PICO_SET_TITLE,

    PICO_WRITE,
    PICO_WRITELN
} PICO_IO;

typedef struct {
    PICO_IO sub;
    union {
        // INPUT

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

        // OUTPUT

        Pico_2i Draw_Pixel;
        struct {
            Pico_2i pos;
            const char* txt;
        } Draw_Text;

        struct {
            Pico_2i* phy;
            Pico_2i* log;
        } Get_Size;

        Pico_4i Set_Color_Clear;
        Pico_4i Set_Color_Draw;
        Pico_2i Set_Cursor;
        struct {
            char* file;
            int height;
        } Set_Font;
        struct {
            Pico_2i phy;
            Pico_2i log;
        } Set_Size;
        char* Set_Title;

        char* Write;
        char* WriteLn;
    };
} Pico_IO;

void pico_init   ();
int  pico_input  (Pico_IO inp);
void pico_output (Pico_IO out);
