# 2026-05-cv-layer

Collapse the `pico_cv_pos_*` family to a single `_to` / `_from` pair
parameterized by a named target layer.
Generalizes the current view-transform CV and fixes the sub-layer bug.

## Context

In `2026-05-window-tex`, we extracted the mouse view-transform into
public CV functions:

```c
SDL_Point pico_cv_pos_cur_win (const Pico_Rel_Pos* pos);
void      pico_cv_pos_win_cur (SDL_Point phy, Pico_Rel_Pos* out);
```

They project between the **current layer's** logical frame and
**window** physical pixels via `G.layer->scene.src/dst`.

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

## Design observations

1. **`abs` is degenerate `rel`**.
   `Pico_Abs_Pos {x,y}` ≡ `Pico_Rel_Pos {!, NW, x, y}`
   resolved into some frame, with the frame reference dropped.
   Two abs values are not comparable without out-of-band frame info.
   That missing frame tag is exactly what enables silent miscomposes.

2. **The user never needs to hold abs values**.
   SDL interop (mouse in, draw out) is wrapped by pico:
   mouse arrives wrapped as `Pico_Rel_Pos {!, NW, x, y}` in
   `"window"` frame; draw consumes rel.
   Everywhere internal, the user works in rel.

3. **`from = cur` is the dominant case**.
   The user always operates inside the current layer
   (`G.layer`).
   Projections go *from* cur *to* a named target,
   or *from* a named source *to* cur.

4. **Only ancestor walks are well-defined**.
   `hier.up` gives a single deterministic path from cur outward.
   Down-walks need to pick a child and may hit overlapping `dst`
   rects; sibling walks need LCA + down-walk.
   Restricting to ancestors makes the math unambiguous and the
   algorithm a single up-chain walk.

## Proposal

Two functions, parameterized by a single layer name; the other end
is always cur:

```c
// cur -> layer  (project out)
void pico_cv_pos_to   (const char* layer, const Pico_Rel_Pos* fr, Pico_Rel_Pos* to);

// layer -> cur  (bring in)
void pico_cv_pos_from (const char* layer, const Pico_Rel_Pos* fr, Pico_Rel_Pos* to);
```

Semantics:

- `layer` is always first — the named ancestor of cur.
- `fr` is interpreted in cur (`_to`) or `layer` (`_from`).
- `to` is written in `layer` (`_to`) or cur (`_from`),
  with `to->mode` and `to->anchor` controlling expression.
- `_to` and `_from` are exact inverses (modulo rounding).

### Ancestor-only contract

The named target (`to` for `_to`, `from` for `_from`)
MUST be cur or an ancestor of cur via `hier.up`.

| direction        | allowed?                                    |
|------------------|---------------------------------------------|
| cur → ancestor   | yes — unique path via `hier.up`             |
| cur → cur        | yes — mode/anchor conversion only           |
| cur → child      | NO — down-walk has multi-child ambiguity    |
| cur → sibling    | NO — would need LCA + down-walk             |
| cur → unrelated  | NO — no path                                |
| cur → orphan     | NO — orphan unreachable                     |

**Violation → assert and abort**:

```c
pico_assert(0 && "cv: target must be cur or one of cur's ancestors");
```

User remedies by calling `pico_set_layer(other)` to make `other`
(or a layer below it) the new cur, then performing the conversion.

### Special layer names

| name      | resolves to               |
|-----------|---------------------------|
| `NULL`    | `G.layer` (current)       |
| `"window"`| `G.window.layer`          |
| `"world"` | `G.world`                 |
| `<other>` | user-named layer in scope |

`pos_to(NULL, p, &out)` collapses to today's `rel_rel`
(mode/anchor conversion, no projection).
No `"cur"` string sentinel — `NULL` is the only "current layer"
shorthand, consistent with the rest of pico's API
(`pico_cv_*_rel_abs(..., NULL)` etc.).

### Reduction of existing API

| today                                  | new                                              |
|----------------------------------------|--------------------------------------------------|
| `pos_rel_abs` / `pos_abs_rel`          | internal only (drop from public header)          |
| `pos_rel_rel(a, b, base)`              | `pos_to(NULL, a, b)`                             |
| `pos_cur_win(rel)` → `SDL_Point`       | `pos_to("window", rel, &out_pxNW)`               |
| `pos_win_cur(phy, out)`                | `pos_from("window", &in_pxNW, out)`              |
| `pos_cur_lyr(rel, name)` (prev plan)   | `pos_to(name, rel, &out)`                        |
| `pos_lyr_cur(phy, name, out)` (prev)   | `pos_from(name, in, out)`                        |

7 public position functions collapse to 2.

### Parallel forms

```c
void pico_cv_rect_to   (const char* layer, const Pico_Rel_Rect*, Pico_Rel_Rect*);
void pico_cv_rect_from (const char* layer, const Pico_Rel_Rect*, Pico_Rel_Rect*);
void pico_cv_dim_to    (const char* layer, const Pico_Rel_Dim*,  Pico_Rel_Dim*);
void pico_cv_dim_from  (const char* layer, const Pico_Rel_Dim*,  Pico_Rel_Dim*);
```

Total public CV API: **6 functions** (today's 11 + previous plan's 4 = 15).

## Why not `(from, to)` in one function?

| approach              | typical call                    | reads as     |
|-----------------------|---------------------------------|--------------|
| `(in, from, out, to)` | `pos(p, NULL, &out, "hud")`     | NULL noise   |
| `_to` / `_from`       | `pos_to("hud", p, &out)`        | clean prose  |

`from = cur` is the dominant case; making it implicit beats
papering over the asymmetry with `NULL`.

## abs and ad-hoc base — implementation details

The entire `pico_cv_pos_*_*` family drops from the public header:

- `pico_cv_pos_rel_abs`, `pico_cv_pos_abs_rel`,
  `pico_cv_pos_rel_rel`,
  `pico_cv_pos_cur_win`, `pico_cv_pos_win_cur` → removed.
- Underlying `_sdl_pos`, `_rel_pos`, `_abs_pos`,
  `_sdl_rect`, `_rel_rect` (aux.hc) → stay internal.

Justification:

- `pos_cur_win` / `pos_win_cur` have zero external callers in tree.
- `pos_rel_abs` / `pos_abs_rel` / `pos_rel_rel` are used externally
  (tst/, lua/) but all real-world projection cases collapse to either
  `_to` / `_from` (named layer) or `pico_in_*` (compose against a
  known rect).
  The `Pico_Abs_Rect* base` argument is a low-level escape hatch:
  every in-tree non-NULL caller is a unit test of the conversion
  math itself, not a user-level operation.

Result: every public value carries its frame tag
(a layer name passed at the call site).
The type system has no way to express "untagged pixels",
so silent miscomposes become impossible.

Same `_to` / `_from` design applies to rect and dim.

## hier.up reachability (verified)

`hier.up` is set **only** by `_layer_attach` (layers.hc:65)
when the caller passes a non-NULL `up`.

| `G.layer` is …                              | up-chain reaches `"window"`? |
|---------------------------------------------|------------------------------|
| `&G.window.layer`                           | trivial — already at root    |
| `&G.world`                                  | yes (attached at init)       |
| user layer with `up != NULL`                | yes                          |
| user layer with `up == NULL` (orphan)       | no                           |
| `Pico_Layer_Sub` w/ `up != NULL`            | yes                          |
| `Pico_Layer_Sub` w/ `up == NULL`            | no                           |

Edge cases the algorithm must handle:

- **cur == `G.window.layer`**: only valid target is `"window"` itself
  (identity / mode-conv). Any other target aborts.
- **Orphan**: target check fails before walking. Abort with
  "cv: cur is not attached to a parent chain" message.
- **Re-attach / cycles** are out of scope for this plan; pre-existing
  responsibility of `_layer_attach` callers.

## Algorithm

```
project(in, target):                  # cur -> target
    L = G.layer
    p = _sdl_pos(in, &L.scene.src)    # float in L's logical frame
    while L != target:
        pico_assert(L.hier.up != NULL && "cv: target unreachable")
        # map L's logical -> L's parent
        p = compose_via_src_dst(p, L)
        L = layer(L.hier.up)
    write p into out  (mode/anchor from out, frame = target)

unproject(in, source):                # source -> cur
    # walk same chain top-down, applying inverse compose
    chain = collect_up_chain_from_cur_until(source)
    pico_assert(found && "cv: source unreachable")
    p = _sdl_pos(in, &source.scene.src)
    for L in reverse(chain):
        p = inverse_compose_via_src_dst(p, L)
    write p into out  (frame = cur)
```

Edge case: `target == cur` → no walk, mode/anchor conversion only.

## Lua bindings

```lua
pico.cv.pos_to    (layer: string?, fr: Pos, to: Pos)
pico.cv.pos_from  (layer: string?, fr: Pos, to: Pos)
pico.cv.rect_to   (layer: string?, fr: Rect, to: Rect)
pico.cv.rect_from (layer: string?, fr: Rect, to: Rect)
pico.cv.dim_to    (layer: string?, fr: Dim,  to: Dim)
pico.cv.dim_from  (layer: string?, fr: Dim,  to: Dim)
```

Existing `pos_cur_win`/`pos_win_cur` lua bindings either:

- get removed (breaking change), or
- become 1-line lua wrappers calling `pos_to("window", ...)` etc.

## Trade-offs

- **Smaller surface** (6 vs 15) but **breaking change** for
  callers of `pos_cur_win`/`pos_win_cur` and the
  `rel_abs`/`abs_rel`/`rel_rel` family.
- **No public abs type** — must be wrapped at SDL boundary inside
  pico. Mouse event delivery and similar entry points need a
  small `SDL_Point -> Pico_Rel_Pos {!,NW}` shim.
- **String-keyed targets** — 1 hash per call, cheap but non-zero.
  Could add a `Pico_Layer*` overload internally if perf matters.
- **Ancestor-only** rules out sibling/child conversions; user must
  `pico_set_layer` to a layer that has both ends in its ancestor
  chain first. Cheap escape hatch.

## Decision

Pending.
Decide after current plan `2026-05-window-tex` lands.

## Tasks (phased)

- [x] Verify `hier.up` reachability for arbitrary `G.layer`.
- [x] Lock ancestor-only contract; assert-and-abort on violation.
- [x] Audit external callers in tst/ and lua/.
- [x] Commit to dropping ad-hoc-base from public API.
- [x] Implement `pico_cv_pos_to` / `pico_cv_pos_from` (int, via public CV).

### Phase A — float-precision rewrite

- [x] Switch `_to`/`_from` internals from
      `pico_cv_pos_rel_abs`/etc. to `_sdl_pos`/`_sdl_rect`/`_rel_pos`.
- [x] Verify build clean.

### Phase B — retire `pos_cur_win` / `pos_win_cur`

- [x] Rewire `pico_set_mouse` to call `pico_cv_pos_to("window", rel, &out_pxNW)`.
- [x] Rewire `pico_get_mouse` to call `pico_cv_pos_from("window", &inNW, out)`.
- [x] Drop declarations from pico.h.
- [x] Delete implementations from geom.hc.
- [x] Update valgrind.supp SDL_Init line.

### Phase B.1 — finalize signatures

- [x] Reorder `_to`/`_from` args to `(layer, fr, to)`.
- [x] Update all call sites and docs.

### Phase C — convert internal pico.c callers off `pos_rel_abs`/etc.

- [ ] Replace each internal `pico_cv_pos_rel_abs(rel, NULL)` with
      `_sdl_pos(rel, NULL)` + `_abs_pos(...)`.
- [ ] Replace `pico_in_pos` ad-hoc-base usage with `_sdl_pos(rel, &out_abs)`.
- [ ] Same for `pico_cv_pos_abs_rel` callers.

### Phase D — drop public `pos_rel_abs` / `pos_abs_rel` / `pos_rel_rel`

- [ ] Remove declarations from pico.h.
- [ ] Remove implementations from pico.c.

### Phase E — migrate tst/ tests

- [ ] tst/cv.c: rewrite ad-hoc-base tests via named sub-layers OR delete
      if redundant with named-layer coverage; rewrite NULL-base tests
      against `_to`/`_from`.
- [ ] tst/in.c: same.
- [ ] Add new tests:
    - [ ] mode/anchor convert in cur (`to=NULL`)
    - [ ] cur <-> window round-trip identity
    - [ ] sub-layer (HUD inside world) round-trip
    - [ ] orphan layer → assert fires
    - [ ] sibling target → assert fires

### Phase F — migrate Lua bindings

- [ ] lua/pico.c: drop `base` arg from `pico.cv.pos(...)`;
      add `pos_to` / `pos_from` bindings.
- [ ] lua/tst/cv.lua: rewrite tests.
- [ ] Update lua/doc/api.md.

### Phase G — rect/dim parallels

- [ ] Add `pico_cv_rect_to` / `pico_cv_rect_from`.
- [ ] Add `pico_cv_dim_to`  / `pico_cv_dim_from`.
- [ ] Lua bindings for both.
- [ ] Tests for both.
