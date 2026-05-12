# 2026-05 empty-layer

Two breaking changes to `pico_layer_empty`:

1. Add `int clear` parameter (sets `scene.clear`).
2. Switch `dim` from `Pico_Abs_Dim` to `Pico_Rel_Dim`.

## Why

- `clear` is currently hardcoded to `1` in `_alloc_layer_empty`
  (`src/mem.hc:144`).
    - Callers wanting `clear=0` must call `pico_set_scene_clear`
      after creation. Make it primary.
- `Pico_Abs_Dim` forces pixel sizing.
    - Rel dim lets the empty layer be sized as a fraction of its
      parent (`%`), of the parent's tile grid (`#`), or as raw
      pixels (`!`).
    - Example: `pico.layer.empty('up', 'me', true, {'%',w=.5,h=.5})`

## Decisions

| topic         | choice                                            |
|---------------|---------------------------------------------------|
| % base        | parent (`up`) layer's `scene.dim`; NULL up = current `G.layer` |
| tile          | stays `Pico_Abs_Dim` (grid count, not a size)     |
| Lua dim form  | keep `{mode, w=, h=}` (no array shorthand)        |

## C API

### Before

```c
void pico_layer_empty (
    const char* up, const char* key,
    Pico_Abs_Dim dim, Pico_Abs_Dim* tile
);
void pico_layer_empty_mode (
    int mode,
    const char* up, const char* key,
    Pico_Abs_Dim dim, Pico_Abs_Dim* tile
);
```

### After

```c
void pico_layer_empty (
    const char* up, const char* key,
    int clear,
    Pico_Rel_Dim dim, Pico_Abs_Dim* tile
);
void pico_layer_empty_mode (
    int mode,
    const char* up, const char* key,
    int clear,
    Pico_Rel_Dim dim, Pico_Abs_Dim* tile
);
```

## Implementation

### `src/mem.hc`

- `_alloc_empty_t` (line 13):
    - `dim` field: `Pico_Abs_Dim` -> `Pico_Rel_Dim`
    - add `int clear`
    - add `const char* up` (to look up % base parent)
- `_alloc_layer_empty` (line 137):
    - resolve `arg->dim` to `Pico_Abs_Dim`:
        - if `arg->up != NULL`: look up parent via
          `realm_get(G.realm, arg->up)` and use its `scene.dim`
          as `Pico_Abs_Rect` base
        - if `arg->up == NULL`: use `G.layer->scene.dim` (the
          current layer)
        - resolve with `_sdl_dim` / `_rnd_dim`
    - apply tile multiplier on the resolved abs dim (same as
      today)
    - pass `arg->clear` to `_layer_new` (replaces hardcoded `1`)

### `src/pico.c`

- `pico_layer_empty` (line 760): add `int clear` arg, change
  `dim` type, forward to `_mode`.
- `pico_layer_empty_mode` (line 765): same; build
  `_alloc_empty_t` including `clear` and `up`.

### `src/pico.h`

- Update prototypes (lines 369-378) and doc comments — note
  that `dim` mode `%` resolves against `up`'s scene.dim, and
  that `clear` sets `scene.clear`.

### `lua/pico.c`

- `l_layer_empty` (line 1128):
    - parse `clear` as bool argument **between key and dim**
    - parse `dim` with `C_rel_dim(L, i+3)` instead of manual
      `{w,h}` extraction
    - forward to `pico_layer_empty_mode(m, up, key, clear,
      dim, ptr)`

New Lua signature:

```lua
pico.layer.empty([mode,] up, key, clear, dim [, tile])
-- ex: pico.layer.empty('up', 'me', true, {'%',w=.5,h=.5})
```

## Affected callers (informational, not part of edits)

C (`tst/`): keep.c, layers.c, layer-empty-tile.c, plus any
that call through the typed signature.

Lua (`lua/tst/`): layers.lua, cv.lua, layer-empty-tile.lua,
plus guide/doc generators.

Regression tests are **not** modified by this plan. Updating
callers to match the new signature is a separate step the
user will approve.

## Steps

1. `src/pico.h` — prototypes + docs.
2. `src/mem.hc` — `_alloc_empty_t` + `_alloc_layer_empty`.
3. `src/pico.c` — `pico_layer_empty` + `_mode`.
4. `lua/pico.c` — `l_layer_empty`.
5. Stop. Hand off to user for caller updates / test runs.

## Open items

- Confirm `realm_get(G.realm, up)` returns the parent
  `Pico_Layer*` (read `scene.dim` from it).
- Confirm `_sdl_dim` signature accepts `Pico_Rel_Dim*` +
  `Pico_Abs_Rect*` base (check `src/aux.hc`).
