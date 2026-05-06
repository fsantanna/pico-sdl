# Plan: Enforce Layer Param, Remove Current Layer

## Context

Every state API today (`pico_set_pencil*`, `pico_get_effect*`,
`pico_set_scene*`, etc.) accepts `NULL` for the layer parameter,
falling back to a global "current layer" tracked in `S.layer`.
The current layer is set/queried via `pico_set_layer` /
`pico_get_layer`, and `_pico_layer_null` resolves `NULL` to
`S.layer`.

This implicit state leaks across call sites.
The goal is to remove the current-layer concept entirely so
every layer-bound API requires an explicit, non-NULL layer key.

Outcome:
- All layer-bound public APIs take a non-NULL layer key.
- `pico_set_layer` / `pico_get_layer` removed.
- `_pico_layer_null` removed.
- `S.layer` removed.
- `_layer_traverse` uses a local var instead of `S.layer`.
- `up == NULL` (detached parent) on creation funcs unchanged.

## Decisions

| # | topic                       | decision                                |
|---|-----------------------------|-----------------------------------------|
| 1 | output APIs                 | each gets non-NULL layer 1st arg        |
| 2 | aux.hc / cv transforms      | layer arg internally and on pub API     |
| 3 | parent alpha modulation     | pass parent layer explicitly            |
| 4 | text auto cache key         | `_pico_layer_text` takes layer arg      |
| 5 | root-only asserts           | drop assertions, no layer arg           |
| 6 | expert keys                 | always operate on root                  |
| 7 | layer create `up` param     | unchanged; NULL = detached, not "root"  |
| 8 | Lua bindings                | layer optional; default "root"          |

## Final API Signatures

### State (35 fns) — `const char* layer` becomes non-NULL

```c
void pico_get_pencil       (const char* layer, Pico_Layer_Pencil* draw);
Pico_Color pico_get_pencil_color (const char* layer);
const char* pico_get_pencil_font  (const char* layer);
PICO_STYLE pico_get_pencil_style (const char* layer);
void pico_set_pencil       (const char* layer, Pico_Layer_Pencil draw);
void pico_set_pencil_color (const char* layer, Pico_Color color);
void pico_set_pencil_font  (const char* layer, const char* path);
void pico_set_pencil_style (const char* layer, PICO_STYLE style);

void          pico_get_effect        (const char* layer, Pico_Layer_Effect* show);
unsigned char pico_get_effect_alpha  (const char* layer);
Pico_Color    pico_get_effect_color  (const char* layer);
PICO_FLIP     pico_get_effect_flip   (const char* layer);
int           pico_get_effect_grid   (const char* layer);
Pico_Rot      pico_get_effect_rotate (const char* layer);
void pico_set_effect        (const char* layer, Pico_Layer_Effect show);
void pico_set_effect_alpha  (const char* layer, unsigned char alpha);
void pico_set_effect_color  (const char* layer, Pico_Color color);
void pico_set_effect_flip   (const char* layer, PICO_FLIP flip);
void pico_set_effect_grid   (const char* layer, int on);
void pico_set_effect_rotate (const char* layer, Pico_Rot rotate);

void          pico_get_scene      (const char* layer, Pico_Layer_Scene* view);
Pico_Rel_Rect pico_get_scene_clip (const char* layer);
Pico_Abs_Dim  pico_get_scene_dim  (const char* layer);
Pico_Rel_Rect pico_get_scene_dst  (const char* layer);
int           pico_get_scene_keep (const char* layer);
Pico_Rel_Rect pico_get_scene_src  (const char* layer);
Pico_Abs_Dim  pico_get_scene_tile (const char* layer);
void pico_set_scene      (const char* layer, Pico_Layer_Scene view);
void pico_set_scene_clip (const char* layer, Pico_Rel_Rect clip);
void pico_set_scene_dim  (const char* layer, Pico_Rel_Dim* dim);
void pico_set_scene_dst  (const char* layer, Pico_Rel_Rect dst);
void pico_set_scene_keep (const char* layer, int on);
void pico_set_scene_src  (const char* layer, Pico_Rel_Rect src);
void pico_set_scene_tile (const char* layer, Pico_Abs_Dim tile);
```

Each body replaces `_pico_layer_null(layer)` with
`_pico_layer_name(layer)`.

### Output APIs — non-NULL layer 1st arg added

```c
void pico_output_clear      (const char* layer);
void pico_output_draw_image (const char* layer, const char* path,
                             Pico_Rel_Rect* rect);
void pico_output_draw_layer (const char* layer, const char* key,
                             Pico_Rel_Rect* rect);
void pico_output_draw_line  (const char* layer, Pico_Rel_Pos* p1,
                             Pico_Rel_Pos* p2);
void pico_output_draw_oval  (const char* layer, Pico_Rel_Rect* rect);
void pico_output_draw_pixel (const char* layer, Pico_Rel_Pos* pos);
void pico_output_draw_pixels(const char* layer, int n,
                             const Pico_Rel_Pos* ps);
void pico_output_draw_pixmap(const char* layer, const char* key,
                             Pico_Abs_Dim dim,
                             const Pico_Color pixmap[],
                             const Pico_Rel_Rect* rect);
void pico_output_draw_poly  (const char* layer, int n,
                             const Pico_Rel_Pos* ps);
void pico_output_draw_rect  (const char* layer, Pico_Rel_Rect* rect);
void pico_output_draw_text  (const char* layer, const char* text,
                             Pico_Rel_Rect* rect);
void pico_output_draw_text_mode (const char* layer, int mode,
                                 const char* key, const char* text,
                                 Pico_Rel_Rect* rect);
void pico_output_draw_tri   (const char* layer, Pico_Rel_Pos* p1,
                             Pico_Rel_Pos* p2, Pico_Rel_Pos* p3);
```

At entry, each resolves `L = _pico_layer_name(layer)`, then:
- replace `S.layer->pencil.color`/`.style` with
  `L->pencil.color`/`.style`.
- push the SDL render target to `L->tex` (and clip to
  `L->scene.clip`) for the duration of the call, then pop
  back.

### Coordinate transforms — non-NULL layer arg

```c
Pico_Abs_Dim pico_cv_dim_rel_abs (const char* layer,
                                  Pico_Rel_Dim* dim,
                                  Pico_Abs_Rect* base);
void         pico_cv_dim_abs_rel (const char* layer,
                                  const Pico_Abs_Dim* fr,
                                  Pico_Rel_Dim* to,
                                  Pico_Abs_Rect* base);
void         pico_cv_dim_rel_rel (const char* layer,
                                  Pico_Rel_Dim* fr,
                                  Pico_Rel_Dim* to,
                                  Pico_Abs_Rect* base);
Pico_Abs_Pos pico_cv_pos_rel_abs (const char* layer,
                                  const Pico_Rel_Pos* pos,
                                  Pico_Abs_Rect* base);
SDL_Point    pico_cv_pos_rel_win (const char* layer,
                                  const Pico_Rel_Pos* pos,
                                  Pico_Abs_Rect* base);
void         pico_cv_pos_win_rel (const char* layer,
                                  SDL_Point phy,
                                  Pico_Rel_Pos* to,
                                  Pico_Abs_Rect* base);
Pico_Abs_Rect pico_cv_rect_rel_abs (const char* layer,
                                    const Pico_Rel_Rect* rect,
                                    Pico_Abs_Rect* base);
void         pico_cv_rect_abs_rel (const char* layer,
                                   const Pico_Abs_Rect* fr,
                                   Pico_Rel_Rect* to,
                                   Pico_Abs_Rect* base);
void         pico_cv_rect_rel_rel (const char* layer,
                                   Pico_Rel_Rect* fr,
                                   Pico_Rel_Rect* to,
                                   Pico_Abs_Rect* base);
void         pico_cv_pos_abs_rel (const char* layer,
                                  const Pico_Abs_Pos* fr,
                                  Pico_Rel_Pos* to,
                                  Pico_Abs_Rect* base);
void         pico_cv_pos_rel_rel (const char* layer,
                                  const Pico_Rel_Pos* fr,
                                  Pico_Rel_Pos* to,
                                  Pico_Abs_Rect* base);
```

`base == NULL` still permitted (means "use the layer's scene
dim/tile as the reference rect"). Internal aux helpers
(`_sdl_pos`, `_sdl_rect`, `_sdl_dim`, `_rel_pos`, `_rel_rect`,
`_rel_dim`, `_pos_win_to_wld`, `_pos_wld_to_win`,
`_dim_win_to_wld`, `_dim_wld_to_win`) all gain a
`Pico_Layer*` first arg replacing the implicit `S.layer`.

`pico_get_mouse(mode, base)` and `pico_set_mouse(pos)` —
mouse coords are window-relative, always resolved via the
root layer's scene; they do not gain a layer arg. Confirm
during implementation.

### Removed APIs

```c
// pico.h
void pico_set_layer (const char* key);   // delete
const char* pico_get_layer (void);       // delete
```

### Internal cleanup

| change                                                       | file/line              |
|--------------------------------------------------------------|------------------------|
| delete `_pico_layer_null`                                    | layers.hc:34, 61-67    |
| delete `S.layer`                                             | pico.c:52              |
| delete init `S.layer = &G.root`                              | pico.c:283             |
| `_layer_traverse`: drop `old`/`S.layer = UP` save/restore    | layers.hc:87-111       |
| `_pico_layer_text` gains `Pico_Layer* L` arg                 | layers.hc:145, 152-164 |
| `_pico_output_draw_layer` gains `Pico_Layer* parent` arg     | layers.hc:177-211      |
| `pico_set_dim`: drop `assert(S.layer==&G.root)`              | pico.c:580-585         |
| `pico_output_present`: drop assertion                        | pico.c:1600            |
| `pico_output_screenshot`: drop assertion                     | pico.c:1627            |
| `pico_set_effect_color`: drop S.layer render-target restore  | pico.c:667-680         |
| Expert keys (zoom/pan/grid): drop `S.layer == &G.root` gate, always operate on `&G.root` | pico.c:992-1055 |
| `_pico_output_present`: replace `S.layer != &G.root` check with internal "inside output fn" flag | pico.c:1526 |
| `_show_grid`: take `Pico_Layer*` arg instead of using S.layer | pico.c:1409-1437       |

### Lua bindings

For each Lua state/output binding (`l_get_pencil`,
`l_set_pencil`, `l_get_effect`, `l_set_effect`, `l_get_scene`,
`l_set_scene`, output fns, etc.):
- read layer arg from Lua stack; if absent or nil, default to
  `"root"`.
- delete `l_set_layer`, `l_get_layer` and their registry
  entries.

Lua signature pattern:
```
set_pencil([layer], {color=...})   -- layer optional → "root"
draw_rect([layer], rect)           -- layer optional → "root"
get_pencil([layer]) -> table       -- layer optional → "root"
```

### Tests / examples to rewrite

| file                                                  | change |
|-------------------------------------------------------|--------|
| `tst/layers.c`                                        | rewrite: drop `pico_set_layer`/`pico_get_layer` cases; convert `(NULL, ...)` to explicit layer key; "switch back to main" → draws targeting `"root"` |
| any other `tst/*.c` passing `(NULL, ...)` to set/get/output | update to pass explicit layer key (likely `"root"`) |
| `lua/tst/*`                                           | drop `set_layer`/`get_layer` tests; otherwise unchanged (Lua defaults missing layer to `"root"`) |

### Docs

| file              | change |
|-------------------|--------|
| `src/pico.h` doxygen | remove "(NULL = current layer)" from every state-API param doc; remove `pico_set_layer`/`pico_get_layer` blocks; document new mandatory `layer` on output/cv funcs |
| `README.md`       | drop any mention of "current layer" / `set_layer` / `get_layer` (Phase 1 found none — confirm during impl) |

## Critical files

- `src/pico.h` — public API (signatures change)
- `src/pico.c` — bodies + global `S.layer` removal
- `src/layers.hc` — `_pico_layer_null`, `_layer_traverse`,
  `_pico_layer_text`, `_pico_output_draw_layer`
- `src/aux.hc` — coordinate transforms gain `Pico_Layer*` arg
- `lua/pico.c` — bindings: layer optional, defaults to `"root"`
- `tst/layers.c` — rewrite
- `tst/*.c` (others) — patch `(NULL, ...)` calls

## Verification

```bash
make tests                  # all C tests
make int   T=layers         # interactive sanity check
make gen   T=layers         # regen baselines after rewrite
make test  T=layers         # diff against baselines
cd lua/ && make tests       # Lua bindings
make lua   T=layers         # if lua/tst has a layer test
```

Manual smoke:
```bash
./pico-sdl tst/layers.c
./pico-sdl tst/draw.c
```

`valgrind.supp` last clause `sdl-init` line `src:pico.c:N`
must be re-checked after `S.layer` removal shifts line
numbers in `pico_init`.

## Risk notes

- Removing `S.layer` shifts behavior for any caller that
  relied on "set_layer then draw without specifying": every
  such site must be rewritten. Tests cover the main paths.
- `_pico_output_draw_layer` parent-alpha behavior is preserved
  by passing parent explicitly — code-only refactor, no visual
  change expected.
- `pico_set_effect_color` currently switches render target to
  the named layer to clear it, then restores `S.layer->tex`.
  After the change, output funcs each push/pop their own
  render target, so the restore is irrelevant — confirm no
  other code path expects a particular post-call render
  target.
- `aux.hc` adding `Pico_Layer*` to many internals is
  mechanical but touches every call site; budget time for the
  sweep.

## Order of work

1. ✅ DONE — aux.hc internal helpers gain `Pico_Layer*` arg;
   `pico_cv_*` and `pico_vs_*` gain `const char* layer` arg;
   threaded through every caller (S.layer placeholder where
   the actual layer hasn't yet been refactored in).
2. ✅ DONE — Output funcs gain non-NULL `const char* layer` first arg;
   resolve `L = _pico_layer_name(layer)`, set SDL render target
   to `L->tex` and clip to `L->scene.clip`; replace
   `S.layer->pencil/effect.*` reads with `L->*`.
   Pulled in from former step 4:
   - `_pico_output_draw_layer` gains `Pico_Layer* parent` arg;
     alpha mod uses `parent->pencil.color.a`.
   - `_pico_layer_text` gains `Pico_Layer* L` arg; cache key
     uses `L->pencil.font/color`.
   Also: `_show_grid`/`_show_tile` refactored to use SDL
   drawing directly (window-level overlay, not layer-based).
   Lua bindings updated to pass "root" as layer.
3. ✅ DONE — Update state getters/setters: replace `_pico_layer_null`
   with `_pico_layer_name`.
4. ✅ DONE — Refactor `_layer_traverse` to use a local var instead of
   `S.layer` save/restore (the other parts of original step 4
   were folded into step 2).
5. ✅ DONE — Drop root-only assertions; rework expert keys (always operate
   on `&G.root`); replace `_pico_output_present`'s
   `S.layer != &G.root` check with `SDL_GetRenderTarget(G.ren) != G.root.tex`.
6. ✅ DONE — Delete `pico_set_layer`, `pico_get_layer`,
   `_pico_layer_null`, `S.layer`. Sweep any remaining
   `S.layer->name` placeholders left from step 1.
   Also deleted Lua `l_get_layer`, `l_set_layer`.
7. ✅ DONE — Update Lua bindings; all getter/setter/output funcs use "root".
8. ✅ DONE — `tst/layers.c` rewritten; all tests swept clean of
   `pico_set_layer`/`pico_get_layer`/NULL-first-arg state calls.
9. ✅ DONE — pico.h doxygen updated; `(NULL = current layer)` →
   `(required, non-NULL)`; further trimmed to plain
   `layer key` after `(must exist)` strip. lua/doc/api.md and
   lua/doc/guide.md rewritten — no more `pico.set.layer` /
   `pico.get.layer` references.
10. ✅ DONE — valgrind.supp line ref updated (245→252).
    Core library + Lua bindings compile; C and Lua tests pass.

## STATUS: Implementation complete

All 10 steps done:
- Core API: state setters/getters, output funcs all require explicit layer arg
- Removed: pico_set_layer, pico_get_layer, _pico_layer_null, S.layer
- Internal cleanup: _pico_present_if_root helper for layer-aware auto-present
- _show_grid label rendering refactored to use SDL_RenderCopy directly on window
- C tests rewritten (layers.c) and swept; no remaining pico_set_layer or
  NULL-first-arg state calls in tst/ or lua/tst/
- Lua bindings: optional first layer string arg via L_layer_arg helper
- Lua tests rewritten to match new API
- Doxygen `(NULL = current layer)` → `(required, non-NULL)`; further
  trimmed via `(must exist)` strip
- lua/doc/api.md and lua/doc/guide.md rewritten — no more
  `pico.set.layer` / `pico.get.layer` references
- valgrind.supp line ref updated (245→252)

## Notes
- Some visual baselines may need re-regeneration on different machines via
  `make gen T=<app>`
- `_show_grid` label dst must scale text textures to fixed `H=10` height
  with aspect-correct width (`w = H * tex.w / tex.h`) — not native texture
  dims. Old code went through `pico_output_draw_text` which aspect-filled
  to H=10 via `pico_get_text`; using native dims produced larger labels
  and broke `font-03/04` baselines.

## Follow-up: layer at table[1] for Lua state setters

### Goal

Move the layer key from a positional argument **before** the table to
**inside** the table at index 1, for the three Lua state setters:

| current                                            | proposed                          |
|----------------------------------------------------|-----------------------------------|
| `pico.set.pencil("flag", { color=... })`           | `pico.set.pencil { "flag", color=... }` |
| `pico.set.effect("flag", { flip='horizontal' })`   | `pico.set.effect { "flag", flip='horizontal' }` |
| `pico.set.scene("flag", { target={'%',...} })`     | `pico.set.scene { "flag", target={'%',...} }` |

Defaults to `"root"` when index 1 is missing or not a string —
exactly as before with the positional form.

### Rationale

- Single-table calls drop the extra parens / commas — easier to scan.
- The whole spec (layer + named fields) becomes one cohesive table.
- Index 1 is unused on these state tables today (only positional/rect
  tables put `'%'`/`'!'` mode there).

### Scope (only these three)

Only the bulk Lua setters that take a config table are affected:

| binding             | file/lines           |
|---------------------|----------------------|
| `l_set_pencil`      | lua/pico.c:944-979   |
| `l_set_effect`      | lua/pico.c:1004-1053 |
| `l_set_scene`       | lua/pico.c:1063-...  |

Out of scope:
- **Getters** (`l_get_pencil/effect/scene`) — no input table, layer
  stays as positional arg via `L_layer_arg`.
- **Granular setters** (`set.pencil.color`, `set.effect.alpha`, etc.)
  — take a value, not a table; layer stays positional.
- **Output funcs** (`pico.output.draw.*`) — table at arg 2 already
  has mode at `[1]`; layer stays as positional first arg.

### Implementation

In each of `l_set_pencil`, `l_set_effect`, `l_set_scene` replace the
current opening:

```c
const char* layer = L_layer_arg(L);
luaL_checktype(L, 1, LUA_TTABLE);
```

with a new helper that reads layer from `T[1]` rather than from
the stack:

```c
luaL_checktype(L, 1, LUA_TTABLE);
const char* layer = L_layer_at1(L, 1);   // new helper
```

`L_layer_at1` (new, lua/pico.c near `L_layer_arg`):
- `lua_geti(L, ti, 1)` — fetch `T[1]`
- if string: copy, `lua_pop(L, 1)`, return string
- else: `lua_pop(L, 1)`, return `"root"`

Note: keep `T[1]` in the table during the rest of parsing (other
field reads use `lua_getfield` so the integer key is harmless).

### Test/doc migration

| target                              | change                                  |
|-------------------------------------|-----------------------------------------|
| `lua/tst/*.lua` — affected files    | `pico.set.X("L", {...})` → `pico.set.X { "L", ... }` (sed-friendly) |
| `lua/doc/api.md`                    | update three signatures + examples      |
| `lua/doc/guide.md` §7.1, §7.2, §7.4 | update worked examples to new style     |

Sweep command (preview):

```bash
grep -nE 'pico\.set\.(pencil|effect|scene)\("' lua/tst/*.lua lua/doc/*.md
```

### Verification

```bash
cd lua/ && make tests          # all Lua tests pass
make tests                     # C tests unaffected
```

No C-side change. No baseline regen needed (visual output identical).

## Reusing `pico_set_layer` / `pico_get_layer` names — do not

The names should stay retired:

| reason       | detail                                                      |
|--------------|-------------------------------------------------------------|
| state vibe   | `set_layer`/`get_layer` read as "current layer" mutators;   |
|              | reusing them reintroduces the mental model we removed       |
| no real gap  | every layer op now takes `layer` explicitly; nothing needs  |
|              | a generic name                                              |
| ambiguous    | "set a layer" could mean create/destroy/swap/rename/select  |
|              | — too vague to commit to one shape                          |

If layer-management APIs are added later, prefer specific names that
describe the action:

- `pico_layer_exists(key)`           — existence check
- `pico_layer_destroy(key)`          — explicit teardown
- `pico_layer_list(...)`             — enumeration
- `pico_layer_rename(old, new)`      — rename
- `pico_layer_attach(parent, child)` — hierarchy edits
- `pico_layer_detach(child)`         — hierarchy edits
