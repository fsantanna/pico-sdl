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
- [x] tests (C, written; need impl + asr/ baselines)
    - [x] appended to `tst/colors.c` as `colors-08/09/10` + default assertion
    - [x] `lua/tst/colors.lua` appended `colors-09/10/11` + alpha-storage assert
    - [x] `tst/asr/colors-08..10.png` generated (user)
    - [ ] generate `lua/tst/asr/colors-09..11.png` after lua build
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

## Baselines to regenerate (after impl)

Existing visual tests render with the old alpha-`0x77` blended gray;
the new opaque `0xFF` will differ in the letterbox area:

- `tst/asr/view-target-01.png`
- `tst/asr/view-target-02.png`
- `tst/asr/view-target-04.png`
- `tst/asr/keep-*.png` (any with shrunk dst)
- `tst/asr/layer-hier-*.png` (any with shrunk dst)

New baselines to generate:

- `tst/asr/colors-08.png` (default gray)
- `tst/asr/colors-09.png` (red letterbox)
- `tst/asr/colors-10.png` (green letterbox via bulk)

Run `make gen T=<app>` for each, manually verify, then commit.
