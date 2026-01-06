/*
 * Visual Regression Testing for pico-sdl
 *
 * Single-header library for screenshot-based visual testing.
 * Tests render graphics and compare pixel-perfect screenshots against expected images.
 *
 * COMPILATION:
 *   You MUST define exactly ONE of these when compiling:
 *
 *   -DPICO_CHECK_ASR    Assert mode (normal testing - default for CI)
 *   -DPICO_CHECK_GEN    Generate mode (create/update expected images)
 *   -DPICO_CHECK_INT    Interactive mode (pause for visual inspection)
 *
 *   Example:
 *     gcc -DPICO_CHECK_ASR test.c src/pico.c src/hash.c -Isrc -lSDL2 ...
 *
 * USAGE:
 *   1. Include this file: #include "check.h"
 *   2. After rendering, call: _pico_check("file-01")
 *   3. Screenshots are saved to tst/out/ and compared with tst/asr/
 *
 * NAMING CONVENTION:
 *   Use format: filename-XX (e.g., "anchor_pct-01", "anchor_pct-02")
 *   - filename: matches the test file name
 *   - XX: sequential test number (01, 02, 03, ...)
 *
 * MODES:
 *
 *   Interactive Mode (-DPICO_CHECK_INT) [DEFAULT]:
 *     - Pauses before each check and waits for keypress
 *     - Useful for visually inspecting rendering
 *     - Still performs assertion after viewing
 *     - This is the default when running ./pico-sdl
 *
 *   Assert Mode (-DPICO_CHECK_ASR):
 *     - Compares generated screenshots against asr/.*.png
 *     - Fails if pixels don't match exactly
 *     - Use this for automated testing and CI
 *
 *   Generate Mode (-DPICO_CHECK_GEN):
 *     - Creates/updates asr/.*.png files
 *     - Use this when you've verified output is correct
 *     - WARNING: Overwrites existing expected images
 *
 * HEADLESS TESTING WITH XVFB:
 *
 *   For CI/CD or headless environments without a display server, use Xvfb
 *   (X Virtual Frame Buffer) to provide a virtual display with ASR mode:
 *
 *     xvfb-run ./pico-sdl tst/anchor_pct.c
 *     make tests  # automatically uses ASR mode
 *
 *   Xvfb performs full graphical rendering in memory, producing real pixel
 *   data for screenshot comparison. This is essential for visual regression
 *   testing in automated environments.
 *
 * EXAMPLE:
 *   #include "pico.h"
 *   #include "check.h"
 *
 *   int main(void) {
 *       pico_init(1);
 *       pico_output_clear();
 *       pico_output_draw_rect_pct(&(Pico_Rect_Pct){0.5, 0.5, 0.4, 0.4, PICO_ANCHOR_C, NULL});
 *       _pico_check("anchor_pct-01");  // Compares with asr/anchor_pct-01.png
 *       pico_init(0);
 *       return 0;
 *   }
 */

#ifndef PICO_CHECK_H
#define PICO_CHECK_H

#include "pico.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>
#include <stdlib.h>
#include <string.h>

// Enforce exactly one mode is defined
#if defined(PICO_CHECK_ASR) + defined(PICO_CHECK_GEN) + defined(PICO_CHECK_INT) != 1
#error "Exactly ONE of PICO_CHECK_ASR, PICO_CHECK_GEN, or PICO_CHECK_INT must be defined"
#endif

void _pico_check(const char *msg);

#ifdef PICO_CHECK_GEN
void _pico_check(const char *msg) {
    char fmt[256];
    sprintf(fmt, "asr/%s.png", msg);
    pico_output_screenshot(fmt);
}
#endif

#if defined(PICO_CHECK_INT) || defined(PICO_CHECK_ASR)
static void _pico_check_assert(const char *msg) {
    char fmt1[256], fmt2[256];
    sprintf(fmt1, "out/%s.png", msg);
    sprintf(fmt2, "asr/%s.png", msg);
    pico_output_screenshot(fmt1);

    SDL_Surface *sfc1 = IMG_Load(fmt1);
    assert(sfc1 && "could not open out/ file");
    SDL_Surface *sfc2 = IMG_Load(fmt2);
    assert(sfc2 && "could not open asr/ file");
    if (memcmp(sfc1->pixels, sfc2->pixels, sfc1->pitch*sfc1->h) != 0) {
        printf("CHECK ERROR : files mismatch : %s --- %s\n", fmt1, fmt2);
        exit(1);
    }
    SDL_FreeSurface(sfc1);
    SDL_FreeSurface(sfc2);
}

#ifdef PICO_CHECK_INT
void _pico_check(const char *msg) {
    puts("interactive: press any key");
    pico_input_event(NULL, PICO_KEYDOWN);
    _pico_check_assert(msg);
}
#endif

#ifdef PICO_CHECK_ASR
void _pico_check(const char *msg) {
    _pico_check_assert(msg);
}
#endif
#endif

#endif // PICO_CHECK_H
