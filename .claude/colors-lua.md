# Plan: Refactor c_color → c_color_s + c_color_st — DONE

## Goal

Remove the 3-argument `(r,g,b)` form from `c_color`. Split into:
- `c_color_s(L, i)` — string lookup (e.g., `'red'`)
- `c_color_st(L, i)` — string or table, dispatches to `c_color_s` or `c_color_t`

All functions take an explicit `int i` index parameter.

## File: `lua/pico.c` — DONE

### 1. Extract `c_color_s(L, i)` from the string branch of `c_color` — DONE

### 2. Rename `c_color` → `c_color_st(L, i)`, remove `(r,g,b)` branch — DONE

### 3. Update 6 callers — DONE

- `l_color_darker` — `c_color_st(L, 1)`
- `l_color_lighter` — `c_color_st(L, 1)`
- `l_color_mix` — `c_color_st(L, 1)` + `c_color_st(L, 2)`, removed `lua_remove` hack
- `l_set_color_clear` — `c_color_st(L, 1)`
- `l_set_color_draw` — `c_color_st(L, 1)`

## Lua test files — convert `(r,g,b)` to string/table — DONE

Color mapping:

| `(r,g,b)` | Replacement |
|-----------|-------------|
| `0xFF, 0xFF, 0xFF` | `'white'` |
| `0xFF, 0x00, 0x00` | `'red'` |
| `0x00, 0xFF, 0x00` | `'green'` |
| `0x00, 0x00, 0xFF` | `'blue'` |
| `0xFF, 0xFF, 0x00` | `'yellow'` |
| `0x00, 0x00, 0x00` | `'black'` |
| non-standard | `{'!', r=N, g=N, b=N}` |

Files (20 total):
- `lua/tst/blend_pct.lua` — 7× red
- `lua/tst/blend_raw.lua` — 7× red
- `lua/tst/dim.lua` — 1× white, 1× red
- `lua/tst/collide_pct.lua` — 3× white, 3× red
- `lua/tst/collide_raw.lua` — 2× white, 2× red
- `lua/tst/view_raw.lua` — 1× white, 1× red
- `lua/tst/shot.lua` — 3× non-standard → table form
- `lua/tst/pos.lua` — 5× white, 4× red
- `lua/tst/rect.lua` — 3× white, 4× red
- `lua/tst/layers.lua` — 2× non-standard → table, 1× green, 1× black
- `lua/tst/clip_pct.lua` — 1× non-standard → table, 1× red, 1× blue, 1× yellow
- `lua/tst/clip_raw.lua` — 1× non-standard → table, 1× red, 1× blue, 1× yellow
- `lua/tst/buffer_pct.lua` — 1× white
- `lua/tst/buffer_raw.lua` — 1× white
- `lua/tst/image_raw.lua` — 1× white
- `lua/tst/image_pct.lua` — 1× white
- `lua/tst/todo/move2.lua` — 1× white
- `lua/tst/todo/pct.lua` — 5× white, 4× red
- `lua/tst/todo/move.lua` — 1× white, 1× black (commented)
- `lua/tst/todo/main.lua` — 1× white

## Verification

- `make tests` (or `xvfb-run make tests`) — pending
