#include <stdio.h>
#include <SDL2/SDL.h>

typedef SDL_Point Pico_Pos;
typedef SDL_Point Pico_Dim;
typedef SDL_Rect  Pico_Rect;
typedef SDL_Color Pico_Color;
typedef SDL_Point Pico_Pct;

static SDL_Window* WIN;

#define REN (SDL_GetRenderer(WIN))

typedef struct Pico_Ctx {
    int          alpha;
    Pico_Dim     dim;
    Pico_Pos     pos;
    SDL_Texture* tex;
} Pico_Ctx;

Pico_Ctx _ctx = {
    0xFF,
    {640,360},
    {0, 0},
    NULL,
};

Pico_Ctx* CTX = &_ctx;

static void out (Pico_Ctx* ctx) {
    SDL_Texture* up = (ctx == &_ctx) ? NULL : _ctx.tex;
    SDL_SetRenderTarget(REN, up);
    SDL_Rect dst = { ctx->pos.x, ctx->pos.y, ctx->dim.x, ctx->dim.y };
    SDL_SetTextureAlphaMod(ctx->tex, ctx->alpha);
printf(">>> [%d] %d\n", ctx==&_ctx, ctx->alpha);
    SDL_RenderCopy(REN, ctx->tex, NULL, &dst);
    //SDL_SetTextureAlphaMod(ctx->tex, 0xFF);
    if (ctx == &_ctx) {
        SDL_RenderPresent(REN);
    }
    if (ctx != &_ctx) {
        out(&_ctx);
    }
    Pico_Dim Z = ctx->dim;
    SDL_SetRenderTarget(REN, ctx->tex);
}

void pico_set_zoom () {
    SDL_DestroyTexture(CTX->tex);
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
    CTX->tex = SDL_CreateTexture (
        REN, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
        CTX->dim.x, CTX->dim.y
    );
    SDL_SetTextureBlendMode(CTX->tex, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(REN, CTX->tex);

    puts("rect pos=30, dim=50");
    SDL_SetRenderDrawColor(REN, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(REN);
    out(CTX);
    Pico_Rect r1 = { 192,108,320,180 };

    Pico_Ctx ctx = {
        0xFF,
        {0,0},
        {0, 0},
        NULL,
    };
    CTX = &ctx;

    CTX->dim = (Pico_Dim){r1.w,r1.h};
    CTX->tex = SDL_CreateTexture (
        REN, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
        CTX->dim.x, CTX->dim.y
    );
    SDL_SetTextureBlendMode(CTX->tex, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(REN, CTX->tex);

    CTX->pos = (Pico_Pos) { 32, 18 };
    SDL_SetRenderDrawColor(REN, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(REN);
    out(CTX);
    SDL_Delay(500);

    puts("red centered under white");
    Pico_Rect r2 = { 80,45,160,90 };

    SDL_SetRenderDrawColor(REN, 0xFF, 0x00, 0x00, 0xFF);

    SDL_RenderFillRect(REN, &r2);
    out(CTX);

    SDL_Delay(500);

    CTX = &_ctx;

    SDL_SetRenderDrawColor(REN, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(REN);
    out(CTX);

    CTX = &ctx;

    CTX->alpha = 0x88;
    out(CTX);
    SDL_Delay(500);
    out(CTX);
    SDL_Delay(500);
    out(CTX);
    SDL_Delay(500);

    return 0;
}
