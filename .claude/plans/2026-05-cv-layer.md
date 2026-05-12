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

## Status (handoff)

### Done

| phase | scope                                                |
|-------|------------------------------------------------------|
| init  | hier.up audit, ancestor-only contract, cv.c audit    |
| A     | `_to`/`_from` use `_sdl_*`/`_rel_*` (float precision) |
| B     | `pos_cur_win`/`pos_win_cur` retired; `set/get_mouse` use `_to`/`_from`; valgrind.supp updated |
| B.1   | signatures fixed to `(layer, fr, to)` (layer first)  |
| D     | dropped 9 old decls from pico.h (pos/rect/dim × rel_abs/abs_rel/rel_rel) |
| E     | tst/cv.c full rewrite (170+ combos); tst/in.c migrated |
| F-1   | lua/pico.c: 6 new `_to`/`_from` shims; old bindings + unused helpers dropped |
| F-2   | lua/tst/cv.lua full rewrite                          |
| F-3   | aux.hc `_rel_*` `!` case rounds (integer-pixel semantic preserved) — **REVERTED**: tst/mouse.c needs subpixel precision (e.g. `0.3f` from 500→50 scale). All three `_rel_dim` / `_rel_pos` / `_rel_rect` `!` branches restored to raw float assignment. `tst/cv.c` already used `(int)` casts so unaffected. |
| G     | `rect_to`/`rect_from`/`dim_to`/`dim_from` C impls + decls + tests |
| H     | rename src helpers `_abs_*` → `_rnd_*` (aux.hc / geom.hc / layers.hc): disambiguates "rounding helper" from the `Pico_Abs_*` type. Test-local `_abs_*` wrappers in tst/cv.c, tst/in.c left as-is (different role: rel→abs convenience). |
| I     | `make tests` + `cd lua && make tests` all green after F-3 revert + H rename |
| J     | Phase C internal cleanup: inline all `pico_cv_*_rel_abs` / `_abs_rel` / `_rel_rel` callers (~38 sites across pico.c, geom.hc, layers.hc, mem.hc) using `_sdl_*` + `_rnd_*`; deleted the 9 old impls from geom.hc. `_rnd_*` switched to by-value (drop intermediate var at ~30 callsites). |
| K     | Lua `cv` API restructure: `pico.cv.{pos,rect,dim}_{to,from}` → nested `pico.cv.{pos,rect,dim}.{to,from}` (lua/pico.c + lua/tst/cv.lua). Six inline lua bindings (no `_l_cv_*` helper). |
| L     | cv tests trimmed: dropped resolution + round-trip sections (redundant with other tests); kept named-layer projection. tst/cv.c 288→110 LOC; lua/tst/cv.lua 170→60 LOC. |
| M     | in tests trimmed: tst/in.c 147→60 LOC (4 cases inline, helpers dropped); lua/tst/in.lua 83→38 LOC (3 binding-smoke cases on current API). |
| N     | `vs` API extended: 2 new args per side (L1/L2 = direct child of cur, or NULL=cur). Added `pico_vs_pos_pos` (pixel equality after rounding). Direct G.layer swap (no `pico_set_layer` SDL side effects). File-static `_vs_pos` / `_vs_rect` aux helpers shared across 3 vs fns. |
| O     | Lua `vs` bindings: 2-arg shorthand (2 strings → layer-pair, 2 tables → value-pair) plus 4-arg full form. tst/vs.c + lua/tst/vs.lua updated; existing callers in collide_pct/raw, mouse-rect-click prefixed with NULL/NULL. |
| P     | `pico_set_mouse` rounds via `_rnd_pos` instead of float→int truncation (1-pixel fix at SDL boundary). |
| Q     | tst/mouse-w-click simplified: dropped `pico_set_expert(1, 0)`, dropped window-layer block for green pixel; draw pixel in world (composited 5× on auto-present); `pico_set_mouse({'%', 0.8, 0.76, NW})` at cur=world. Matches the original asr (098918a) without modification. |

### Public API now

```c
void pico_cv_pos_to    (const char* layer, const Pico_Rel_Pos*,  Pico_Rel_Pos*);
void pico_cv_pos_from  (const char* layer, const Pico_Rel_Pos*,  Pico_Rel_Pos*);
void pico_cv_rect_to   (const char* layer, const Pico_Rel_Rect*, Pico_Rel_Rect*);
void pico_cv_rect_from (const char* layer, const Pico_Rel_Rect*, Pico_Rel_Rect*);
void pico_cv_dim_to    (const char* layer, const Pico_Rel_Dim*,  Pico_Rel_Dim*);
void pico_cv_dim_from  (const char* layer, const Pico_Rel_Dim*,  Pico_Rel_Dim*);
```

`layer = NULL` → cur (mode/anchor conversion only).
Lua: `pico.cv.{pos,rect,dim}.{to,from}(layer, fr, to)`.

### `vs` API now

```c
int pico_vs_pos_rect  (const char* L1, Pico_Rel_Pos*  p1, const char* L2, Pico_Rel_Rect* r2);
int pico_vs_pos_pos   (const char* L1, Pico_Rel_Pos*  p1, const char* L2, Pico_Rel_Pos*  p2);
int pico_vs_rect_rect (const char* L1, Pico_Rel_Rect* r1, const char* L2, Pico_Rel_Rect* r2);
```

`Lx = NULL` → side's value is in cur. `Lx = <child>` → value is in
that direct child of cur. `rect = NULL` with `Lx` set uses the child
layer's bounds. `pos` must never be NULL.

Lua bindings accept three forms: 2 strings (layer-pair), 2 tables
(value-pair, both in cur), or full 4-arg.

## Next steps (explicit)

### 1. Verify full test suites pass

```bash
cd /x/pico-sdl/.work/2026-05-base-layer
make tests           # C tests
cd lua && make tests # Lua tests
```

Expected: all pass.
Known pre-existing flake: `tst/mouse-rect-click.c` step `02` sometimes
fails on xvfb (predates this work — confirmed on clean tree).

### Final task list (for completion)

- [x] Run `make tests` from project root.
- [x] Run `cd lua && make tests`.
- [x] Update `lua/doc/api.md` (cv + vs sections + type defs: drop `up:Rect` from Dim/Pos/Rect; drop dead `rect` arg from `get.mouse`; drop `pos.up` from `set.mouse`).
- [x] Phase C internal cleanup (done in J).
- [x] Commit (done by user).
- [ ] PR.
