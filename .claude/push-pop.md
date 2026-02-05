# pico_push / pico_pop

- GitHub: https://github.com/fsantanna/pico-sdl/issues/21
- Status: implemented

Save/restore drawing state for nested rendering with temporary state
changes.

## Usage

```c
pico_push();          // save current state
pico_set_color_draw(PICO_COLOR_RED);
pico_set_style(PICO_STYLE_STROKE);
// ... draw something ...
pico_pop();           // restore previous state
```

## State to push/pop

Include (affects drawing operations):

- `S.alpha` — transparency
- `S.angle` — rotation
- `S.color.clear` — clear color
- `S.color.draw` — draw color
- `S.crop` — image crop rectangle
- `S.font` — font path
- `S.style` — fill/stroke
- `S.layer` — current active layer (+ SDL target/clip restore)

Exclude (per issue discussion):

- `S.expert` — system-level mode
- `S.win` — window properties
- `view.src` — zoom/scroll viewport
- `view.dst`, `view.dim`, `view.grid`, `view.clip`, `view.tile`

## Stack

- Fixed-size array of 16 entries (`PICO_STACK_MAX`)
- Asserts on overflow/underflow
- Reset on `pico_init`

## Tests

- `tst/push.c` — C test (4 visual checks)
- `lua/tst/push.lua` — Lua test (4 visual checks)

Test cases:

1. `push-01` — push/pop restores draw color
2. `push-02` — push/pop restores style (fill vs stroke)
3. `push-03` — push/pop restores alpha
4. `push-04` — nested push/pop (2 levels deep)

Note: reference images (`tst/asr/push-*.png`) must be generated
by running the test once, then copying from `tst/out/` to `tst/asr/`.

## Changes summary

| File | Place | Description |
|------|-------|-------------|
| `src/pico.c` | globals | `Pico_State` struct + `STACK` |
| `src/pico.c` | `pico_init` | reset `STACK.n = 0` |
| `src/pico.c` | after SET | `pico_push()`, `pico_pop()` |
| `src/pico.h` | State group | declare `pico_push`, `pico_pop` |
| `lua/pico.c` | bindings | `l_push`, `l_pop`, register |
| `tst/push.c` | new | C test |
| `lua/tst/push.lua` | new | Lua test |
| `Makefile` | `tests:` | add `tst/push.c` |
| `lua/Makefile` | `tests:` | add `tst/push.lua` |
