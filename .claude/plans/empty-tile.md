# empty-tile

Add an optional `tile` parameter to `pico_layer_empty` so empty layers can
be created in tile units and carry their tile size from birth.

## Goal

Today `pico_layer_empty(key, dim)` takes `dim` as absolute pixels and the
new layer's `view.tile` is `{0,0}`.
This makes tile-based workflows awkward:

- Caller must compute `cols * tile.w` themselves.
- After creation, caller must call `pico_set_view(tile=...)` to register
  the tile size before `'#'`-mode dims, `view.grid`, or `_show_tile` work.

The change: add a 3rd optional `Pico_Abs_Dim* tile` argument.

- `tile == NULL` → unchanged: `dim` is pixels.
- `tile != NULL` → `dim` is interpreted as **cols × rows**;
  texture pixel size is `dim.w*tile->w` by `dim.h*tile->h`;
  `view.tile` is recorded on the new layer.

## API

```c
void pico_layer_empty (
    const char* key, Pico_Abs_Dim dim, Pico_Abs_Dim* tile
);
void pico_layer_empty_mode (
    int mode, const char* key, Pico_Abs_Dim dim, Pico_Abs_Dim* tile
);
```

Buffer/image constructors are **not** changed.
Their size comes from source data, so the tile reinterpretation doesn't
apply.
They could later gain a metadata-only `tile` arg if sprite-sheet
ergonomics demand it, but that is out of scope here.

## Internals

`_alloc_layer_empty` in `src/mem.hc`:

1. Receive a context carrying both `dim` and `tile` (pointer or NULL).
2. Compute pixel dim:
    - if tile: `{dim.w*tile->w, dim.h*tile->h}`
    - else:    `dim`
3. Build the layer via `_layer_new` with the pixel dim.
4. If tile is non-NULL, overwrite `view.tile = *tile` after `_view_new`
   (which zeros it).

Order is irrelevant because the pixel dim is computed before texture
creation.
No globals are touched, so the alloc callback stays pure with respect to
`S.layer`.

## Call sites (C)

Every existing `pico_layer_empty(key, dim)` call appends `, NULL`.
Same for `_mode` variants.
Mechanical migration via grep.

Files to update (TBD by grep at edit time):

- `src/pico.c`             | `pico_layer_empty`, `pico_layer_empty_mode` | new signature
- `src/pico.h`             | declarations                                | new signature
- `src/mem.hc`             | `_alloc_layer_empty`, alloc ctx struct      | thread tile through
- `lua/pico.c`             | `pico.layer.empty` binding                  | accept optional 3rd positional
- `tst/*.c`                | existing callers                            | append `, NULL`
- `lua/tst/*.lua`          | existing callers                            | unchanged (3rd arg optional)

## Lua binding

Positional, 3rd argument optional:

```lua
pico.layer.empty("map", {20,15}, {16,16})  -- 20x15 grid of 16x16 tiles
pico.layer.empty("fx",  {256,256})          -- plain pixel layer
```

## Tests

New visual regression: `tst/layer-empty-tile.c` (+ `lua/tst/layer-empty-tile.lua`).

Coverage in one program:

1. Create empty layer with `tile={16,16}`, `dim={20,15}`.
2. `pico_set_view(grid=1)` to overlay the tile grid.
3. Draw something into the layer (a few rects on tile boundaries).
4. `pico_set_view(dim='#'...)` to resize in tile units;
   confirm the new texture size matches expectation and the grid still
   aligns.

Run:

```bash
make gen  T=layer-empty-tile     # generate asr/
make test T=layer-empty-tile     # check
cd lua && make test T=layer-empty-tile
```

## Open questions

- None blocking.
  Confirmed: no assertions on tile/dim positivity (existing code already
  fails loudly via `_tex_create` if zero).

## Status

- [ ] `src/pico.h` — update declarations
- [ ] `src/mem.hc` — extend alloc ctx, thread tile, set `view.tile`
- [ ] `src/pico.c` — update `pico_layer_empty` / `_mode` bodies
- [ ] `lua/pico.c` — accept optional 3rd positional
- [ ] migrate existing C callers (`tst/*.c`, anywhere in `src/`)
- [ ] add `tst/layer-empty-tile.c`
- [ ] add `lua/tst/layer-empty-tile.lua`
- [ ] update `valgrind.supp` line `N` if `src/pico.c` `SDL_Init` line moves
