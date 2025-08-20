#include "pico.h"
#include <SDL2/SDL_image.h>
#include <string.h>

void _pico_gen_output(const char *msg) {
    char fmt[256] = "";
    sprintf(fmt, "expected/%s.png", msg);
    pico_output_screenshot(fmt);
}

void _pico_assert_output(const char *msg) {
    char fmt1[256] = "", fmt2[256] = "";
    sprintf(fmt1, "out/%s.png", msg);
    sprintf(fmt2, "expected/%s.png", msg);

    pico_output_screenshot(fmt1);
    SDL_Surface *sfc1 = IMG_Load(fmt1);
    SDL_Surface *sfc2 = IMG_Load(fmt2);
    assert(memcmp(sfc1->pixels, sfc2->pixels, sfc1->pitch*sfc1->h) == 0);
}
