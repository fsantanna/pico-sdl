# Shot Layer

Capture a screenshot into a live, realm-owned layer instead of a
PNG file on disk.

## Goal

`pico_output_screenshot` flattens a layer to a PNG.
We want the same capture to optionally yield an in-memory layer,
re-drawable / transformable like any other layer.

## API

```c
void pico_layer_screenshot      (const char* up, const char* key,
                                 const char* src, const Pico_Rel_Rect* rect);
void pico_layer_screenshot_mode (int mode, const char* up, const char* key,
                                 const char* src, const Pico_Rel_Rect* rect);
```

- `src`  : layer to capture (NULL = current)
- `rect` : region to capture (NULL = full layer)
- mirrors the `pico_layer_image` constructor shape

## Design

Both endpoints share one capture helper and diverge at the last step.

```
                ┌─ pico_output_screenshot → IMG_SavePNG       (disk)
_pico_shot(sfc) ┤
                └─ _pico_mem_alloc_layer_shot                 (layer)
                       → CreateTextureFromSurface → _pico_mem_layer_new
```

Key decisions

- `_pico_shot(layer, rect)` owns all layer resolution and rect
  resolution (rect based on the captured layer's dims, not `G.layer`).
- Surface owns its pixels (`SDL_CreateRGBSurfaceWithFormat`),
  so there is no borrowed `buf` to free.
- `_pico_shot` restores render target + clip to `G.layer` before
  returning (capture retargets the renderer).
- Layer path stays realm-owned via `realm_put` so it is named,
  attachable, and freed like any layer.

## Files

| file          | place                       | description                       |
|---------------|-----------------------------|-----------------------------------|
| src/output.c  | `_pico_shot`                | shared capture → owning surface   |
| src/output.c  | `pico_output_screenshot`    | PNG tail; forwards name           |
| src/mem.c     | `_pico_mem_alloc_layer_shot`| surface → texture → layer         |
| src/layer.c   | `pico_layer_screenshot[_mode]` | `realm_put` + attach           |
| src/_pico.h   | ctx + decls                 | `_pico_mem_alloc_shot_t`, helper  |
| src/pico.h    | Layer group                 | public docs + signatures          |

## Status

- [x] extract `_pico_shot`, share with `pico_output_screenshot`
- [x] `pico_layer_screenshot[_mode]` realm-owned constructor
- [x] surface owns pixels (no `buf`)
- [x] unify layer + rect resolution inside `_pico_shot`
- [x] restore render target + clip after capture (anchor_pct fix)
- [x] verify full C suite green (`make test T=shot`)
- [x] Lua binding `pico.layer.screenshot` + mirrored `shot.lua` tests
- [x] verify full Lua suite green (`cd lua/ && make tests`)
- [x] docs: `pico.h` doxygen + `lua/doc/api.md` + `lua/doc/guide.md` §10.1
- [x] guide test: `lua/tst/guide.lua` §10.1.c scene (`guide-10-01-03`)
