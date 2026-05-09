# 2026-05 base-layer

## Context

Today, ~24 state get/set functions take an optional
`const char* layer` ("base") that defaults to the current layer
through `_pico_layer_null()`.
~16 CV/VS/mouse helpers take an extra `Pico_Abs_Rect* base` or
`const char* layer` for the same reason
(see `.claude/plans/2026-05-layer-utils.md`, which added them).
A separate `'w'` mode lets callers express coordinates in physical
window pixels, implemented by six `_win_*` helpers in `aux.hc`.

The redundancy bites in three ways:

- every call site has to pick between "implicit current layer" and
  "explicit layer name" — two ways to express the same thing
- `_pico_layer_null` makes NULL a magic value, easy to forget
- `'w'` mode duplicates a coordinate system that *should* be just
  another layer

**Goal**: one mechanism — the current layer.
Callers say `pico.set.layer(L)` once, then call APIs without a
"base"/"layer" argument.
The window becomes a predefined layer (`"window"`),
the world/root becomes a predefined layer (`"world"`).

## Decisions (locked)

| #  | Question                                  | Choice                              |
|----|-------------------------------------------|-------------------------------------|
| 1  | `window` role                             | drawable render target              |
| 2  | `pico.set.layer()` no-arg                 | error / required                    |
| 3  | layer-creation `up` parameter             | keep explicit (out of scope)        |
| 4  | NULL layer arg                            | moot — argument removed entirely    |
| 5  | CV/VS `Pico_Abs_Rect* base` arg           | **keep for now** — real semantic override; revisit later |
| 6  | Hard rename `root`→`world`                | yes — `"root"` stops working        |
| 7  | C field `G.world` (lowercase)             | matches C convention                |
| 8  | Enum / string keep uppercase / lowercase  | `PICO_LAYER_WORLD`, `"world"`       |
| 9  | NULL key check                            | none — `realm_find` segfaults; let it crash |
| 10 | Lua scope                                 | included (tests + guide.md)         |
| 11 | Is `window` really a layer?               | yes — degenerate (some fields N/A)  |
| 12 | What does `set.layer("window")` do?       | `SDL_SetRenderTarget(ren, NULL)` (window framebuffer) |
| 13 | Does `world` survive?                     | yes — auto-created child of window  |
| 14 | Predefined enums                          | both `PICO_LAYER_WINDOW` + `PICO_LAYER_WORLD` (open question below) |
| 15 | Reserve `"window"` against user creators  | inherit `"world"` behavior — realm `'!'` mode |
| 16 | `Pico_Window` ↔ `Pico_Layer` relation     | composition (`G.window` contains both `Pico_Window pub` and `Pico_Layer layer`) |
| 17 | `Pico_Window` shape                       | trimmed to `{fs, show, title}`; color/dim live on the layer |
| 18 | Desktop-as-parent unification             | out of scope (follow-up)            |

## Progress

- [x] Drop `layer` param from state get/set — 34 funcs (8 pencil + 12 effect + 14 scene)
- [x] Delete `_pico_layer_null`
- [x] `pico_set_layer` returns previous layer name; tests adopt `old = set(X); …; set(old)`
- [x] Hard rename `root` → `world` in `src/`, `tst/*.c`, `lua/`, `lua/doc/guide.md`
- [x] `pico_set_layer(NULL)` errors (NULL fast-path removed)
- [x] `pico.set.layer()` / `(nil)` errors via `luaL_checkstring`
- [x] Migrate test sites to `"world"`
- [x] Merge `S` into `G`; `G.window.{win,ren,layer,ing,pub}`
- [x] Add `PICO_LAYER_WINDOW`; register `"window"`; `_layer_attach("window","world")`
- [x] Window resize syncs `G.window.layer.scene.dim`
- [x] Trim `Pico_Window` to `{fs,show,title}`; `pico_set_scene_dim` window special-case
- [x] Generalize `pico_output_screenshot` to current layer + universal blit-through-TARGET fallback in `src/pico.c:1661` (handles pixmap/image/text/static + video/streaming uniformly via `SDL_RenderCopy → temp TARGET → ReadPixels`)
- [x] 7 screenshot test cases (`tst/shot.c` + `lua/tst/shot.lua`) for world/empty/sub/pixmap/image/text/video; baselines in `tst/asr/shot-{...}.png`
- [x] Issue [#113](https://github.com/fsantanna/pico-sdl/issues/113) filed: drawing on non-writable layers silently no-ops (SDL error returned + dropped). Tracked separately.
- [x] Rename `PICO_EVENT_WIN_RESIZE` → `PICO_EVENT_WINDOW_RESIZE`; tag `"win.resize"` → `"window.resize"` (commit `83a698b`)
- [~] **`tst/window.c` + `lua/tst/window.lua`** — passing cases (1) initial layer is `"world"`, (2) `set.layer("window")` switches and returns `"world"`, (3) dims (window=500×500 phy / world=100×100 log). Case (4) "draw on window + world at SE quadrant" stays redesigned (baseline `tst/asr/window-01.png` is the post-refactor target). **Currently disabled** in both Makefiles (commented out). Re-enable once the present refactor allows window-as-root (auto-present + traverse from `&G.window.layer`).
- [x] **`G.window.layer.tex` allocated as TARGET texture** (commit-pending) — `pico_init` creates `phy = _tex_create(PICO_DIM_PHY)` + BLENDMODE_NONE; `_pico_output_present` retargets the bespoke world→fb blit to write into `window.tex`, then a final `SDL_RenderCopy(window.tex → fb)` mirrors. `_show_grid` baked into `window.tex` (so screenshots see it; window screenshots still read fb to preserve SDL's alpha=255 fb behavior). `pico_set_scene_dim` recreates `window.tex` on resize. Init refactor: pre-allocate resources into local vars before the big designated initializer; `.layer = NULL` initial + `pico_set_layer` NULL guard for `G.layer->name`. Both test suites green.
- [ ] **Refactor `_pico_output_present` — full traverse-from-window** — replace bespoke world→window.tex blit with `_layer_traverse(&G.window.layer)`. Prerequisite (a) landed; (b) still pending:
    - **(a) Port aux-clipping into `_pico_output_draw_layer`.** ✓ done — aux block ported with bound queried from `SDL_GetRenderTarget`+`SDL_QueryTexture` (handles `_show_grid` and any other case where current G.layer != render target). `tst/layer-clip.{c,lua}` regression suite added (6 cases). Bespoke aux still active for the world→window edge until Phase B.4 lands.
    - **(b) Allow auto-present + explicit `pico_output_present()` from the window layer** so direct-window draws actually present. Today both gate on `G.layer == &G.world`.
    - With (b) done, `_pico_output_present` collapses to `_layer_traverse(&G.window.layer)` + final mirror, the bespoke blit goes away, and `tst/window.c` window-01 starts passing.
- [x] **`pico_set_window_fs` no longer touches `scene.dim`** — fullscreen is now a render-time stretch (final mirror `RenderCopy(window.tex → fb)` stretches automatically). Eliminated `_old`/`new`/`SDL_SetWindowSize` shuffle. Side effect: `_show_grid` no longer iterates past `window.tex` bounds in fs mode (was triggering Phase A aux's "rect entirely outside" assert).
- [ ] Delete `'w'` mode branches in `aux.hc` (5 sites); after the refactor lands
- [ ] Delete `_*_win_to_wld` / `_*_wld_to_win` (6 helpers)
- [ ] Delete `pico_cv_pos_rel_win` / `pico_cv_pos_win_rel`
- [ ] Migrate mouse / `'w'` test sites
- [ ] Regenerate `asr/` for any visual tests that shift after the refactor
- [ ] Add `pico.set.scene_keep` test once world is unlocked (per Open question below)
- [ ] Update `valgrind.supp:sdl-init` line `N` after `pico_init` line shifts

### Open questions

- **Do we need both `PICO_LAYER_WORLD` and `PICO_LAYER_WINDOW` enum values?** Used only for assert-style "is this a special root?" checks. Three options:
    - (a) keep both — symmetric with PLAIN/VIDEO/SUB, precise messages
    - (b) merge into one `PICO_LAYER_ROOT` — same semantics; tell apart by `tex==NULL` or pointer compare
    - (c) drop entirely — pointer-compare `L == &G.world` everywhere

- **Should `world.scene.keep` remain `-1` (locked)?** After the architectural shift, world's specialness is mostly inertia.

    `keep` is consulted at:

    | site | logic |
    |------|-------|
    | `layers.hc:95` post-composite clear | `if (!CUR->scene.keep)` clears |
    | `pico.c:700` `set_effect_color` shortcut | `if (!L->scene.keep && L->hier.up != NULL)` |
    | `pico.c:788` `set_scene_keep` gate | rejects WORLD and SUB (user can't change) |

    `world.keep = -1` does two jobs: (1) skip auto-clear after composite — fires now that world has a parent; (2) forbid user changes (leftover specialness).

    | option | `world.keep` | user can change? | semantics |
    |---|---|---|---|
    | (a) status quo | `-1` | no | "world is the main canvas, hands off" |
    | (b) unlock | `1` | yes | "default persistent, flip to 0 for accumulation" |
    | (c) ordinary | `0` | yes | "scratch buffer, redraw each frame" — saves explicit `output.clear()` |

    Recommendation: **(b)**. Keeps user-visible behavior, drops the WORLD case in `set_scene_keep`, lets motion-trails opt in. Strengthens the case for option (c) on the enum question (drop `PICO_LAYER_WORLD`).

## Failed refactor attempt (2026-05-08)

Tried to land the present refactor (line 230 above) in one pass:

1. `_pico_output_present`: `_layer_traverse(&G.window.layer)` instead of `&G.world`; dropped bespoke aux-clipped blit; dropped per-frame fb clear; allowed auto-present from window or world
2. `pico_init`: one-shot fb clear with window's color
3. `pico_set_scene_dim` window branch: fb clear on resize
4. `pico_output_present()`: relaxed `world`-only assert to `world || window`
5. Redesigned `window-01`: red rect on window at NW + world blue at SE quadrant — meant to demonstrate both visible

Result: 17 visual baselines diverged. Two distinct causes:

| cause | tests hit | fix |
|---|---|---|
| dropped fb clear → leftover content from prior frames in non-full-cover regions | colors-08–10 (4) | follow-up B1/B2 (above) |
| `_pico_output_draw_layer` lacks aux-clipping → over-extending dst loses src compensation | navigate-02–07, rot-flip-02/05/09, tile-grid-02–04, tiles-01–04, view-target-01/02 (17) | follow-up (a) (above) |

Reverted. Two prerequisites split out as concrete follow-ups so the next attempt is bounded.

## Out-of-scope follow-ups

- `'w'` mode + 6 `_win_*` helpers + `pico_cv_pos_*_win` deletion — gated on the present refactor
- CV/VS `base` arg removal (Decision 5)
- Doc rewrite (`api.md`, `guide.md`) — track in `2026-05-layer-utils.md` Backlog
- **Desktop-as-parent unification** (Decision 18): wire `window.effect.alpha` → `SDL_SetWindowOpacity`; `window.scene.dst` → `SDL_SetWindow{Position,Size}`; fold `Pico_Window.fs` into `scene.dst`. After that, `Pico_Window` collapses to layer + sdl-handle.

## Historical (compressed)

### Step B+D: world rename + NULL → realm error
Hard rename `"root"` → `"world"` everywhere; `pico_set_layer(NULL)` becomes a `realm_find` segfault rather than a polite assert. Fold-in: Lua wrapper `l_set_layer` uses `luaL_checkstring`. Done across `src/`, `tst/*.c`, `lua/`, `lua/doc/guide.md`. See "Notes from execution" for sed pitfalls.

### Architectural shift: `window` as the new root
Hierarchy: `window (PICO_LAYER_WINDOW, tex=NULL) → world (PICO_LAYER_WORLD, tex=TARGET) → user layers`. `Pico_Window` is the public type (trimmed to `{fs,show,title}`). Internal `G.window` contains both the public struct and private layer/sdl. The G/S merge and most window-as-layer wiring are in. **Pending pieces** are the present refactor (line 230) and consequences (`'w'` mode deletion, etc.).

## Notes from execution

- `_pico_layer_null` had to be deleted earlier than planned: once
  the state get/set sweep removed every caller, GCC's
  `-Werror=unused-function` blocked the build.
- Sed sweeps for the state group needed two extra patterns missed
  on the first pass: function names with a space before `(`
  (`pico_set_scene_dim (NULL, …)`) and multi-line `(NULL,\n …)`.
  A whitespace-tolerant Python regex fixed the stragglers.
- `tst/keep.c:23` was the only state-group call site with a real
  non-NULL layer arg (`pico_set_scene_keep("right", 1)`); the
  current layer was already `"right"` on the previous line, so the
  rewrite was behavior-preserving.
- During the world-rename sweep, `lua/tst/guide.lua:306` had a user
  text layer keyed `"world"` (the literal `"World!"` content).
  After the rename, this collided with the predefined `"world"`
  slot at `realm_put` time. Resolved by renaming the user layer to
  `"World"` (capital W) — case-sensitive realm lookup.
- Lua wrapper `l_set_layer` was simplified twice in the same step:
  drop the NULL fast-path (`luaL_checkstring`); then expose
  `pico_set_layer`'s return value (`lua_pushstring; return 1`).
- G/S merge done as a separate pass. Resolution of the `.win`
  field-name clash chose to rename the SDL handle: `G.win` →
  `G.window.win` (later regrouped into
  `G.window.{win,ren,layer,ing,pub}`).
- After `Pico_Window` was trimmed, `G.window.pub` shrank to just
  `{fs}`. Color/dim live on the layer's `effect.color` and
  `scene.dim`. `pico_get/set_window_color/dim` were removed.
- `pico_set_scene_dim` window special-case must call
  `SDL_SetWindowSize` and **refresh the framebuffer's clip rect**
  — otherwise SDL retains the pre-resize clip per render-target
  and right/bottom strips don't get cleared/blitted. Caused
  `blend_pct-01` and similar tests to fail with a colored strip
  on the right.
- User-error pattern: `local w = pico.get.window(); w.color = …;
  pico.set.window(w)` silently no-ops once `Pico_Window` lost
  `color`/`dim` — the Lua wrapper just ignores those fields.
  Tests rewritten to use the layer-switch pattern explicitly.
- Initial pixmap/image/text TARGET-tex fix (commit `4e28dba`) was
  reverted in favor of the universal blit-through-TARGET fallback
  in `pico_output_screenshot` (commit `58c54b6`). Single mechanism
  covers all non-TARGET layer types incl. video (STREAMING/YV12),
  no `mem.hc` semantic shift, smaller VRAM footprint.
- Failed refactor attempt (2026-05-08, this session): see dedicated
  section above. Lesson: the bespoke world→window blit silently
  carried two responsibilities (aux clipping + per-frame fb reset)
  that need explicit homes before traverse-from-window can replace
  it.
