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
#include <stdio.h>
#include <string.h>

int _pico_cmp_files (const char* path1, const char* path2) {
    int ok = 1;

    FILE* f1 = fopen(path1, "rb");
    if (f1 == NULL) return 0;
    FILE* f2 = fopen(path2, "rb");
    if (f2 == NULL) {
        fclose(f1);
        return 0;
    }

    fseek(f1, 0, SEEK_END);
    int sz1 = ftell(f1);
    fseek(f1, 0, SEEK_SET);

    fseek(f2, 0, SEEK_END);
    int sz2 = ftell(f2);
    fseek(f2, 0, SEEK_SET);

    if (sz1 != sz2) {
        ok = 0;
        goto _CLOSE_;
    }

    char buf1[4096];
    char buf2[4096];
    int i = sz1;
    while (i>0 && ok) {
        int n = (i > 4096) ? 4096 : i;
        fread(buf1, 1, n, f1);
        fread(buf2, 1, n, f2);
        if (memcmp(buf1, buf2, n) != 0) {
            ok = 0;
        }
        i -= n;
    }

_CLOSE_:
    fclose(f1);
    fclose(f2);
    return ok;
}

void _pico_check (const char* msg) {
    // Always write to out/
    char fmt_out[256];
    sprintf(fmt_out, "out/%s.png", msg);
    pico_output_screenshot(fmt_out, NULL);
    printf("Testing: %s\n", msg);

    #ifdef PICO_CHECK_INT
    // Pause for visual inspection if INT is defined
    puts("-=- press any key -=-");
    pico_input_event(NULL, PICO_KEYDOWN);
    #endif

    #ifdef PICO_CHECK_ASR
    // Assert if ASR is defined
    char fmt_asr[256];
    sprintf(fmt_asr, "asr/%s.png", msg);
    if (!_pico_cmp_files(fmt_out, fmt_asr)) {
        // Base doesn't match, try -CI variant
        sprintf(fmt_asr, "asr/%s-CI.png", msg);
        assert(_pico_cmp_files(fmt_out, fmt_asr));
    }
    #endif
}

#endif // PICO_CHECK_H
