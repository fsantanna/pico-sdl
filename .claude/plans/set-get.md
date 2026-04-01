# Plan: set-get

Round-trip tests for simple getter/setter pairs.

## Scope

Only simple cases — no side effects, no complex parameters:

| Pair        | Type         | C getter | Lua getter |
|-------------|--------------|----------|------------|
| alpha       | `int`        | MISSING  | MISSING    |
| color_clear | `Pico_Color` | exists   | MISSING    |
| color_draw  | `Pico_Color` | exists   | MISSING    |
| style       | `PICO_STYLE` | exists   | MISSING    |

## Tasks

### C: add `pico_get_alpha`

- [ ] `src/pico.h` — declare `int pico_get_alpha(void)` in GET section
- [ ] `src/pico.c` — implement `pico_get_alpha` returning `S.alpha`

### Lua: add missing getters

- [ ] `lua/pico.c` — add `l_get_alpha` (pushinteger)
- [ ] `lua/pico.c` — add `l_get_style` (push string via registry
      reverse lookup)
- [ ] `lua/pico.c` — add `l_get_color_clear` (push table `{r,g,b}`)
- [ ] `lua/pico.c` — add `l_get_color_draw` (push table `{r,g,b}`)
- [ ] `lua/pico.c` — register in `ll_get[]`: `alpha`
- [ ] `lua/pico.c` — register in `ll_get[]`: `style`
- [ ] `lua/pico.c` — create `ll_get_color[]` table: `clear`, `draw`
- [ ] `lua/pico.c` — wire `pico.get.color` sub-table in
      `luaopen_pico_native`

### Tests

- [ ] `tst/get-set.c` — C round-trip asserts for all 4 pairs
- [ ] `lua/tst/get-set.lua` — Lua round-trip asserts for all 4 pairs

## Notes

- No visual checks needed — pure assert tests
- Follow `style.c` lines 74–82 pattern for C asserts
- `valgrind.supp` — check if `pico.c` line shift affects
  `src:pico.c:N`
