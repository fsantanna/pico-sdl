# 2026-04 window-color

Add a window background color property to substitute the hardcoded
letterbox gray at `src/pico.c:1509`.

## Goal

Expose `S.win.color` (default `{0x77,0x77,0x77,0xFF}`) as a settable
property used for the area outside `root.tex` (letterbox/border).

## Distinction

- `root.show.color`     -> clears the root texture (game canvas)
- `S.win.color` (new)   -> fills the window area outside the canvas

## Tasks

- [x] `src/pico.h`
    - [x] add `Pico_Color color` to `Pico_Window`
    - [x] declare `pico_get_window_color`
    - [x] declare `pico_set_window_color`
- [x] `src/pico.c`
    - [x] add `color` to exposed `S.win` struct
    - [x] init `S.win.color = {0x77,0x77,0x77,0xFF}` in `pico_init`
    - [x] replace hardcoded `0x77,0x77,0x77,0x77` with `S.win.color`
    - [x] implement `pico_get_window_color`
    - [x] implement `pico_set_window_color` (calls `_pico_output_present(0)`)
    - [x] propagate `color` in `pico_get_window` / `pico_set_window`
- [x] `lua/` bindings
    - [x] `l_get_window`: emit `color` field via `L_push_color`
    - [x] `l_set_window`: accept `color` (nil-aware) via `C_color_tis`
- [x] tests (all written, all baselines generated, all pass)
    - [x] appended to `tst/colors.c` as `colors-08/09/10` + default + alpha-storage assertions
    - [x] `lua/tst/colors.lua` appended `colors-08/09/10` + alpha-storage assert
    - [x] `tst/asr/colors-08..10.png` generated
    - [x] `lua/tst/asr/colors-08..10.png` generated
- [x] `valgrind.supp`
    - [x] updated `src:pico.c:244` -> `src:pico.c:245`

## Decisions

- **Type**: `Pico_Color` (uniform with `draw.color`, `show.color`).
- **Storage**: preserve user-set alpha as-is (get returns what was set).
- **Render**: pass stored alpha as-is to `SDL_SetRenderDrawColor`
  (no override).
- **Default**: `{0x77, 0x77, 0x77, 0xFF}` — opaque gray.
- **Naming**: `pico_get/set_window_color` (consistent with existing
  `draw.color` / `show.color`).

## Status: complete

All tasks done. All C and Lua tests pass. Baselines generated and
committed. Affected pre-existing visual baselines (`view-target`,
`keep`, `layer-hier` etc.) regenerated for opaque-`0xFF` letterbox.

## Follow-up: bulk roundtrip fix

`l_set_window` and `l_set_view` previously parsed `dim` via
`C_rel_dim` (requires `[1]` mode), causing `pico.set.x(pico.get.x())`
to error with "invalid mode at index 1".

Fixed by mirroring C bulk semantics: parse `dim` as `Pico_Abs_Dim`
(direct `w`/`h` extraction, no mode); construct `Pico_Rel_Dim{'!', ...}`
internally before calling the per-field setter — same pattern used
by C `pico_set_window` / `pico_set_view`.

Tests (C + Lua) appended to `tst/get-set.c` and `lua/tst/get-set.lua`
under `window roundtrip` and `view roundtrip`. C always passed; Lua
now passes after the fix.
