#include <stdio.h>
#include <SDL2/SDL.h>

static SDL_Window*  WIN;
static SDL_Texture* TEX;
static SDL_Texture* tex;
static int alpha;

#define REN (SDL_GetRenderer(WIN))

static void out0 () {
    SDL_SetRenderTarget(REN, NULL);
    SDL_RenderCopy(REN, TEX, NULL, NULL);
    SDL_RenderPresent(REN);
    SDL_SetRenderTarget(REN, TEX);
}

static void out1 () {
    SDL_SetRenderTarget(REN, TEX);
    SDL_Rect dst = { 32, 18, 320, 180 };
    SDL_SetTextureAlphaMod(tex, alpha);
printf(">>> [%d] %d\n", 1, alpha);
    SDL_RenderCopy(REN, tex, NULL, &dst);
    //SDL_SetTextureAlphaMod(tex, 0xFF);

    {
        SDL_SetRenderTarget(REN, NULL);
        SDL_RenderCopy(REN, TEX, NULL, NULL);
        SDL_RenderPresent(REN);
        SDL_SetRenderTarget(REN, TEX);
    }

    SDL_SetRenderTarget(REN, tex);
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

    SDL_SetRenderDrawColor(REN, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(REN);
    out0();

    tex = SDL_CreateTexture (
        REN, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
        320, 180
    );
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(REN, tex);

    SDL_SetRenderDrawColor(REN, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(REN);
    out1();
    SDL_Delay(250);

    SDL_Rect r2 = { 80,45,160,90 };

    SDL_SetRenderDrawColor(REN, 0xFF, 0x00, 0x00, 0xFF);
    SDL_RenderFillRect(REN, &r2);
    alpha = 0xFF;
    out1();

    SDL_Delay(250);

    SDL_SetRenderDrawColor(REN, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(REN);
    out0();

    alpha = 0x88;
    out1();
    SDL_Delay(250);
    out1();
    SDL_Delay(250);
    out1();
    SDL_Delay(250);
    out1();
    SDL_Delay(250);
    out1();
    SDL_Delay(250);
    out1();
    SDL_Delay(250);

    return 0;
}
