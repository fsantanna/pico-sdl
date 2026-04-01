# Plan: set-get

Round-trip tests for simple getter/setter pairs.

## Scope

Only simple cases — no side effects, no complex parameters:

| Pair        | Type           | C getter | Lua getter |
|-------------|----------------|----------|------------|
| alpha       | `int`          | done     | done       |
| color_clear | `Pico_Color_A` | exists   | done (rgba)|
| color_draw  | `Pico_Color`   | exists   | done       |
| style       | `PICO_STYLE`   | exists   | done       |

## Tasks

### C: add `pico_get_alpha`

- [x] `src/pico.h` — declare `int pico_get_alpha(void)` in GET section
- [x] `src/pico.c` — implement `pico_get_alpha` returning `S.alpha`

### Lua: add missing getters

- [x] `lua/pico.c` — add `l_get_alpha` (pushinteger)
- [x] `lua/pico.c` — add `l_get_style` (push `"fill"` or `"stroke"`)
- [x] `lua/pico.c` — add `l_get_color_clear` (push `{r,g,b,a}` via
      `pico_get_color_clear_alpha`)
- [x] `lua/pico.c` — add `l_get_color_draw` (push `{r,g,b}`)
- [x] `lua/pico.c` — register in `ll_get[]`: `alpha`, `style`
- [x] `lua/pico.c` — create `ll_get_color[]` table: `clear`, `draw`
- [x] `lua/pico.c` — wire `pico.get.color` sub-table in
      `luaopen_pico_native`

### Tests

- [x] `tst/get-set.c` — C round-trip asserts for all 4 pairs
- [x] `lua/tst/get-set.lua` — Lua round-trip asserts for all 4 pairs
      (includes color_clear alpha round-trip)
- [x] Run C test — PASS
- [x] Run Lua test — PASS
- [x] Verify `valgrind.supp` — `src:pico.c:522` unchanged

## Complete
