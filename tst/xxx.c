#include <stdio.h>
#include <SDL2/SDL.h>

typedef SDL_Point Pico_Pos;
typedef SDL_Point Pico_Dim;
typedef SDL_Rect  Pico_Rect;
typedef SDL_Color Pico_Color;
typedef SDL_Point Pico_Pct;

static SDL_Window*  WIN;
static SDL_Texture* TEX;

#define REN (SDL_GetRenderer(WIN))

typedef struct Pico_Ctx {
    int          alpha;
    Pico_Dim     dim;
    Pico_Pos     pos;
    SDL_Texture* tex;
} Pico_Ctx;

Pico_Ctx CTX = {
    0xFF,
    {0,0},
    {0, 0},
    NULL,
};

static void out0 () {
    SDL_SetRenderTarget(REN, NULL);
    SDL_RenderCopy(REN, TEX, NULL, NULL);
    SDL_RenderPresent(REN);
    SDL_SetRenderTarget(REN, TEX);
}

static void out1 () {
    SDL_SetRenderTarget(REN, TEX);
    SDL_Rect dst = { CTX.pos.x, CTX.pos.y, CTX.dim.x, CTX.dim.y };
    SDL_SetTextureAlphaMod(CTX.tex, CTX.alpha);
printf(">>> [%d] %d\n", 1, CTX.alpha);
    SDL_RenderCopy(REN, CTX.tex, NULL, &dst);
    //SDL_SetTextureAlphaMod(CTX.tex, 0xFF);
    out0();
    SDL_SetRenderTarget(REN, CTX.tex);
}

int main (void) {
    SDL_Init(SDL_INIT_VIDEO);
    WIN = SDL_CreateWindow (
        "XXX", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        640, 320,
        (SDL_WINDOW_SHOWN)
    );
    SDL_CreateRenderer(WIN, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(REN, SDL_BLENDMODE_BLEND);
    TEX = SDL_CreateTexture (
        REN, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
        640, 360
    );
    SDL_SetTextureBlendMode(TEX, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(REN, TEX);

    puts("rect pos=30, dim=50");
    SDL_SetRenderDrawColor(REN, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(REN);
    out0();
    Pico_Rect r1 = { 192,108,320,180 };

    CTX.dim = (Pico_Dim){r1.w,r1.h};
    CTX.tex = SDL_CreateTexture (
        REN, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
        CTX.dim.x, CTX.dim.y
    );
    SDL_SetTextureBlendMode(CTX.tex, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(REN, CTX.tex);

    CTX.pos = (Pico_Pos) { 32, 18 };
    SDL_SetRenderDrawColor(REN, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(REN);
    out1();
    SDL_Delay(500);

    puts("red centered under white");
    Pico_Rect r2 = { 80,45,160,90 };

    SDL_SetRenderDrawColor(REN, 0xFF, 0x00, 0x00, 0xFF);

    SDL_RenderFillRect(REN, &r2);
    out1();

    SDL_Delay(500);

    SDL_SetRenderDrawColor(REN, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(REN);
    out0();

    CTX.alpha = 0x88;
    out1();
    SDL_Delay(500);
    out1();
    SDL_Delay(500);
    out1();
    SDL_Delay(500);

    return 0;
}
