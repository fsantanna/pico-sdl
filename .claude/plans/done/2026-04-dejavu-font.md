# Plan: DejaVuSans Font Test

Exercise `pico_set_draw_font` / `pico.set.draw{font=...}` with a
real external TTF (DejaVuSans) alongside the embedded tiny font.

## Context

- Embedded default: `src/tiny_ttf.h` (via `_alloc_font` in `src/mem.hc`)
- New external font: `tst/DejaVuSans.ttf`, `lua/tst/DejaVuSans.ttf`
- Related: `.claude/plans/TODO.md` #19 "Default DejaVu font"

## Tasks

- [x] Extend `tst/font.c`:
    - `font-03`: abs mode, NW, DejaVuSans, "hg - gh"
    - `font-04`: pct mode, C, DejaVuSans, "hg - gh"
- [x] Extend `lua/tst/font.lua`:
    - `font-03`: abs mode, NW, DejaVuSans, "hg - gh"
    - `font-04`: pct mode, C, DejaVuSans, "hg - gh"
- [x] Generate reference images in `tst/out/` (C) for review
- [x] Copy approved images to `tst/asr/`
- [x] Generate reference images in `lua/tst/out/` (Lua) for review
- [x] Copy approved Lua images to `lua/tst/asr/`
