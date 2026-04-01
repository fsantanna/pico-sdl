# Plan: `pico_set_color_clear_alpha`

Support alpha in the clear color, primarily useful for layers.

## Summary

- Add `pico_set_color_clear_alpha(Pico_Color_A color)` to the C API
- Change internal storage of clear color from `Pico_Color` to
  `Pico_Color_A`
- `pico_set_color_clear(Pico_Color)` stays, sets alpha to 255
- `pico_output_clear` uses the stored alpha instead of hardcoded
  `0xFF`
- Add getter `pico_get_color_clear_alpha(void)` returning
  `Pico_Color_A`
- Existing `pico_get_color_clear(void)` stays, returns `Pico_Color`
  (drops alpha)
- Push/pop saves and restores the full `Pico_Color_A`
- Lua: `pico.set.color.clear` accepts optional `a` field via
  unified `c_color_st` (returns `Pico_Color_A`)

## Changes

### 1. `src/pico.c` — internal state

- [x] `Pico_State.color.clear`: `Pico_Color` → `Pico_Color_A`
- [x] `S.color.clear`: `Pico_Color` → `Pico_Color_A`
- [x] `pico_init`: `{0, 0, 0, 0xFF}` literal

### 2. `src/pico.c` — set functions

- [x] `pico_set_color_clear`: converts to `Pico_Color_A` with
  `a=255`
- [x] `pico_set_color_clear_alpha`: stores `Pico_Color_A` directly

### 3. `src/pico.c` — get functions

- [x] `pico_get_color_clear`: uses `_pico_color()` to strip alpha
- [x] `pico_get_color_clear_alpha`: returns `Pico_Color_A`

### 4. `src/pico.c` — `pico_output_clear`

- [x] Replaced hardcoded `0xFF` with `S.color.clear.a`

### 5. `src/pico.c` — push/pop

- [x] Type change propagates automatically

### 6. `src/pico.h` — declarations

- [x] `void pico_set_color_clear_alpha(Pico_Color_A color);`
- [x] `Pico_Color_A pico_get_color_clear_alpha(void);`

### 7. `src/colors.h` — helper

- [x] `static inline Pico_Color _pico_color(Pico_Color_A c)`

### 8. `lua/pico.c` — refactored color functions

- [x] `c_color_t`: returns `Pico_Color_A`, parses optional `a`
  (merged `c_color_a_t`)
- [x] `c_color_s`: returns `Pico_Color_A` with `a=0xFF`
- [x] `c_color_st`: returns `Pico_Color_A`
- [x] `c_color_a_t`: removed
- [x] `l_set_color_clear`: uses `c_color_st` +
  `pico_set_color_clear_alpha`
- [x] `l_set_color_draw`: strips alpha via `_pico_color()`
- [x] `l_color_darker/lighter/mix/alpha`: strip alpha via
  `_pico_color()`
- [x] Buffer parsing: `c_color_a_t` → `c_color_t`

### 9. Tests

- [x] `tst/clear_alpha.c` — C test
- [x] `lua/tst/clear_alpha.lua` — Lua test
- [ ] Run full test suite (`make tests`) for regressions
- [ ] Copy `tst/out/clear_alpha-01.png` to `tst/asr/`

### 10. Documentation

- [x] `.claude/CLAUDE.md` — Color Types, `c_color_st`, Lua API

## Notes

- Automatic clears in `pico_init` and `pico_set_layer` use
  `pico_output_clear`, which reads `S.color.clear`. Since the
  default alpha is 255, these remain opaque — no special handling
  needed.
- `pico_set_color_clear` resets alpha to 255, preventing stale
  alpha from a previous `pico_set_color_clear_alpha` call.
