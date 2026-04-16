# sub-layer-parent

Eliminate `Pico_Layer.parent` — it is not needed at runtime.

## Claim

`Pico_Layer.parent` is set only for `PICO_LAYER_SUB` and used
at exactly one runtime site. It can be removed by snapshotting
the parent's `view.dim` into the SUB layer at create time.

## Evidence

Only two references in the codebase:

| where                | what                                       |
|----------------------|--------------------------------------------|
| `src/mem.hc:179`     | set at create: `data->parent = c->par`     |
| `src/layers.hc:99`   | runtime read: `root = layer->parent ?: layer;` then `&root->view.dim` for `pico_cv_rect_rel_abs(&layer->view.src, ...)` |

Things one might assume need `parent` are already snapshotted
at create time and do **not** look at `parent`:

- Texture aliasing: `data->tex = c->par->tex`
  (`src/mem.hc:174`)
- `view.src.up` chain: `data->view.src.up = &c->par->view.src`
  (`src/mem.hc:178`) — independent `Pico_Rel_Rect*` chain

So `parent` exists solely to fetch `parent->view.dim` at draw.

## Preconditions

- `Pico_Layer.view.dim` must be immutable after `_layer_new`.
    - Verified: only set in `_layer_new` at `src/mem.hc:110`
    - No other writers found in `src/`

## Options

1. **Snapshot dim** (recommended)
    - Add `Pico_Abs_Dim src_dim` to `Pico_Layer` (used only by
      SUB; zero for non-SUB)
    - Set in `_alloc_layer_sub` from `c->par->view.dim`
    - `layers.hc:99` becomes `&layer->src_dim`

2. **Pre-resolve absolute src**
    - Store `Pico_Abs_Rect src_abs` computed once in
      `_alloc_layer_sub` via `pico_cv_rect_rel_abs`
    - Skips the call at every draw — slightly faster
    - Loses ability for user code to mutate `view.src` after
      create

3. **Hybrid**
    - Keep `view.src` mutable, snapshot only the dim
      (= option 1)

Recommend option 1 unless `view.src` is provably never
mutated post-create, in which case option 2 wins.

## Scope

- `src/pico.c:46-52` — add `src_dim`, remove `parent`
- `src/mem.hc:166-181` — `_alloc_layer_sub` snapshot
- `src/layers.hc:99` — read snapshot instead of `parent`
- No public API change
- Tests: existing visual tests should pass unchanged

## Interaction with up-layer redesign

This is a **pure cleanup**, independent of `up`. After both
land, `Pico_Layer` has:

- `up` (draw target) — new, mandatory
- `src_dim` — SUB only
- no `parent`

Removes one of the open questions in the up-layer plan
("does `up` default to `parent`?") because there is no
`parent` to default to.

## Order

Land **sub-layer-parent first** (smaller, no API break), then
up-layer redesign on top. The up-layer plan should be re-read
after this lands to drop `parent` references.

## Pending

- [ ] Verify `view.src` mutability (decide option 1 vs 2)
- [ ] Add `src_dim` field, remove `parent`
- [ ] Update `_alloc_layer_sub` to snapshot
- [ ] Update `_pico_output_draw_layer` to read snapshot
- [ ] Run `make tests` and `cd lua && make tests`
- [ ] Visual regression: re-run any SUB-using test
- [ ] Update `valgrind.supp:97` `sdl-init` line if `pico_init`
      shifted (unlikely for this change)
