# Plan: Embedded Default Font → PixelOperator

Replace embedded default font `04b_03` (a.k.a. `tiny.ttf`) with
PixelOperator (CC0, ~17 KB).

## Context

- Old default: `src/tiny_ttf.h` → 04b_03 by Yuji Oshimoto, freeware
- New default: `src/pixel_operator_ttf.h` → PixelOperator by Jayvee
  Enaguas (HarvettFox96), CC0
- Source ttf: `/x/pico-sdl/PixelOperator.ttf` (kept for re-gen)
- Old tiny ttf: `src/tiny_ttf.h` kept on disk for future alias

## Tasks

- [x] Download `PixelOperator.ttf` (CC0) from fontlibrary.org
- [x] Generate `src/pixel_operator_ttf.h` via
      `xxd -i -n pico_pixel_operator PixelOperator.ttf`
- [x] Switch `#include "tiny_ttf.h"` → `pixel_operator_ttf.h` in
      `src/pico.c`
- [x] Switch `_alloc_font` symbols
      (`pico_tiny_ttf` → `pico_pixel_operator`) in `src/mem.hc`
- [ ] Update `valgrind.supp` `sdl-init` line if needed
      (no shift expected — `SDL_Init` still at `pico.c:244`)
- [ ] Build and run tests; identify all `asr/` images that
      contain rendered text and need regeneration
- [ ] Regenerate affected `tst/asr/*.png` via `make gen T=...`
- [ ] Regenerate affected `lua/tst/asr/*.png`
- [ ] Verify text dimension assertions
      (e.g. `tst/font.c` asserts `r.w==17 && r.h==10` for "ABC"
      at `h=10` — these may differ for PixelOperator and need
      adjustment)
