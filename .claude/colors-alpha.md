# Plan: PICO_COLOR_TRANSPARENT + pico_color_alpha

## Status: Done

## Goal

Add a transparent color constant (`Pico_Color_A`) and a utility function
to convert `Pico_Color` (RGB) to `Pico_Color_A` (RGBA) by attaching an
alpha value.

## Changes

| File | Line | Place | Description |
|------|------|-------|-------------|
| `src/colors.h` | 39 | extern declaration | `extern const Pico_Color_A PICO_COLOR_TRANSPARENT` |
| `src/colors.h` | 76 | definition | `const Pico_Color_A PICO_COLOR_TRANSPARENT = {0,0,0,0}` |
| `src/pico.h` | 533 | declaration | `Pico_Color_A pico_color_alpha(Pico_Color, Uint8)` |
| `src/pico.c` | 500 | function | `pico_color_alpha` implementation |
| `valgrind.supp` | 97 | suppression | Updated line ref `524` → `528` |
| `lua/pico.c` | 516 | function | `L_push_color_a` — push `{r,g,b,a}` table |
| `lua/pico.c` | 546 | function | `l_color_alpha` — Lua binding |
| `lua/pico.c` | 1323 | table entry | `"alpha"` in `ll_color[]` |

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

## Verification

- Compile: `gcc -Wall -g -o /tmp/t tst/colors.c src/pico.c -Isrc -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -lSDL2_gfx`
- Lua: `pico-lua lua/tst/colors.lua`
