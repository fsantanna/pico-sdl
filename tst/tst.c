#include "pico.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>
#include <stdlib.h>
#include <string.h>

void _pico_check_generate (const char *msg) {
    char fmt[256] = "";
    sprintf(fmt, "expected/%s.png", msg);
    pico_output_screenshot(fmt);
}

void _pico_check_assert (const char *msg) {
    char fmt1[256] = "", fmt2[256] = "";
    sprintf(fmt1, "output/%s.png", msg);
    sprintf(fmt2, "expected/%s.png", msg);
    pico_output_screenshot(fmt1);

    SDL_Surface *sfc1 = IMG_Load(fmt1);
    assert(sfc1 && "could not open expected file");
    SDL_Surface *sfc2 = IMG_Load(fmt2);
    assert(sfc2 && "could not open output file");
    assert(memcmp(sfc1->pixels, sfc2->pixels, sfc1->pitch*sfc1->h) == 0);
    SDL_FreeSurface(sfc1);
    SDL_FreeSurface(sfc2);
}

void _pico_check (const char *msg) {
    if (getenv("PICO_CHECK_GENERATE")) {
        _pico_check_generate(msg);
    } else {
        _pico_check_assert(msg);
    }

    if (getenv("PICO_CHECK_DEBUG")) {
        puts("debug: press any key");
        pico_input_event(NULL, PICO_KEYDOWN);
    }
}