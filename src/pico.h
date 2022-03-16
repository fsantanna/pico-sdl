#include <assert.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

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
    Left=1, Center, Right
} HAnchor;

typedef enum {
    Bottom=1, Middle, Top
} VAnchor;

typedef enum {
    PICO_INPUT = 1,
    PICO_OUTPUT,
} PICO;

typedef enum {
    PICO_INPUT_DELAY = 1,
    PICO_INPUT_EVENT
} PICO_Input;

typedef enum {
    PICO_INPUT_EVENT_FOREVER = 1,
    PICO_INPUT_EVENT_TIMEOUT
} PICO_Input_Event;

typedef enum {
    PICO_OUTPUT_DRAW_PIXEL = 1,
    PICO_OUTPUT_DRAW_LINE,
    PICO_OUTPUT_DRAW_RECT,
    PICO_OUTPUT_DRAW_TEXT,
    PICO_OUTPUT_DRAW_IMAGE
} PICO_Output_Draw;

typedef enum {
    PICO_OUTPUT_GET_SIZE = 1,
} PICO_Output_Get;

typedef enum {
    PICO_OUTPUT_SET_COLOR_CLEAR = 1,
    PICO_OUTPUT_SET_COLOR_DRAW
} PICO_Output_Set_Color;

typedef enum {
    PICO_OUTPUT_SET_ANCHOR = 1,
    PICO_OUTPUT_SET_AUTO,
    PICO_OUTPUT_SET_COLOR,
    PICO_OUTPUT_SET_CURSOR,
    PICO_OUTPUT_SET_FONT,
    PICO_OUTPUT_SET_GRID,
    PICO_OUTPUT_SET_PAN,
    PICO_OUTPUT_SET_SIZE,
    PICO_OUTPUT_SET_TITLE,
    PICO_OUTPUT_SET_ZOOM,
} PICO_Output_Set;

typedef enum {
    PICO_OUTPUT_WRITE_NORM = 1,
    PICO_OUTPUT_WRITE_LINE
} PICO_Output_Write;

typedef enum {
    PICO_OUTPUT_PRESENT = 1,
    PICO_OUTPUT_CLEAR,
    PICO_OUTPUT_DRAW,
    PICO_OUTPUT_GET,
    PICO_OUTPUT_SET,
    PICO_OUTPUT_WRITE
} PICO_Output;

typedef struct {
    union {
        // INPUT
        struct {
            union {
                int Delay;
                struct {
                    union {
                        int type;
                        struct {
                            int type;
                            int timeout;
                        } Timeout;
                    };
                    int tag;
                } Event;
            };
            PICO_Input tag;
        } Input;

        // OUTPUT
        struct {
            union {
                struct {
                    union {
                        struct {
                            Pico_2i p1;
                            Pico_2i p2;
                        } Line;
                        Pico_2i Pixel;
                        struct {
                            Pico_2i pos;
                            Pico_2i size;
                        } Rect;
                        struct {
                            Pico_2i pos;
                            const char* txt;
                        } Text;
                        struct {
                            Pico_2i pos;
                            const char* path;
                        } Image;
                    };
                    PICO_Output_Draw tag;
                } Draw;
                struct {
                    union {
                        Pico_2i* Size;
                    };
                    PICO_Output_Get tag;
                } Get;
                struct {
                    union {
                        Pico_2i Anchor;
                        int     Auto;
                        struct {
                            union {
                                Pico_4i Clear;
                                Pico_4i Draw;
                            };
                            PICO_Output_Set_Color tag;
                        } Color;
                        Pico_2i Cursor;
                        int     Grid;
                        struct {
                            char* file;
                            int height;
                        } Font;
                        Pico_2i Pan;
                        Pico_2i Size;
                        char* Title;
                        Pico_2i Zoom;
                    };
                    PICO_Output_Set tag;
                } Set;
                struct {
                    union {
                        char* Norm;
                        char* Line;
                    };
                    PICO_Output_Write tag;
                } Write;
            };
            PICO_Output tag;
        } Output;
    };

    PICO tag;
} Pico;

void pico_open   ();
void pico_close  ();
int pico_input   (SDL_Event* out, Pico inp);
void pico_output (Pico out);
