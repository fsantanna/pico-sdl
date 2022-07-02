#include <assert.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#define _TITLE_ "pico-SDL"
#define _WIN_ 510
#define pico_assert(x) if (!(x)) { fprintf(stderr,"%s\n",SDL_GetError()); assert(0 && "SDL ERROR"); }

#define SDL_ANY 0

#define pico_input_event_poll(inp,evt) \
    pico_input(inp, (Pico_Input){ .tag=PICO_INPUT_EVENT, .Event={.tag=PICO_INPUT_EVENT_POLL,.type=evt} });

#define pico_output_set_auto(v) \
    pico_output((Pico_Output) { .tag=PICO_OUTPUT_SET, .Set={.tag=PICO_OUTPUT_SET_AUTO,.Auto=v} });
#define pico_output_set_color_clear_rgb(r,g,b) \
    pico_output((Pico_Output) { .tag=PICO_OUTPUT_SET, .Set={.tag=PICO_OUTPUT_SET_COLOR,.Color={.tag=PICO_OUTPUT_SET_COLOR_CLEAR,.Clear=(Pico_4i){r,g,b,0xFF}}} });
#define pico_output_set_color_draw_rgb(r,g,b) \
    pico_output((Pico_Output) { .tag=PICO_OUTPUT_SET, .Set={.tag=PICO_OUTPUT_SET_COLOR,.Color={.tag=PICO_OUTPUT_SET_COLOR_DRAW,.Draw=(Pico_4i){r,g,b,0xFF}}} });
#define pico_output_set_grid(v) \
    pico_output((Pico_Output) { .tag=PICO_OUTPUT_SET, .Set={.tag=PICO_OUTPUT_SET_GRID,.Grid=v} });
#define pico_output_set_image_crop(rct) \
    pico_output((Pico_Output) { .tag=PICO_OUTPUT_SET, .Set={.tag=PICO_OUTPUT_SET_IMAGE,.Image={.tag=PICO_OUTPUT_SET_IMAGE_CROP,.Crop=rct}} });
#define pico_output_set_image_crop_xywh(x,y,w,h) \
    pico_output((Pico_Output) { .tag=PICO_OUTPUT_SET, .Set={.tag=PICO_OUTPUT_SET_IMAGE,.Image={.tag=PICO_OUTPUT_SET_IMAGE_CROP,.Crop=(Pico_4i){x,y,w,h}}} });
#define pico_output_set_image_size_wh(w,h) \
    pico_output((Pico_Output) { .tag=PICO_OUTPUT_SET, .Set={.tag=PICO_OUTPUT_SET_IMAGE,.Image={.tag=PICO_OUTPUT_SET_IMAGE_SIZE,.Size=(Pico_2i){w,h}}} });
#define pico_output_set_size_wh(w,h) \
    pico_output((Pico_Output) { .tag=PICO_OUTPUT_SET, .Set={.tag=PICO_OUTPUT_SET_SIZE,.Size=(Pico_2i){w,h}} });
#define pico_output_set_pixel_wh(w,h) \
    pico_output((Pico_Output) { .tag=PICO_OUTPUT_SET, .Set={.tag=PICO_OUTPUT_SET_PIXEL,.Pixel=(Pico_2i){w,h}} });

#define pico_output_clear() \
    pico_output((Pico_Output) { .tag=PICO_OUTPUT_CLEAR })
#define pico_output_present() \
    pico_output((Pico_Output) { .tag=PICO_OUTPUT_PRESENT })
#define pico_output_draw_pixel(v) \
    pico_output((Pico_Output) { .tag=PICO_OUTPUT_DRAW, .Draw={.tag=PICO_OUTPUT_DRAW_PIXEL, .Pixel=v}})
#define pico_output_draw_pixel_xy(x,y) \
    pico_output((Pico_Output) { .tag=PICO_OUTPUT_DRAW, .Draw={.tag=PICO_OUTPUT_DRAW_PIXEL, .Pixel=(Pico_2i){x,y}}})
#define pico_output_draw_rect(pos,dim) \
    pico_output((Pico_Output) { .tag=PICO_OUTPUT_DRAW, .Draw={.tag=PICO_OUTPUT_DRAW_RECT, .Rect={pos,dim}}})
#define pico_output_draw_rect_xywh(x,y,w,h) \
    pico_output((Pico_Output) { .tag=PICO_OUTPUT_DRAW, .Draw={.tag=PICO_OUTPUT_DRAW_RECT, .Rect={{x,y},{w,h}}}})
#define pico_output_draw_image(pos,path) \
    pico_output((Pico_Output) { .tag=PICO_OUTPUT_DRAW, .Draw={.tag=PICO_OUTPUT_DRAW_IMAGE, .Image={pos,path}}})

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
    PICO_INPUT_DELAY = 1,
    PICO_INPUT_EVENT
} PICO_Input;

typedef enum {
    PICO_INPUT_EVENT_FOREVER = 1,
    PICO_INPUT_EVENT_TIMEOUT,
    PICO_INPUT_EVENT_POLL
} PICO_Input_Event;

typedef enum {
    PICO_OUTPUT_DRAW_PIXEL = 1,
    PICO_OUTPUT_DRAW_LINE,
    PICO_OUTPUT_DRAW_RECT,
    PICO_OUTPUT_DRAW_TEXT,
    PICO_OUTPUT_DRAW_IMAGE
} PICO_Output_Draw;

typedef enum {
    PICO_OUTPUT_GET_SIZE_WINDOW = 1,
    PICO_OUTPUT_GET_SIZE_IMAGE
} PICO_Output_Get_Size;

typedef enum {
    PICO_OUTPUT_GET_SIZE = 1
} PICO_Output_Get;

typedef enum {
    PICO_OUTPUT_SET_COLOR_CLEAR = 1,
    PICO_OUTPUT_SET_COLOR_DRAW
} PICO_Output_Set_Color;

typedef enum {
    PICO_OUTPUT_SET_IMAGE_CROP = 1,
    PICO_OUTPUT_SET_IMAGE_SIZE
} PICO_Output_Set_Image;

typedef enum {
    PICO_OUTPUT_SET_ANCHOR = 1,
    PICO_OUTPUT_SET_AUTO,
    PICO_OUTPUT_SET_COLOR,
    PICO_OUTPUT_SET_CURSOR,
    PICO_OUTPUT_SET_FONT,
    PICO_OUTPUT_SET_GRID,
    PICO_OUTPUT_SET_IMAGE,
    PICO_OUTPUT_SET_PAN,
    PICO_OUTPUT_SET_PIXEL,
    PICO_OUTPUT_SET_SIZE,
    PICO_OUTPUT_SET_TITLE,
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

typedef struct Pico_Input {
    union {
        int Delay;
        struct {
            union {
                int type;   // TODO: type -> Forever
                struct {
                    int type;
                    int timeout;
                } Timeout;
            };
            int tag;
        } Event;
    };
    PICO_Input tag;
} Pico_Input;

typedef struct Pico_Output {
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
                struct {
                    union {
                        Pico_2i* Window;
                        struct {
                            Pico_2i* size;
                            char* path;
                        } Image;
                    };
                    PICO_Output_Get_Size tag;
                } Size;
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
                struct {
                    char* file;
                    int height;
                } Font;
                int     Grid;
                struct {
                    union {
                        Pico_4i Crop;
                        Pico_2i Size;
                    };
                    PICO_Output_Set_Image tag;
                } Image;
                Pico_2i Pan;
                Pico_2i Pixel;
                Pico_2i Size;
                char* Title;
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
} Pico_Output;

void pico_open   ();
void pico_close  ();
int pico_input   (SDL_Event* out, Pico_Input inp);
void pico_output (Pico_Output out);
int pico_isPointVsRect (Pico_2i pt, Pico_4i r);
