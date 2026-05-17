#ifndef PICO_STATE_H
#define PICO_STATE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "realm.hc"
#include "pico.h"
#include "layers.h"

typedef struct PicoState {
    int           init;
    realm_t*      realm;
    Pico_Layer    world;
    Pico_Layer*   layer;
    struct {
        int on;
        int fps;
        int ms;
        int t0;
    } expert;
    struct {
        SDL_Window*   win;
        SDL_Renderer* ren;
        Pico_Layer    layer;
        struct {
            int fs;
            int out;
        } ing;
        struct { int fs; } pub;
    } window;
} PicoState;

extern PicoState G;

///////////////////////////////////////////////////////////////////////////////
// internal pico core (defined in pico.c)
///////////////////////////////////////////////////////////////////////////////

void         _pico_guard          (void);
SDL_Texture* _tex_create          (Pico_Abs_Dim dim);
TTF_Font*    _font_get            (const char* path, int h);
void         _pico_output_present (int force);

#endif
