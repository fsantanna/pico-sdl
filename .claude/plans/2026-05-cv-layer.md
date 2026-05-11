# 2026-05-cv-layer

Generalize the view-transform CV pair to arbitrary named layers.

## Context

In `2026-05-window-tex`, we extracted the mouse view-transform into
public CV functions:

```c
SDL_Point pico_cv_pos_cur_win (const Pico_Rel_Pos* pos);
void      pico_cv_pos_win_cur (SDL_Point phy, Pico_Rel_Pos* out);
```

They project between the **current layer's** logical frame and **window**
physical pixels via `G.layer->scene.src/dst`.

## Latent bug (shallow math)

The current impl uses `G.window.layer.scene.dim` as the base for
converting `G.layer->scene.dst`:

```c
Pico_Abs_Rect dst = pico_cv_rect_rel_abs (
    &G.layer->scene.dst,
    &(Pico_Abs_Rect){0, 0, G.window.layer.scene.dim.w, ...}
);
```

But `scene.dst` is in the **parent's** frame, not window's.
Only correct when `G.layer`'s parent **is** window
(i.e., current layer is world or another top-level child).

For sub-layers (HUD inside world, etc.) the math is wrong.

## Proposal

Add layer-targeted pair:

```c
// project: current layer's logical frame -> <name> layer's frame
SDL_Point pico_cv_pos_cur_lyr (const Pico_Rel_Pos* pos, const char* name);

// unproject: <name> layer's frame -> current layer's logical frame
void      pico_cv_pos_lyr_cur (SDL_Point phy, const char* name, Pico_Rel_Pos* out);
```

Existing `cur_win`/`win_cur` become 1-line wrappers:

```c
SDL_Point pico_cv_pos_cur_win (const Pico_Rel_Pos* pos) {
    return pico_cv_pos_cur_lyr(pos, "window");
}
void pico_cv_pos_win_cur (SDL_Point phy, Pico_Rel_Pos* out) {
    pico_cv_pos_lyr_cur(phy, "window", out);
}
```

## Shallow vs deep

| flavor   | behavior                                                              |
|----------|-----------------------------------------------------------------------|
| shallow  | assume `name` is the direct parent of `G.layer`; swap base dim only   |
| deep     | walk hierarchy via `hier.up`; chain transforms up to `name`           |

Only **deep** fixes the latent bug for sub-layers.

## Deep algorithm sketch

```
project(pos, name):
    p = _sdl_pos(pos)         # abs in G.layer's frame
    cur = G.layer
    while cur != layer(name):
        # cur's view: src (sample region) -> dst (placement in parent)
        normalize p in cur.src
        place p in cur.dst (parent's frame)
        cur = cur.up
    return p (in name's frame)
```

`unproject` is the reverse walk.

Assumes `name` is an ancestor of `G.layer`. If not (sibling or
unrelated), walk to LCA. Edge case: same layer = identity.

## Naming alternatives

| candidate                 | reads as                  |
|---------------------------|---------------------------|
| `pos_cur_lyr` / `lyr_cur` | "layer by name" (chosen)  |
| `pos_cur_arg` / `arg_cur` | generic                   |
| `pos_cur_to`  / `to_cur`  | directional               |
| overload existing `pos`   | hidden behavior; rejected |

`cur_lyr` chosen: explicit, short, matches `pico_cv_*_<from>_<to>` style.

## Lua bindings

```lua
pico.cv.pos_cur_lyr (pos: Pos, name: string) -> { x: integer, y: integer }
pico.cv.pos_lyr_cur (phy: { x, y }, name: string, to: Pos)
pico.cv.pos_cur_win (pos: Pos) -> { x: integer, y: integer }  -- wrapper
pico.cv.pos_win_cur (phy: { x, y }, to: Pos)                  -- wrapper
```

## Trade-offs

- Generalizing now: more API surface, fixes sub-layer bug, future-proof.
- Keep `cur_win` only: simple, covers today's mouse needs, sub-layer
  math broken until someone hits it.

## Decision

Pending. Decide after current plan `2026-05-window-tex` lands.

## Tasks

- [ ] Decide shallow vs deep.
- [ ] Verify `hier.up` is reachable for arbitrary `G.layer`.
- [ ] Implement `pico_cv_pos_cur_lyr` / `pico_cv_pos_lyr_cur`.
- [ ] Convert `cur_win`/`win_cur` to wrappers.
- [ ] Add lua bindings.
- [ ] Document in `lua/doc/api.md`.
- [ ] Add tests for sub-layer mouse mapping.
