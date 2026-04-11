# up-layer — scene-graph layer system

## Context

`pico-sdl` layers are flat. SUB layers carry a `sup` snapshot
of their source's `view.dim` but no notion of a draw-target
parent. Drawing relies on global `S.layer`
implicitly. `pico_get_mouse` / `pico_set_mouse`
(`src/pico.c:449, :475`) resolve coords against `S.layer` —
repro at `tst/mouse-w-click.c` (TODO #22). No first-class
collision API.

This redesigns layers as a **scene graph** with:
- Explicit `up` (draw target), id-keyed
- Auto-composite traversal in `pico_output_present`
- Free-mode explicit composite for detached layers
- Realm-as-sole-owner memory model
- String-keyed query API
- Post-composite clear with per-type `view.keep` defaults

---

## States

| state    | definition                                            |
|----------|-------------------------------------------------------|
| root     | `name == "root"`, `up == NULL`. Exactly one.          |
| attached | `up != NULL` and `realm_get(G.realm, up) != NULL`.    |
| detached | non-root with `up == NULL`, OR `realm_get(up)` fails. |

`"root"` is a reserved id; user creates reject it.

## 1. New `Pico_Layer` struct

Replaces `src/pico.c:46-52`. All cross-layer references are
**ids** (`const char*`), borrowed from realm-stable storage —
no `strdup`/`free`, ids may soft-dangle (lookup returns NULL).

```c
typedef struct Pico_Layer {
    PICO_LAYER    type;
    const char*   name;        // own id; borrowed from realm
    SDL_Texture*  tex;
    Pico_View     view;        // dst/src/rot/flip/clear/alpha/keep/...

    struct {
        const char* up;        // parent id; NULL = root or detached
        const char* nxt;       // next sibling under same up
        struct {
            const char* fst;   // first child (back; drawn first)
            const char* lst;   // last child  (front; drawn last)
        } dn;
    } hier;
} Pico_Layer;

// SUB-only fields live in a derived struct in src/layers.hc:
typedef struct {
    Pico_Layer   base;
    Pico_Abs_Dim sup;          // snapshot of source view.dim
} Pico_Layer_Sub;
```

**New enum value** `PICO_LAYER_ROOT` added to `PICO_LAYER`
(`src/pico.c:40-52`). Used solely by `G.root`; backs the
`root | false` row in the §3.3 `view.keep` table and lets
asserts reject root in places where it does not apply (e.g.
`pico_layer_remove_subtree`, sub-layer parents).

`G.root` (was `G.main`) statically initialized:

```c
static Pico_Layer G_root = {
    .type = PICO_LAYER_ROOT,
    .name = "root",
    .tex  = NULL,               // set in pico_init after renderer
    .hier = { .up  = NULL,
              .nxt = NULL,
              .dn  = { .fst = NULL, .lst = NULL } },
    .view = { /* defaults; .keep = 0 */ },
};
```

Inserted into the realm at `pico_init`, after the renderer
and screen-sized texture are created:

```c
G.root.tex = SDL_CreateTexture(...);
realm_put(G.realm, '!', strlen("root")+1, "root",
          _root_noop_free, NULL, &G.root);
```

- `'!'` (exclusive) — root must never be replaced.
- `_root_noop_free` — no-op; the texture is owned by
  `pico_quit`, and the struct is static.
- `realm_get(G.realm,"root")` resolves to `&G.root`; no
  special case in `pico_set_layer` / traversal.

## 2. Lifetime / memory

**Realm is the sole owner.** No refcount.

- **Create** (`_layer_new` in `src/mem.hc:101`): take
  `const char* up`. Insert into realm; append self id to
  `up_layer->hier.dn` list (O(1) via `hier.dn.lst`).
- **`up == NULL` at create** ⇒ create as detached orphan.
  Valid state; no list insertion.
- **Remove** (realm `~` or close): just drop the realm entry.
  Surviving siblings/parents/children still hold the dead id
  in `hier.up`/`hier.dn`/`hier.nxt` — but lookups via
  `realm_get` return NULL, treated as "detached" by all
  walkers.
- **No traversal-on-remove, no null-out pass.** Detach is
  implicit through failed lookup.
- **Stable id storage**: realm must hold id strings in stable
  memory so borrowed pointers stay readable as bytes after
  removal. Lookup fails cleanly; no UB.
- **Cycles**: impossible — `hier.up` is locked at create.

O(1) attach:
```c
static void _layer_attach (Pico_Layer* up, Pico_Layer* self) {
    self->hier.nxt = NULL;
    if (up->hier.dn.fst == NULL) {
        up->hier.dn.fst = self->name;
        up->hier.dn.lst = self->name;
    } else {
        Pico_Layer* tail = realm_get(G.realm, up->hier.dn.lst);
        tail->hier.nxt = self->name;
        up->hier.dn.lst = self->name;
    }
}
```

## 3. Draw model

Two phases:

| phase           | who triggers      | what it does                                          |
|-----------------|-------------------|-------------------------------------------------------|
| pixel fill      | user (explicit)   | `set.layer("L")` + primitives → write into L's texture |
| composition     | `present` (auto)  | walk tree root→leaves, composite each onto its `up`   |

### Pixel fill (unchanged)

User picks a target with `set.layer("L")`, then draws
primitives. Same as today; works for any layer regardless of
attached/detached.

### Auto-composite traversal

`pico_output_present` walks the tree from root:

1. Start at `G.root`. Recurse into `dn` head→tail.
2. For each child `c`, set render target to `c->up`'s texture,
   then `RenderCopyEx(c->tex, src, dst, c->view.rot, anchor,
   c->view.flip)` with `SetTextureAlphaMod(c->view.alpha)`.
3. After composite, **clear `c->tex`** (post-composite clear,
   §3.3) unless `c->view.keep`.
4. Recurse into `c->dn`.
5. Skip any `dn`/`nxt` id whose `realm_get` fails.
6. After traversal, swap window buffer; clear root unless
   `root->view.keep`.

`S.layer` is owned by the traversal during composite. User
must not pixel-fill from inside callbacks invoked by present.

### Free mode (detached only)

```c
void pico_output_draw_layer (const char* layer, Pico_Rel_Rect* rect);
// asserts: layer is detached
```

The only path that explicitly stamps a layer onto the current
target. Attached layers cannot be free-drawn — they would
composite twice.

### Z-order

Last added = last drawn = on top. `dn` list is back-to-front;
new children **append to tail** (O(1) via `dn_tail`).

### Asserts

- `pico_set_layer(name)`: reject `NULL`. Pass `"root"` to draw
  on root.
- `pico_output_draw_layer(name, rect)`: layer must be detached.
- Inside traversal: skip child if `realm_get` fails.

### View extensions (clear / alpha / keep)

```c
typedef struct Pico_View {
    // ... existing dst/src/rot/flip/tile ...

    Pico_Color_A  clear;   // canvas color/alpha after clear
    unsigned char alpha;   // composite mod (whole-texture)
    unsigned char keep;    // skip post-composite clear
} Pico_View;
```

### Alpha pipeline

Three independent channels stack:

1. **Pixel alpha** — what's stored in each pixel of `tex`.
   Written by `clear` (uses `view.clear.a`) and primitives
   (use `S.color.draw.rgb` + `S.alpha`).
2. **Pixel-fill alpha** `S.alpha` — global drawing-state knob,
   push/pop'd via `STACK`. Used during pixel-fill phase only:
   `SetRenderDrawColor(r,g,b, S.alpha)`. Feeds (1).
3. **Composite alpha** `view.alpha` — per layer. Applied at
   composite time via `SetTextureAlphaMod(L->tex, L->view.alpha)`.

Effective output alpha = `pixel_alpha * view.alpha / 255`.

Both (1) and (3) are needed — they're orthogonal:
- An opaque-cleared layer can be composited translucent
  (`view.clear.a=255`, `view.alpha=128`) without affecting
  what its pixel-fill draws blend onto.
- A transparent-cleared layer can hold opaque content
  (`view.clear.a=0`, sprite drawn at full alpha) — only the
  sprite shows, not the canvas.
- Whole-layer fades use `view.alpha` without touching pixel
  contents.

### Globals

| global         | role                                                  |
|----------------|-------------------------------------------------------|
| `S.color.draw` | primitive draw color                                  |
| `S.alpha`      | pixel-fill alpha (per primitive)                      |
| ~~`S.color.clear`~~ | removed; per-layer `view.clear`                  |

### 3.3 Post-composite clear

After a layer is composited (auto-traversal or free-mode),
the engine clears its texture to `view.clear` — unless
`view.keep` is set. Layer textures become **single-frame
scratch buffers** by default.

Note: the existing `pico_output_clear` function and the new
`view.clear` field share the name "clear" but at different
points — the function clears *now* using `view.clear`.

#### `view.keep` defaults per type

| type   | `keep` | rationale                                              |
|--------|--------|--------------------------------------------------------|
| root   | false  | screen surface; fresh frame each present               |
| empty  | false  | user scratch buffer; pixel-fill is per-frame           |
| image  | true   | loaded asset; content is the whole point               |
| buffer | true   | raw-pixel asset                                        |
| text   | true   | rendered once at create; reused                        |
| video  | true   | decoder owns texture                                   |
| sub    | true (forced) | shares texture with source; clearing would wipe parent |

`sub` is *forced* — reject any attempt to set
`view.keep = false` on a SUB at create.

#### Persistence escape hatch

Set `view.keep = true` on any non-SUB layer to disable the
post-composite clear. The texture acts as before: persistent
until the user clears it explicitly. Documented as the
intentional way to do motion trails / accumulation buffers.

## 4. Collision model

**AABB only.** Each layer's collision shape is its `view.dst`
in `up`'s coordinate system.

`pico.vs(l1, l2)` — **LCA walk** via id chains:
1. Resolve l1, l2 from realm; both must be attached.
2. Walk `l1->up` chain marking visited names. Walk `l2->up`
   until a marked name — that's the LCA. `"root"` is fallback.
3. Helper `_layer_rect_in(layer, ancestor)` walks layers
   composing `pico_cv_rect_rel_abs(&cur->view.dst,
   &parent_dim_rect)` per step.
4. `SDL_HasIntersection` of the two rects in LCA's space.

`pico.mouse.get/set(name, ...)` walks from root down to the
target along `up`, applying inverse `dst` per ancestor.

## 5. C and Lua API signatures

Layer creates take leading `const char* up` (id). `NULL` ⇒
detached orphan; `"root"` ⇒ attached to root.

```c
void pico_layer_buffer  (const char* up, const char* key, ...);
void pico_layer_empty   (const char* up, const char* key, ...);
void pico_layer_image   (const char* up, const char* key, const char* path);
void pico_layer_sub     (const char* up, const char* key,
                         const char* parent, const Pico_Rel_Rect* crop);
void pico_layer_text    (const char* up, const char* key, int h,
                         const char* text);
void pico_layer_video   (const char* up, const char* key, const char* path);
// ...and *_mode variants
```

`"root"` is reserved — `key == "root"` rejected at create.

Accessors:

```c
Pico_Mouse pico_get_mouse (const char* layer, char mode);
void       pico_set_mouse (const char* layer, char mode,
                           float x, float y, Pico_Anchor anchor);
int        pico_vs        (const char* l1, const char* l2);
int        pico_vs_pos    (const char* layer, Pico_Rel_Pos* pos);

void       pico_set_layer (const char* key);
// rejects NULL; pass "root" to draw on root explicitly

void       pico_output_draw_layer (const char* layer, Pico_Rel_Rect* rect);
// restricted to detached layers (free mode)
```

Legacy `pico_vs_pos_rect`/`pico_vs_rect_rect` (`src/pico.c:224,
:230`) stay — pure geometry.

Lua bindings (`lua/pico.c`): `pico.layer.*` first arg is the
parent id; `pico.mouse.get/set(layer, ...)`; new
`pico.vs(l1, l2)` dispatcher.

## 6. File-by-file change list

| File | Lines | Change |
|---|---|---|
| `src/pico.c:40-52` | enum + struct | add `PICO_LAYER_ROOT`; new `Pico_Layer` (id-based, `hier.{up,nxt,dn.fst,dn.lst}`); drop `sup` (moved to `Pico_Layer_Sub`) |
| `src/layers.hc` | new typedef | `Pico_Layer_Sub { Pico_Layer base; Pico_Abs_Dim sup; }` |
| `src/mem.hc:166-181` | `_alloc_layer_sub` | allocate `sizeof(Pico_Layer_Sub)`; write `sup` via cast |
| `src/layers.hc:99` | `_pico_output_draw_layer` | cast SUB layer to `Pico_Layer_Sub*` for `sup` |
| `src/pico.c` (new fn) | `_root_noop_free` | no-op realm free for static `G.root` |
| `src/pico.c:60-89` | G/S | rename `G.main` → `G.root`; static init `name="root"`; remove `S.color.clear` |
| `src/pico.c` | `pico_set_color_clear` | remove API; replace with per-layer `view.clear` access |
| `src/pico.c:283-384` | `pico_init` | insert `G.root` into realm; bump `valgrind.supp:97` line N |
| `src/pico.c:432, :435, :528, :1173, :1307` | helper callers | `_pico_layer_image/_text` gain `up`, callers pass `"root"` |
| `src/pico.c:449-473` | `pico_get_mouse` | new sig; chain walk via realm |
| `src/pico.c:475-480` | `pico_set_mouse` | same |
| `src/pico.c:636-653` | `pico_set_layer` | reject NULL |
| `src/pico.c:809-899` | `pico_layer_*` public | `up` arg; reject `"root"` key; insert + attach |
| `src/pico.c` (new fn) | `_pico_present_walk` | tree traversal: composite + post-clear |
| `src/pico.c` `pico_output_present` | rewrite | call walk, swap buffer, clear root if !keep |
| `src/pico.c:1182-1191` | `pico_output_draw_layer` | restrict to detached |
| `src/mem.hc:8-31` | ctx structs | add `const char* up` |
| `src/mem.hc:68-78` | `_free_layer` | no walks; just dispose tex |
| `src/mem.hc:101-115` | `_layer_new` | take `up`; attach via `_layer_attach` |
| `src/mem.hc:121-251` | `_alloc_layer_*` | plumb up; per-type `view.keep` default; SUB enforces `keep=true` |
| `src/layers.hc:4-16` | helper decls | add `up` to internal helpers |
| `src/layers.hc:86-119` | `_pico_output_draw_layer` | invoked by traversal AND free-mode; post-composite clear |
| `src/aux.hc` | new helpers | `_layer_to_win_pos`, `_win_to_layer_pos`, `_layer_rect_in_ancestor`, `_lca`, `_layer_attach` |
| `src/video.hc:119, :143, :152, :240` | video creates | propagate up |
| `lua/pico.c:895-947` | mouse | new signatures |
| `lua/pico.c:996-1001` | `l_set_layer` | reject NULL |
| `lua/pico.c:1168-1276` | `l_layer_*` | first arg `up` |
| `lua/pico.c` (new) | `l_vs` | dispatcher |
| `valgrind.supp:97` | `sdl-init` | bump line N |
| `HISTORY.md`, `lua/doc/guide.md` | docs | hard break + scene graph + auto-composite + clear semantics |

## 7. Migration of tests

- C: ~14 in `tst/`, ~4 in `tst/todo/` use `pico_layer_*`
- Lua: ~13 in `lua/tst/`, ~2 in `lua/tst/todo/`
- Mouse: ~8–10 files each side

Sweep strategy (one hard break, single PR):
1. Apply all C/Lua API changes.
2. `make tests` / `cd lua && make tests`; mechanically prepend
   `"root"` to every `pico_layer_*` call.
3. Rewrite mouse query call sites to string form.
4. Remove explicit `pico.draw(layer)` calls — auto-composite
   handles them. Keep only for detached/free-mode tests.
5. Regen visual baselines where the auto-composite ordering
   changes pixel output.

## 8. Does this close TODO #22?

**Yes, fully** — auto-traversal owns the entire transform
chain per layer, so coordinate mapping for both mouse and
`'w'`-mode draws can use the same chain consistently. The
quantization through the screen log grid disappears once
`pos->up` traces a real layer chain.

## 9. Verification

- `make clean tests` (default + RELEASE)
- `cd lua && make tests`
- `make int T=mouse-w-click` (should pass after fix)
- New tests:
  - `tst/up_attach.c` — chain composited without explicit draw
  - `tst/up_detach.c` — orphan + free-mode `output.draw.layer`
  - `tst/up_remove.c` — parent removed via realm `~`; children
    detach implicitly; free-mode draw still works
  - `tst/scene_vs.c` — three nested layers, `pico.vs` across LCA
  - `tst/keep_persist.c` — `view.keep=true` on empty;
    accumulation
  - `lua/tst/up.lua` — Lua mirror
- Valgrind via `valgrind.supp` (bump `sdl-init:97` line);
  watch realm id stability across removes.
- `make gen` to re-baseline visual tests.

## Related work

This is mainstream territory — basically a tiny **Core
Animation `CALayer`** for SDL2. Nothing here is novel; the
value is the compactness.

### Closest cousins (retained-mode, per-node texture,
compositor traversal)

- **Apple Core Animation `CALayer`** — canonical match. Each
  layer wraps a GPU texture (backing store); compositor walks
  the tree applying per-layer `opacity`, `backgroundColor`,
  transform, masking. Same producer/consumer split (paint vs
  compose).
- **Flutter Layer tree + `RepaintBoundary`** — render tree →
  layer tree → compositor scene. `RepaintBoundary` is
  exactly `view.keep == true`: rasterize once, reuse texture
  across frames.
- **Chrome `cc::Layer`** — each layer backed by tiles; main
  thread paints, compositor thread walks the tree.
- **Cocos2d `Sprite`** / **Godot `CanvasItem`** — 2D scene
  graph with per-node modulate/opacity, parent-relative
  transforms, last-added-on-top.
- **GTK 4 render nodes** / **Android HWUI `RenderNode`** —
  same model in OS UI toolkits.

Older lineage: OpenInventor (Strauss & Carey 1992) introduced
the scene-graph-with-state-stack model that everything here
descends from; the push/pop `STACK` mechanism is straight from
Display PostScript.

### Bundled view vs composed view

| system          | how src/dst/clip/transform/alpha are exposed |
|-----------------|----------------------------------------------|
| CALayer         | one object: `frame`, `contentsRect`, `masksToBounds`, `transform`, `opacity` |
| Flutter         | one node *type* per attribute: `ClipRectLayer`, `TransformLayer`, `OpacityLayer`, ... |
| Cocos2d Sprite  | one sprite object bundling everything       |
| Godot CanvasItem| spread across base + leaf types             |
| **`Pico_View`** | one flat struct, all fields                 |

`Pico_View` is the **flat** form — minimal sound+complete set
(src, dst, clip, rot, flip, alpha, clear, keep) in a single
struct. Cousins: CALayer and Cocos2d Sprite. Trade-off:
simpler API, less mix-and-match than Flutter's compositional
approach.

### Memory model lineage

Three families exist for tree-of-nodes ownership:

1. **Refcount, multi-owner** (CALayer, cc::Layer, Cocos2d) —
   flexible, leak-prone via cycles or stray strong refs.
2. **Tree-as-ownership, single parent** (Qt QObject, Godot
   Node, Rust `Box<Node>`) — strong: drop the root, all
   descendants gone in one recursive destructor. No cycles
   possible.
3. **Registry/arena, single owner** (Bevy ECS `World`, Flecs,
   EnTT, Tk canvas, Apache APR pools, Rust `bumpalo`,
   rustc's type arenas) — strongest: drop the registry,
   *everything* inside is freed atomically. Tree links are
   metadata, not ownership.

**This plan picks family (3).** The realm is the sole owner;
`up`/`dn`/`nxt` are weak id strings that may resolve to NULL
(== detached). Identical in spirit to Bevy's
World+`ChildOf` and to Tk canvas's id-keyed items. Bevy uses
integer generations on entity ids; we use strings.

### What family (3) gives us over Qt-style trees

- **Detached state is first-class**: a layer can outlive its
  parent (still in realm, `up` resolves to NULL). Qt/Godot
  cannot — freeing the parent destroys the child.
- **O(1) parent removal**: just drop the realm entry. No
  recursive destructor walk.
- **No double-free on `~` realm replace**: children re-resolve
  next frame, lookups fail cleanly.
- **Cycles structurally impossible** — `up` is locked at
  create, ids can't form cycles in a registry.

### What we give up vs Qt/Godot, and how to recover it

A detached subtree stays alive in the realm until each node
is individually evicted; Qt frees the entire subtree by
deleting the root. Recover the ergonomics with an optional
helper (Bevy-style):

```c
void pico_layer_remove_subtree (const char* name);
// walks dn/nxt and removes each from the realm
```

Best of both worlds: single-owner guarantee + Qt-like
recursive cleanup when the user wants it. Listed as a
follow-up.

### One-line summary

> "Tiny CALayer for SDL2, with Bevy-style registry ownership
> and Tk-style id keys."

## 10. Risks & follow-ups

- **Soft-dangling ids — BLOCKER, unresolved**:
  `realm_remove_entry` (`src/realm.hc:74`) calls
  `free(e->key)`, so any borrowed `const char*` in
  `up`/`dn`/`nxt` becomes a dangling pointer the moment its
  owner is removed.
  Subsequent `realm_get` would `memcmp` freed memory: UB,
  not a clean NULL as the plan assumes.
  Options:
    1. Intern ids in a separate never-freed string pool;
       realm keys point into the pool.
    2. Refcount id strings held by realm + every borrower.
    3. Change realm to defer key-free / use an arena.
    4. Eager detach pass on remove (kills the
       "no traversal-on-remove" property).
  Option 1 is simplest and matches the plan's spirit.
  Must be resolved before any struct-level work.
- **Auto-composite ordering vs legacy**: existing tests may
  rely on a specific draw order; auto-traversal may produce
  different pixels. Visual baselines need re-review.
- **Trail-bug class**: misuse of `view.keep=true` produces
  silent persistence. Document; consider a debug warning when
  a `keep=true` layer is filled multiple times without
  intermediate clear.
- **Re-entrancy**: pixel-fill during traversal forbidden but
  not enforced — could add `S.in_present` flag and assert in
  `set_layer`.
- **Stack 16→32**: cheap; deeper trees would need heap.
- **Free-mode on root**: forbidden — `output.draw.layer` is
  detached-only.
- **Follow-up 1**: TODO #20 unification (rot/flip/crop on
  text/image/box).
- **Follow-up 2**: explicit `pico_layer_detach(name)` and
  `pico_layer_alive(name)`.
- **Follow-up 4**: `pico_layer_remove_subtree(name)` —
  Qt-style recursive cleanup helper (see Related work).
- **Follow-up 3**: dirty-rect optimization.

## Pending

- [x] Verify realm id-string storage stability
      → **FAILED**: realm frees keys on remove
        (`src/realm.hc:74`); see Risks §10 for options.
        Blocks all subsequent items.
- [ ] Add new struct fields and `_layer_attach`
- [ ] Insert root into realm at `pico_init`
- [ ] Implement `_pico_present_walk` and rewrite
      `pico_output_present`
- [ ] Per-type `view.keep` defaults and post-composite clear
- [ ] Alpha split (`view.alpha` vs `S.alpha`)
- [ ] String-keyed mouse / vs API
- [ ] Restrict `pico_output_draw_layer` to detached
- [ ] Migrate all C and Lua tests
- [ ] New tests (up_attach, up_detach, up_remove, scene_vs,
      keep_persist)
- [ ] Re-baseline visual tests
- [ ] Update `HISTORY.md` and `lua/doc/guide.md`
