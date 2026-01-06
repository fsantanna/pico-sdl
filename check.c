/*
 * Visual Regression Testing Utilities for pico-sdl
 *
 * This file provides screenshot-based testing for visual output.
 * Tests render graphics and compare pixel-perfect screenshots against expected images.
 *
 * USAGE:
 *   1. Include this file in your test: #include "check.c"
 *   2. After rendering, call: _pico_check("test_name")
 *   3. Screenshots are saved to tst/output/ and compared with tst/expected/
 *
 * HOW IT WORKS:
 *   - Takes a screenshot of current rendering
 *   - Compares it pixel-by-pixel with expected image
 *   - Test fails if any pixel differs
 *
 * MODES (via environment variables):
 *
 *   Normal mode (default):
 *     make tests
 *     - Compares output screenshots against expected/*.png
 *     - Fails if pixels don't match exactly
 *
 *   Generate mode:
 *     PICO_CHECK_GENERATE=1 make tests
 *     - Creates/updates expected/*.png files
 *     - Use this when you've verified output is correct and want to save it
 *
 *   Debug mode:
 *     PICO_CHECK_DEBUG=1 make tests
 *     - Pauses before each check and waits for keypress
 *     - Useful for visually inspecting rendering
 *
 * EXAMPLE:
 *   pico_output_clear();
 *   pico_output_draw_rect_pct(&(Pico_Rect_Pct){0.5, 0.5, 0.4, 0.4, PICO_ANCHOR_C, NULL});
 *   _pico_check("centered_rect");  // Compares with expected/centered_rect.png
 */

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
    assert(sfc1 && "could not open output file");
    SDL_Surface *sfc2 = IMG_Load(fmt2);
    assert(sfc2 && "could not open expected file");
    assert(memcmp(sfc1->pixels, sfc2->pixels, sfc1->pitch*sfc1->h) == 0);
    SDL_FreeSurface(sfc1);
    SDL_FreeSurface(sfc2);
}

void _pico_check (const char *msg) {
    if (getenv("PICO_CHECK_DEBUG")) {
        puts("debug: press any key");
        pico_input_event(NULL, PICO_KEYDOWN);
    }

    if (getenv("PICO_CHECK_GENERATE")) {
        _pico_check_generate(msg);
    } else {
        _pico_check_assert(msg);
    }
}
