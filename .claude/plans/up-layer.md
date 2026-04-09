# up-layer — scene-graph layer system

Depends on `sub-layer-parent.md` landing first
(eliminates `Pico_Layer.parent`).

## Context

`pico-sdl` layers are flat. `Pico_Layer.parent` is set only for
`SUB` (texture crop source). Drawing relies on global `S.layer`
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
    Pico_View     view;        // dst/src/rot/flip/alpha/keep/...

    const char*   up;          // parent id; NULL = root or detached
    const char*   dn;          // first child id  (back; drawn first)
    const char*   dn_tail;     // last child id   (front; drawn last)
    const char*   nxt;         // next sibling under same up

    Pico_Abs_Dim  src_dim;     // SUB only (from sub-layer-parent)
} Pico_Layer;
```

`G.root` (was `G.main`) statically initialized: `name="root"`,
`up=NULL`, `dn=NULL`, `dn_tail=NULL`. Inserted into the realm
at `pico_init` so `realm_get(G.realm,"root")` resolves.

## 2. Lifetime / memory

**Realm is the sole owner.** No refcount.

- **Create** (`_layer_new` in `src/mem.hc:101`): take
  `const char* up`. Insert into realm; append self id to
  `up_layer->dn` list (O(1) via `dn_tail`).
- **`up == NULL` at create** ⇒ create as detached orphan.
  Valid state; no list insertion.
- **Remove** (realm `~` or close): just drop the realm entry.
  Surviving siblings/parents/children still hold the dead id
  in `up`/`dn`/`nxt` — but lookups via `realm_get` return
  NULL, treated as "detached" by all walkers.
- **No traversal-on-remove, no null-out pass.** Detach is
  implicit through failed lookup.
- **Stable id storage**: realm must hold id strings in stable
  memory so borrowed pointers stay readable as bytes after
  removal. Lookup fails cleanly; no UB.
- **Cycles**: impossible — `up` is locked at create.

O(1) attach:
```c
static void _layer_attach (Pico_Layer* up, Pico_Layer* self) {
    self->nxt = NULL;
    if (up->dn == NULL) {
        up->dn = self->name;
        up->dn_tail = self->name;
    } else {
        Pico_Layer* tail = realm_get(G.realm, up->dn_tail);
        tail->nxt = self->name;
        up->dn_tail = self->name;
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

### Alpha split

| field        | purpose                  | when applied                       |
|--------------|--------------------------|------------------------------------|
| `view.alpha` | composite mod, per layer | auto-traversal `RenderCopyEx`      |
| `S.alpha`    | pixel-fill alpha, global | `SetRenderDrawColor` on primitives |

Effective output alpha = `pixel_alpha * view.alpha / 255`.

`S.alpha` lives in `S` (push/pop'd via `STACK`). The user
never touches `S.alpha` for compositing; that's `view.alpha`.

`S.color.clear` is repurposed as the **root clear color**.
Per-layer clear color lives in `view.clear_color`.

### 3.3 Post-composite clear

After a layer is composited (auto-traversal or free-mode),
the engine clears its texture to `view.clear_color` — unless
`view.keep` is set. Layer textures become **single-frame
scratch buffers** by default.

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
| `src/pico.c:40-52` | enum + struct | new `Pico_Layer` (id-based, dn/nxt) |
| `src/pico.c:60-89` | G/S | rename `G.main` → `G.root`; static init `name="root"` |
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

## 10. Risks & follow-ups

- **Soft-dangling ids**: realm must guarantee id-string
  storage outlives any borrower. Validate `realm` impl or
  switch to interning.
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
- **Follow-up 3**: dirty-rect optimization.

## Pending

- [ ] Verify realm id-string storage stability
- [ ] Land sub-layer-parent first
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
