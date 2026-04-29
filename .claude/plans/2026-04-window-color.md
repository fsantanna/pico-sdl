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

### Problem

`pico.set.window(pico.get.window())` errored with "invalid mode at
index 1". Same for view. Get-side emitted `dim={w,h}` (no mode);
set-side parsed via `C_rel_dim` which requires `[1]` mode marker.

C roundtrip was always fine — uses typed structs, not stringly-typed
mode tables.

### First attempt (reverted): force `'!'` in set

Made `l_set_window` / `l_set_view` parse `dim` as `Pico_Abs_Dim`
(direct `w`/`h`, ignore `[1]`); built `Pico_Rel_Dim{'!', ...}`
internally.

Broke `cv.lua:274` which used `dim={'#', w=4, h=4}` (tile mode) in
the bulk setter — silently fell to `'!'` and gave 4×4 absolute
instead of 4 tiles × 4 tiles = 16×16.

Even though `Pico_Window.dim` and `Pico_Layer_View.dim` are
`Pico_Abs_Dim` in the struct, `pico_set_view_dim` accepts rel modes
(`'!'`, `'#'`, `'w'`) and converts; the Lua bulk setter must too.

### Final fix (option A): asymmetric

| side | behavior |
|------|----------|
| set  | keeps `C_rel_dim` — accepts `'!'`, `'%'`, `'#'`, `'w'` modes |
| get  | emits `dim={'!', w, h}` with explicit `[1]='!'` mode marker |

Roundtrip `set(get(x))` survives `C_rel_dim` (`'!'`+`w`+`h` is valid).
Rel modes still work in `set` for users who pass them explicitly.

Applied to `l_get_window` (dim), `l_get_view` (dim + tile),
`l_get_video` (dim). Pattern matches pre-existing `l_get_image`.

### Tests

`tst/get-set.c` and `lua/tst/get-set.lua`: `window roundtrip` +
`view roundtrip` blocks. C always passed; Lua now passes.
