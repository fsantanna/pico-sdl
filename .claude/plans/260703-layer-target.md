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
| get-set.c:360 | `pico_set_scene_dst` asserts `hier.up!=NULL` |
| get-set.c:368 | aspect-fill branch reads parent via `hier.up` |
| geom.c:59,97 | `_pos_root_to` / `_rect_root_to` stop at `up==NULL` |
| geom.c:229 | `_root_of` walks `up` to top; detached = own root |
| geom.c:247 | `_root_rect` identity-projects a detached rect |
| layer.c:250 | `_pico_layer_output`: `rect==NULL` -> `scene.dst` |
| pico.c:1559 | `l_output_draw_layer`: rect already optional |
| pico.c:1146 | `set.scene` maps Lua key `target` -> `scene.dst` |
| _pico.h:32 | `hier.up`: "NULL = root or detached" (overloaded) |

Consequences:

- Setting `target` on a detached layer aborts (SIGABRT).
- `draw.layer(id)` (no rect) would already work if `dst` were set.
- The projection walk treats a detached layer as its own root, so a
  window-space mouse never maps into it.

## Problem

`scene.dst` currently means "placement inside `hier.up`".
A detached layer has no `hier.up`, so `dst` has no reference frame.
Two distinct concepts are conflated on `hier.up`:

- draw cascade parent (who auto-draws me as a child)
- geometry reference (whose frame my target is expressed in)

A detached layer wants the second without the first: it is blitted
manually, but still needs a reference frame for `target` + `vs`.

## Proposal

Introduce a geometry reference for detached layers, separate from the
draw cascade. A detached layer with a target behaves, for geometry and
projection only, as if placed at `target` inside a reference layer,
while remaining outside the auto-draw cascade.

### Reference frame

`target` on a detached layer is expressed relative to a reference
layer. Default: the current layer at the time `target` is set
(mirrors `draw.layer`, whose rect is in the current render target's
frame). Optionally overridable later.

For SNKRX cards the reference resolves to `world`, matching the manual
`draw.layer(id, rct)` blit.

### Approach A (preferred): new `hier.ref` field

- Add `const char* ref` to `hier` (geometry-only reference).
- `pico_set_scene_dst` on a detached layer: drop the hard assert;
  require `ref` to be resolvable (capture current layer if unset);
  use `ref` as the parent box in the aspect-fill branch.
- Projection walk (`_pos_root_to`, `_rect_root_to`, `_dim_root_to`
  and their `_fr` twins, plus `_root_of`): when `up==NULL` and
  `ref!=NULL`, continue the walk into `ref` using `dst` as placement,
  instead of stopping.
- Draw cascade (`draw.layers`, child enumeration via `hier.dn`/`nxt`)
  ignores `ref`, so the layer is not auto-drawn.

### Approach B (smaller, riskier): reuse `hier.up` as reference

Attach for geometry but exclude from the cascade (do not link into the
parent's `dn.fst/lst` / `nxt`). Less code, but overloads `up` further
and risks cascade code assuming `up` implies a child link.

## Implementation Sketch (Approach A)

| file | place | change |
| ------------ | ---------------------------- | ------------------------- |
| _pico.h | `Pico_Layer.hier` | add `const char* ref` |
| get-set.c | `pico_set_scene_dst` | relax assert; use `ref` box |
| get-set.c | `pico_get_scene_dst` | return stored `dst` if set |
| geom.c | `_pos_root_to` `_rect_root_to` `_dim_root_to` | walk into `ref` on detach |
| geom.c | `_pos_root_fr` `_rect_root_fr` `_dim_root_fr` | symmetric down-walk |
| geom.c | `_root_of` | follow `ref` when `up==NULL` |
| layer.c | `_pico_layer_output` | already `dst`-default: no change |
| pico.c (lua) | `l_output_draw_layer` | already optional: no change |

### Open questions

- Reference capture: current-layer-at-set vs explicit param on
  `set.scene { target=..., ref=... }`.
- Should `get.scene.target` return the raw rel rect or the resolved
  `!` rect?
- Aspect-fill (`w==0`/`h==0`) target on detached: resolve against `ref`
  dim; confirm anchor handling matches the attached path.
- Multiple blits: a detached layer may be `draw.layer`-ed into several
  targets/frames; `vs` uses only the single stored `target`. Document
  that `vs` tracks the target, not each manual blit.

## Verification (manual, later)

- `detached_target.lua`: `set.scene{target}` no longer aborts.
- Extend `lua/tst/vs.lua`: detached layer + target, assert
  `vs.pos.rect('window', p, det, inner)` matches the attached case.
- SNKRX `Card`: `Object(nil, id, rect, 1)` hover works with the manual
  blit unchanged.

## Won't Do

- Auto-drawing detached layers (keep manual `draw.layer`).
- Re-parenting / cascade changes beyond the geometry reference.

## Status

- [ ] Decide Approach A vs B
- [ ] Decide reference-capture semantics
- [ ] Implement projection + set/get dst
- [ ] Verify (script + vs.lua + SNKRX)
