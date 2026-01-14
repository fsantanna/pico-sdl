/*
 * Visual Regression Testing for pico-sdl
 *
 * Single-header library for screenshot-based visual testing.
 * Tests render graphics and compare pixel-perfect screenshots against
 * expected images.
 *
 * COMPILATION:
 *   Define zero, one, or both of these when compiling:
 *
 *   -DPICO_CHECK_INT    Interactive mode (pause for visual inspection)
 *   -DPICO_CHECK_ASR    Assert mode (compare against expected images)
 *
 *   These can be combined for different behaviors:
 *     No defines:  Generate mode (write to out/, no pause, no assert)
 *     INT only:    Pause for inspection, no assertion
 *     ASR only:    Assert without pause (default for CI)
 *     Both:        Pause AND assert
 *
 *   Example:
 *     gcc -DPICO_CHECK_ASR test.c src/pico.c src/hash.c -Isrc -lSDL2 ...
 *     gcc -DPICO_CHECK_INT -DPICO_CHECK_ASR test.c ...
 *
 * USAGE:
 *   1. Include this file: #include "check.h"
 *   2. After rendering, call: _pico_check("file-01")
 *   3. Screenshots are always saved to tst/out/
 *   4. If ASR defined, compares with tst/asr/ reference images
 *
 * NAMING CONVENTION:
 *   Use format: filename-XX (e.g., "anchor_pct-01", "anchor_pct-02")
 *   - filename: matches the test file name
 *   - XX: sequential test number (01, 02, 03, ...)
 *
 * MODES:
 *
 *   Generate Mode (no defines):
 *     - Writes screenshots to out/ directory
 *     - No pause, no assertion
 *     - Use this to create new test baselines
 *     - Manually copy verified images from out/ to asr/
 *
 *   Interactive Mode (-DPICO_CHECK_INT):
 *     - Pauses before each check and waits for keypress
 *     - Useful for visually inspecting rendering
 *     - Does NOT assert unless ASR is also defined
 *     - This is the default when running ./pico-sdl
 *
 *   Assert Mode (-DPICO_CHECK_ASR):
 *     - Compares generated screenshots against asr/ reference images
 *     - Fails if pixels don't match exactly
 *     - Use this for automated testing and CI
 *     - No pause unless INT is also defined
 *
 *   Interactive + Assert (-DPICO_CHECK_INT -DPICO_CHECK_ASR):
 *     - Pauses for visual inspection AND asserts
 *     - Useful for verifying tests interactively
 *
 * WORKFLOW FOR CREATING REFERENCE IMAGES:
 *
 *   1. Run test in generate mode (disable INT):
 *        PICO_CHECK_INT= ./pico-sdl tst/newtest.c
 *
 *   2. Review images in tst/out/ directory
 *
 *   3. If correct, manually copy to asr/:
 *        cp tst/out/newtest-*.png tst/asr/
 *
 *   This workflow prevents accidental overwriting of reference images.
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
 *       pico_output_draw_rect_pct(&(Pico_Rect_Pct){
 *           0.5, 0.5, 0.4, 0.4, PICO_ANCHOR_C, NULL});
 *       _pico_check("anchor_pct-01");  // Writes to out/, compares with asr/
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

void _pico_check(const char *msg) {
    // Always write to out/
    char fmt_out[256];
    sprintf(fmt_out, "out/%s.png", msg);
    pico_output_screenshot(fmt_out);

    #ifdef PICO_CHECK_INT
    // Pause for visual inspection if INT is defined
    puts("interactive: press any key");
    pico_input_event(NULL, PICO_KEYDOWN);
    #endif

    #ifdef PICO_CHECK_ASR
    // Assert if ASR is defined
    char fmt_asr[256];
    sprintf(fmt_asr, "asr/%s.png", msg);

    SDL_Surface* sfc_out = IMG_Load(fmt_out);
    pico_assert(sfc_out != NULL);
    SDL_Surface* sfc_asr = IMG_Load(fmt_asr);
    pico_assert(sfc_asr != NULL);

    if (memcmp(sfc_out->pixels, sfc_asr->pixels,
               sfc_out->pitch * sfc_out->h) != 0) {
        printf("CHECK ERROR : files mismatch : %s --- %s\n",
               fmt_out, fmt_asr);
        exit(1);
    }

    SDL_FreeSurface(sfc_out);
    SDL_FreeSurface(sfc_asr);
    #endif
}

#endif // PICO_CHECK_H
