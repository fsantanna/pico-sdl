# Accept scene.target for Detached Layers

## Goal

Allow a detached layer (`up == NULL`) to carry a `scene.target`
(`scene.dst`) so that:

1. `pico.set.scene { target = rct }` is legal on a detached layer.
2. `pico.output.draw.layer(id)` may omit the rect (uses the target).
3. `pico.vs.*` collision works against the detached layer, mapping a
   screen position into the layer through its target.

Motivation: SNKRX cards create a detached layer, blit it manually with
`draw.layer(id, rct)`, then try `Object(nil, id, rect, 1)`.
Collision fails because the layer has no placement visible to the
projection walk.

## Current Behavior (investigation)

| file:line | fact |
| --------------------- | ------------------------------------------- |
| get-set.c:363 | `pico_set_scene_dst` asserts `hier.up!=NULL` |
| get-set.c:368 | aspect-fill branch reads parent via `hier.up` |
| layer.c:257 | `draw.layer` resolves `dst` vs `G.layer` dim (base=NULL) |
| geom.c:59,97 | `_pos/_rect_root_to` stop at `up==NULL` |
| geom.c:41,77,117 | `_dim/_pos/_rect_root_fr` stop at `up==NULL` |
| geom.c:229 | `_root_of` walks `up` to top; detached = own root |
| geom.c:247 | `_root_rect`: detached hits `L==R` -> own interior |
| geom.c:262 | `_root_rect` else branch reads `L->hier.up` |
| mem.c:140 | default `dst = '%' {.5,.5,1,1}` (centered, full) |
| pico.c:207 | `world` is attached to `window` |
| pico.c:1146 (lua) | `set.scene` maps Lua key `target` -> `scene.dst` |
| pico.c:1559 (lua) | `l_output_draw_layer`: rect already optional |

### Where `scene.dst` is read

| reader | path | detached? |
| ---------------------------- | --------------------------- | ----------------- |
| `_pico_layer_output(L,NULL)` | `draw.layer` / auto-cascade | manual `draw.layer` |
| `_*_root_to/_fr` walk | `cv` / `vs` | yes |
| `_root_rect` else branch | `vs` | yes |

Auto-cascade never enumerates a detached layer (no `up`, so
`_pico_layer_traverse` skips it). So setting `dst` on a detached layer
is inert except for manual `draw.layer` and `vs`/`cv`.

## Key Insight

The reference frame for a detached layer's `target` is the **current
layer** (`G.layer`), resolved at **use time**, not capture time:

- `draw.layer(det)` already resolves `dst` against `G.layer->scene.dim`
  (`layer.c:257`, base=NULL). No change; works today.
- `vs`/`cv` just need the projection walk to continue into `G.layer`
  instead of stopping at the detached layer.

At the `vs` call, `G.layer` is the frame the caller is working in
(world, in SNKRX) — the same frame the manual blit used — so it lines
up by construction. No stored reference, no `hier.ref` field, no
set-time capture.

A bonus: a detached layer blitted into several frames just uses
whatever frame is current at each `vs` call, so there is no
"single stored target" limitation to document.

## Implementation

| file | place | change |
| ------------ | -------------------------------- | ----------------------- |
| get-set.c | `pico_set_scene_dst` | drop assert; store `dst` |
| geom.c | `_dim/_pos/_rect_root_to` | splice `G.layer` on detach |
| geom.c | `_dim/_pos/_rect_root_fr` | symmetric down-walk |
| geom.c | `_root_of` | follow `G.layer` on detach |
| geom.c | `_root_rect` else branch | use `G.layer` name |
| layer.c | `_pico_layer_output` | none (already `dst`-default) |
| pico.c (lua) | `l_output_draw_layer` | none (already optional) |

### Detached-root predicate

The walk must still stop at a true root, but continue past a detached
user layer. Reuse the existing pointer checks:

```c
// treat as a root: stop the walk
int _is_root (Pico_Layer* L) {
    return L->hier.up == NULL &&
           (L == &G.world || L == &G.window.layer || L == G.layer);
}
```

- `world` / `window`: true roots (checks used at get-set.c:342,
  pico.c:256).
- `L == G.layer`: a detached layer that *is* the current target;
  projecting it into itself would loop. Stop.

### Projection walk (per `_pos_root_to`, mirror the rest)

```c
if (L->hier.up == NULL) {
    if (_is_root(L)) { *out = p; return L; }
    // detached: current layer is the reference frame
    Pico_Layer* P = G.layer;
    // project p through L->scene.dst into P's frame, then recurse
    ...
    return _pos_root_to(P, p, out);
}
```

- `_*_root_to`: use `L->scene.dst` as placement in `G.layer`, recurse
  into `G.layer`.
- `_*_root_fr`: symmetric; recurse into `G.layer` first, then map down
  through `L->scene.dst`.
- `_root_of`: `while (!_is_root(L)) L = up ? up : G.layer;`
- `_root_rect` else branch (geom.c:262): pass `G.layer->name` instead
  of `L->hier.up` when `L->hier.up == NULL`.

Since `world.up == "window"`, a detached layer resolves
detached -> `G.layer` (world) -> window, sharing window as the root
with a window-space position. Collision maps correctly.

### `pico_set_scene_dst` (detached path)

```c
// drop: assert(L->hier.up != NULL ...)
if (dst.w==0 || dst.h==0) {
    if (L->hier.up == NULL) {
        assert(0 && "detached target needs explicit w,h");
    }
    // existing aspect-fill via hier.up parent box
    ...
}
L->scene.dst = dst;
```

Aspect-fill (`w==0`/`h==0`) needs a parent box; on a detached layer
`G.layer==L` at set time, so there is no box to fit against. SNKRX
passes an explicit-w/h card rect, so require explicit w,h and assert
otherwise. Lazy aspect-fill is Won't Do.

## Decisions (closed)

- **Reference:** current layer at use time (no `hier.ref`, no capture).
- **Aspect-fill on detached:** unsupported; require explicit w,h.
- **`get.scene.target`:** return the raw stored `dst` (attached
  aspect-fill is already stored resolved at set-time), symmetric with
  the attached path.

## Tests (written first, reuse existing files + golden)

| file | cases | golden |
| ----------------------- | ------------------------------------- | ------------ |
| tst/vs.c | detached target: p-in-world vs bounds, point-in-det-frame, parity vs attached sibling | none (assert) |
| lua/tst/vs.lua | mirror of the above | none (assert) |
| tst/view-target.c | case 08: detached stored target + `draw.layer(NULL)` | view-target-04 |
| lua/tst/view-target.lua | case 09: same | view-target-04 |

The visual case draws the detached `bg` via its stored `scene.dst`
(rect=NULL). That calls the same `_pico_layer_output` with the same
rect as the explicit-rect blit of case 04, so it renders
pixel-identically — reuse the existing `view-target-04.png`, no new
golden. It doubles as a stored-target vs explicit-rect equivalence
assertion.

The visual case runs as `04b`, immediately after case 04 and BEFORE
case 05 creates `bg2`. `bg2` is an attached child of `world`, so the
draw cascade auto-composites it onto `world` on every present; running
`04b` before it exists keeps the frame identical to the `04` golden.

## Verification (manual, later)

- `make test T=vs` and `T=view-target` (C); `cd lua && make test ...`.
- SNKRX `Card`: `Object(nil, id, rect, 1)` hover works with the manual
  blit unchanged.

## Won't Do

- `hier.ref` field / Approach A / Approach B (reference is `G.layer`).
- Lazy aspect-fill target on a detached layer.
- Auto-drawing detached layers (keep manual `draw.layer`).
- Re-parenting / cascade changes.

## Status

- [x] Decide reference model (current layer, use-time)
- [x] Write tests (vs.c/lua, view-target.c/lua; reuse view-target-04)
- [x] `pico_set_scene_dst`: drop assert, guard aspect-fill
- [x] Projection walk: `_is_root`, `_walk_up`, `_*_root_to/_fr`, `_root_of`
- [x] `_root_rect` else branch
- [x] Verify: `make test T=view-target` passes (04b reuses golden)
- [ ] Verify: `make test T=vs` + lua + SNKRX (pending user run)
