# Layer Target: API Re-evaluation (empty / target / resolution)

Follow-up to [done/260703-layer-target.md]: detached targets work, but
the API grew asymmetries. Re-evaluate `layer.empty` (C/Lua), make
`target` fully usable in both attach modes, accept `w/h=0` and `%`
everywhere feasible, and pin down *when* each value resolves.

## 1. Current API: `layer.empty`

### C

```c
void pico_layer_empty      (up, key, clear, Pico_Rel_Dim dim,
                            Pico_Abs_Dim* tile);
void pico_layer_empty_mode (mode, up, key, clear, dim, tile);
```

- No target parameter at all; target requires a separate
  `pico_set_layer(key)` + `pico_set_scene_dst(dst)` + restore dance.

### Lua

```
pico.layer.empty([m], up, me, clear, Rect)               -- rect-as-dim
pico.layer.empty([m], up, me, clear, Dim)
pico.layer.empty([m], up, me, clear, Dim, Tile, [Rect])
```

- Rect-as-dim shortcut: the rect's `w/h` double as the layer Dim AND
  the full rect becomes `scene.target` (pico.c:1251).
- Constructor target exists only in Lua; C has none.

### Asymmetries found

| # | issue |
| - | ------------------------------------------------------------ |
| a | C constructor cannot set target; Lua can (two ways) |
| b | rect-as-dim: `w/h=0` aborts (dim IS the rect; no size source; `_pico_tex_create` gets 0) |
| c | attached target `w/h=0`: eagerly resolved to `!` at set time; non-zero `%` stored raw and resolved lazily — mixed semantics |
| d | detached target `w/h=0`: asserted out (no box at set time) |
| e | `vs`/`cv` walk resolves `dst` with `ratio=NULL`, so a stored `w=0` would collide as a zero box (why (c)'s eager fill exists) |
| f | `clear=true` is inert on a detached layer: the cascade reaches flagged layers via `hier.dn` from an ancestor (output.c:21) and the effect-color repaint requires `up!=NULL` (get-set.c:275) — a detached layer has neither, and no re-attach API exists; own tests pass `true` harmlessly (view-target `bg`, vs `det_vs`) |

## 2. Target in both modes: constructor and scene

Goal: `target` equally usable attached/detached, set at construction
or later via `set.scene`.

| mode | constructor | set.scene{target} |
| -------- | ----------------------------- | ------------------ |
| attached | Lua: Dim+Tile+Rect / rect-as-dim; C: none | yes |
| detached | same Lua paths (up=nil) | yes (explicit w,h) |

### Proposal

- **Lua**: keep both forms; lift restrictions per §3 so all rect
  shapes are accepted in both modes.
- **C**: DECIDED option B — no API change. C keeps the set-after
  pattern (`set_layer` -> `set_scene_dst` -> `set_layer(old)`);
  document with an example. Constructor-target stays a Lua-surface
  convenience ([260704-layer-create.md]).

## 3. Accept `w/h=0` and `%` everywhere: go fully lazy

Root cause of (c)(d)(e): aspect-fill is resolved *eagerly* only
because the `vs` walk cannot fill it *lazily* (`ratio=NULL`).
Fix the walk instead; then eager resolve disappears.

### Change

- `pico_set_scene_dst`: store `dst` RAW, always. No eager resolve,
  no asserts (both modes, any shape).
- Thread `&L->scene.dim` as ratio wherever `scene.dst` is resolved:

| site | today | change |
| ------------------------------ | ------------ | ---------------- |
| layer.c `_pico_layer_output` | `dp=&dim` | none (has it) |
| geom.c `_*_root_to/_fr` (x6) | `ratio=NULL` | `&L->scene.dim` |
| geom.c `_root_rect` else | via cv | resolve w/ ratio |

- Harmless for concrete rects (`_f_rat` no-ops when `w,h!=0`).

### Result matrix (target shapes accepted)

| shape | attached | detached |
| --------------- | -------- | -------- |
| `!` w,h > 0 | yes | yes |
| `%` w,h > 0 | yes | yes |
| `w=0` or `h=0` | yes (lazy fill) | yes (lazy fill) |
| `w=h=0` | yes (full layer dim) | yes |

- Behavior change (attached): aspect-fill now re-resolves if the
  parent is later resized (was frozen at set time). More correct —
  aligns with non-zero `%`, which already tracks (today a `%` rect
  freezes even its x,y when w or h is 0, one field apart from one
  that tracks); golden 05-07 unaffected (parent never resizes there).
- `get.scene.target` now always returns the raw stored rect
  (supersedes the 260703 "stored resolved" note; fully symmetric).
- No texture cost: lazy resolution is per-use arithmetic on the
  placement rect only; `dim`/texture are untouched (draw already
  resolves `dst` every blit — vs/cv just gains the same ratio).
- Creation with `target` but no `dim` (rect-as-dim, Lua): inference
  stays EAGER — a one-time copy of `target.w/h` resolved as a normal
  creation Dim; the stored target stays raw/lazy. Requires
  `target.w,h > 0`; inferring an aspect for an empty layer (square?
  parent aspect? parent dim?) is invention with silent-missize
  failure — keep the error ([260704-layer-create.md] `^d`).

### Still impossible

- rect-as-dim with `w/h=0` (b): the rect is the only size source, so
  the layer dim itself is undefined. Keep rejected; require the
  `Dim, Tile, Rect` form. Assert with a clear message in Lua.

## 4. Resolution: when each value resolves

| value | when | base | ratio |
| --------------- | --------------- | -------------------------- | ----- |
| layer `dim` | EAGER, creation | parent dim (up) or current layer (detached) | none: `0` invalid |
| `tile` | eager, creation | multiplies dim | - |
| `target` store | never (raw) | - | - |
| `target` @ draw | LAZY, each draw | current render target dim | layer dim |
| `target` @ vs/cv | LAZY, each call | parent (attached) / current layer (detached) | layer dim |
| `source` | lazy, each use | own dim (sup for sub) | - |

Rules of thumb (document in api.md/guide):

- dim is physical -> eager (texture must exist); everything placing
  the layer is logical -> lazy.
- `%` in dim = fraction of parent at CREATION time.
- `%` in target = fraction of the frame at USE time.
- detached reference frame = current layer at use time (260703).

## Implementation Sketch

| file | place | change |
| ------------ | ------------------------ | -------------------------- |
| get-set.c | `pico_set_scene_dst` | store raw; drop resolve + asserts |
| geom.c | 6 walk fns | `_raw_rect(L->scene.dst, &Pb, &L->scene.dim)` |
| geom.c | `_root_rect` else | same ratio treatment |
| doc | api.md / guide.md | resolution table (§4); C set-after example (§2 B) |

## Tests

- Extend `tst/vs.c` + `lua/tst/vs.lua`: detached target with `w=0`
  and with `%` — collision must match the equivalent concrete rect.
- Extend `view-target`: attached target `%`+`w=0` set BEFORE a parent
  resize -> re-resolves (new lazy semantics).
- C constructor target (opt A): mirror of Lua rect-as-dim case
  reusing `view-target-04` golden.

## Decisions (closed)

- §2 constructor: option B — C unchanged (docs only); Lua kw-table
  carries constructor-target ([260704-layer-create.md]).
- (f) `clear` on detached: resolved by [260704-layer-create.md] —
  kw form defaults `false` and errors on `clear=true` + `up=nil`.
- §3 lazy-everywhere: ACCEPTED, including attached aspect-fill
  re-resolving on parent resize and raw `get.scene.target`.
- rect-as-dim with `target.w/h = 0`: stays an error (no honest
  aspect source for an empty layer).

## Won't Do

- `set_scene_dim` `%`-assert unification: dim concern, not target;
  revisit under [260704-layer-create.md] if it bites there.

## Status

- [x] Decide §2 constructor option: B (no C change)
- [x] Decide lazy-everywhere (§3) incl. resize semantics: accepted
- [x] Implement store-raw + ratio threading (get-set.c + geom.c;
  `_root_rect` resolves locally before cv, which has no ratio)
- [x] Tests: vs.c/vs.lua — detached h=0, detached '%', attached
  re-resolve on parent resize
- [x] Re-run goldens (view-target 05-07, vs sub_aspect) + new cases:
  all tests pass (user-run)
- [x] Docs: api.md (set.scene target + orphan note), guide.md 7.4
  (resolution table), pico.h (set_scene_dst doxygen + set-after
  example)
