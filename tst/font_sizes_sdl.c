// Pure SDL2 + SDL2_ttf reference: draw the same size ladder as
// tst/font_sizes.c, but rasterizing each line's glyphs DIRECTLY at the
// correct pixel size (TTF_OpenFont ptsize == on-screen height) and blitting
// 1:1. No upscaling -> every size stays crisp and readable.
//
// Build:
//   gcc tst/font_sizes_sdl.c -lSDL2 -lSDL2_ttf -lSDL2_image -o /tmp/fs_sdl
// Run from the tst/ directory (paths below are relative to it), headless:
//   cd tst && SDL_AUDIODRIVER=dummy xvfb-run -a /tmp/fs_sdl
// Writes tst/out/font_sizes_sdl.png

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#define W 500
#define H 500
#define FONT "../res/DejaVuSans.ttf"
#define TEXT "Reading 0123 mq"

// on-screen pixel heights, same ladder as tst/font_sizes.c
static const int SIZES[] = { 8, 10, 12, 16, 20, 28, 40, 60 };
#define N (int)(sizeof(SIZES)/sizeof(SIZES[0]))

int main (void) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    if (TTF_Init() != 0) {
        fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
        return 1;
    }

    SDL_Window*   win = SDL_CreateWindow("font sizes (sdl)",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, W, H, SDL_WINDOW_SHOWN);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);

    // black background
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);

    SDL_Color white = { 255, 255, 255, 255 };
    int y = 4;
    for (int i = 0; i < N; i++) {
        int px = SIZES[i];

        // KEY: open the font at the exact pixel height we will draw at, so
        // the glyph is rasterized natively -- no stretch.
        TTF_Font* font = TTF_OpenFont(FONT, px);
        if (font == NULL) {
            fprintf(stderr, "TTF_OpenFont(%s, %d): %s\n", FONT, px, TTF_GetError());
            return 1;
        }

        SDL_Surface* sfc = TTF_RenderText_Blended(font, TEXT, white);
        SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, sfc);

        SDL_Rect dst = { 6, y, sfc->w, sfc->h };   // 1:1, no scaling
        SDL_RenderCopy(ren, tex, NULL, &dst);

        y += sfc->h + 4;
        SDL_DestroyTexture(tex);
        SDL_FreeSurface(sfc);
        TTF_CloseFont(font);
    }

    SDL_RenderPresent(ren);

    // screenshot
    SDL_Surface* shot = SDL_CreateRGBSurfaceWithFormat(0, W, H, 32, SDL_PIXELFORMAT_RGBA32);
    SDL_RenderReadPixels(ren, NULL, SDL_PIXELFORMAT_RGBA32, shot->pixels, shot->pitch);
    if (IMG_SavePNG(shot, "out/font_sizes_sdl.png") != 0) {
        fprintf(stderr, "IMG_SavePNG: %s\n", IMG_GetError());
    } else {
        printf("wrote out/font_sizes_sdl.png\n");
    }
    SDL_FreeSurface(shot);

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
