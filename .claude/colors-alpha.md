# Plan: PICO_COLOR_TRANSPARENT + pico_color_alpha

## Status: Done

## Goal

Add a transparent color constant (`Pico_Color_A`) and a utility function
to convert `Pico_Color` (RGB) to `Pico_Color_A` (RGBA) by attaching an
alpha value.

## Changes

| File | Place | Description |
|------|-------|-------------|
| `src/colors.h` | extern declaration | `extern const Pico_Color_A PICO_COLOR_TRANSPARENT` |
| `src/colors.h` | definition | `const Pico_Color_A PICO_COLOR_TRANSPARENT = {0,0,0,0}` |
| `src/pico.h` | declaration | `Pico_Color_A pico_color_alpha(Pico_Color, Uint8)` |
| `src/pico.c` | function | `pico_color_alpha` implementation |
| `valgrind.supp` | suppression | Updated line ref `524` → `528` |
| `lua/pico.c` | function | `L_push_color_a` — push `{r,g,b,a}` table |
| `lua/pico.c` | function | `l_color_alpha` — Lua binding |
| `lua/pico.c` | table entry | `"alpha"` in `ll_color[]` |
| `tst/colors.c` | `colors-07` block | New test: 3x1 alpha buffer on white |
| `lua/tst/colors.lua` | `colors-07` block | Lua mirror of C test |
| `tst/buffer_raw.c` | `buffer[]` array | `{0,0,0,0}` → `PICO_COLOR_TRANSPARENT` (2x) |
| `tst/buffer_pct.c` | `buffer[]` array | `{0,0,0,0}` → `PICO_COLOR_TRANSPARENT` (2x) |
| `lua/tst/buffer_raw.lua` | buffer tables | `{r=0,g=0,b=0,a=0}` → `pico.color.alpha('black',0)` (4x) |
| `lua/tst/buffer_pct.lua` | buffer tables | `{r=0,g=0,b=0,a=0}` → `pico.color.alpha('black',0)` (4x) |

## Lua API

```lua
-- Attach alpha to any color
local clr_a = pico.color.alpha('red', 128)
-- returns {r=255, g=0, b=0, a=128}

local clr_a = pico.color.alpha({r=10, g=20, b=30}, 0)
-- returns {r=10, g=20, b=30, a=0}
```

## Not included

`'transparent'` is NOT registered as a Lua color string — the registry
stores `Pico_Color*` pointers and `PICO_COLOR_TRANSPARENT` is
`Pico_Color_A`. Only makes sense after color strings are supported in
buffer calls (tracked in `todo.md`).

## Test: `colors-07`

3x1 buffer on white background testing alpha blending:
- pixel 0: `pico_color_alpha(PICO_COLOR_RED, 0xFF)` — full red
- pixel 1: `pico_color_alpha(PICO_COLOR_RED, 0x80)` — blended red+white
- pixel 2: `PICO_COLOR_TRANSPARENT` — shows white through

Buffer test replacements (`buffer_raw`, `buffer_pct`, Lua counterparts)
produce identical pixel output — existing `buffer-01`/`buffer-02`
assertions unchanged.

## Verification

- Compile: `gcc -Wall -g -o /tmp/t tst/colors.c src/pico.c -Isrc -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -lSDL2_gfx`
- Lua: `pico-lua lua/tst/colors.lua`
- Buffer: compile `tst/buffer_raw.c` and `tst/buffer_pct.c` (no regression)
