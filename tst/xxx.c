#include <stdio.h>
#include <assert.h>
#include <SDL2/SDL.h>

#define PICO_TITLE "pico-SDL"
#define PICO_DIM_PHY ((Pico_Dim) {640,360})
#define PICO_DIM_LOG ((Pico_Dim) {640,360})
#define PICO_HASH  128

typedef SDL_Point Pico_Pos;
typedef SDL_Point Pico_Dim;
typedef SDL_Rect  Pico_Rect;
typedef SDL_Color Pico_Color;
typedef SDL_Point Pico_Anchor;
typedef SDL_Point Pico_Flip;
typedef SDL_Point Pico_Pct;

#define PICO_LEFT   0
#define PICO_CENTER 0
#define PICO_RIGHT  0
#define PICO_TOP    0
#define PICO_MIDDLE 0
#define PICO_BOTTOM 0

#define PICO_DIM_KEEP ((Pico_Dim) {0,0})
#define PICO_CLIP_RESET ((Pico_Rect) {0,0,0,0})

#define pico_assert(x) if (!(x)) { fprintf(stderr,"%s\n",SDL_GetError()); assert(0 && "SDL ERROR"); }

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <stdarg.h>

#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include <assert.h>
#include <string.h>

#include <stdlib.h>
#include <stddef.h>

static SDL_Window*  WIN;

#define REN (SDL_GetRenderer(WIN))

typedef struct Pico_Ctx {
    int          alpha;
    struct {
        Pico_Dim phy;
        Pico_Dim log;
    }            dim;
    Pico_Pos     pos;
    SDL_Texture* tex;
    Pico_Pct     zoom;
} Pico_Ctx;

Pico_Ctx _ctx = {
    0xFF,
    { PICO_DIM_PHY, PICO_DIM_LOG },
    {0, 0},
    NULL,
    {100, 100},
};

Pico_Ctx* CTX = &_ctx;

static void _pico_output_present (int force, Pico_Ctx* ctx) {
    SDL_Rect clip;
    SDL_RenderGetClipRect(REN, &clip);

    SDL_Texture* up = (ctx == &_ctx) ? NULL : _ctx.tex;

    SDL_SetRenderTarget(REN, up);
    SDL_RenderSetLogicalSize(REN, _ctx.dim.phy.x, _ctx.dim.phy.y);

    SDL_Rect dst = { ctx->pos.x, ctx->pos.y, ctx->dim.phy.x, ctx->dim.phy.y };
    SDL_SetTextureAlphaMod(ctx->tex, ctx->alpha);
printf(">>> [%d] %d\n", ctx==&_ctx, ctx->alpha);
    SDL_RenderCopy(REN, ctx->tex, NULL, &dst);
    //SDL_SetTextureAlphaMod(ctx->tex, 0xFF);

    if (ctx == &_ctx) {
        SDL_RenderPresent(REN);
    }

    if (ctx != &_ctx) {
        _pico_output_present(force, &_ctx);
    }

    Pico_Dim Z = ctx->dim.log;
    SDL_RenderSetLogicalSize(REN, Z.x, Z.y);
    SDL_SetRenderTarget(REN, ctx->tex);
    SDL_RenderSetClipRect(REN, &clip);
}

void pico_output_present (void) {
    _pico_output_present(1, &_ctx);
}

void pico_set_zoom (Pico_Pct pct) {
    Pico_Dim old = CTX->dim.log;
    CTX->zoom = pct;
    Pico_Dim new = CTX->dim.log;
    
    int dx = new.x - old.x;
    int dy = new.y - old.y;

    SDL_DestroyTexture(CTX->tex);
    CTX->tex = SDL_CreateTexture (
        REN, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
        new.x, new.y
    );
    pico_assert(CTX->tex != NULL);
    SDL_SetTextureBlendMode(CTX->tex, SDL_BLENDMODE_BLEND);
    SDL_RenderSetLogicalSize(REN, new.x, new.y);
    SDL_SetRenderTarget(REN, CTX->tex);

    // TODO: need to init w/ explicit SetClip to save w/h
    //       do not pass NULL, GetClip would also return w=0,h=0
    SDL_Rect clip = { 0, 0, new.x, new.y };
    SDL_RenderSetClipRect(REN, &clip);
}

int main (void) {
    {
        pico_assert(0 == SDL_Init(SDL_INIT_VIDEO));
        WIN = SDL_CreateWindow (
            PICO_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            CTX->dim.phy.x, CTX->dim.phy.y,
            (SDL_WINDOW_SHOWN)
        );
        pico_assert(WIN != NULL);
        SDL_CreateRenderer(WIN, -1, SDL_RENDERER_ACCELERATED);
        pico_assert(REN != NULL);
        SDL_SetRenderDrawBlendMode(REN, SDL_BLENDMODE_BLEND);
        pico_set_zoom(CTX->zoom);
    }

    puts("rect pos=30, dim=50");
    SDL_SetRenderDrawColor(REN, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(REN);
    _pico_output_present(0, CTX);
    Pico_Rect r1 = { 192,108,320,180 };

    Pico_Ctx ctx = {
        0xFF,
        { {0,0}, {0,0} },
        {0, 0},
        NULL,
        {100, 100},
    };
    CTX = &ctx;

    CTX->dim.phy = (Pico_Dim){r1.w,r1.h};
    CTX->dim.log = (Pico_Dim){r1.w,r1.h};
    pico_set_zoom(CTX->zoom);

    CTX->pos = (Pico_Pos) { 32, 18 };
    SDL_SetRenderDrawColor(REN, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(REN);
    _pico_output_present(0, CTX);
    SDL_Delay(500);

    puts("red centered under white");
    Pico_Rect r2 = { 80,45,160,90 };

    SDL_SetRenderDrawColor(REN, 0xFF, 0x00, 0x00, 0xFF);

    SDL_RenderFillRect(REN, &r2);
    _pico_output_present(0, CTX);

    SDL_Delay(500);

    CTX = &_ctx;

    SDL_SetRenderDrawColor(REN, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(REN);
    _pico_output_present(0, CTX);

    CTX = &ctx;

    CTX->alpha = 0x88;
    _pico_output_present(0, CTX);
    SDL_Delay(500);
    _pico_output_present(0, CTX);
    SDL_Delay(500);
    _pico_output_present(0, CTX);
    SDL_Delay(500);

    return 0;
}
