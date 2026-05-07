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

| # | Question                          | Choice                              |
|---|-----------------------------------|-------------------------------------|
| 1 | `window` role                        | drawable render target              |
| 2 | `pico.set.layer()` no-arg         | error / required                    |
| 3 | layer-creation `up` parameter     | keep explicit (out of scope)        |
| 4 | NULL layer arg                    | moot — argument removed entirely    |
| 5 | CV/VS `Pico_Abs_Rect* base` arg   | **keep for now** — real semantic override, not dead code. Proactively migrate non-NULL callers to NULL where a layer can replace the explicit base (e.g. screenshot → window). Revisit dropping the param later. |

## Scope

### A. Drop the `layer` / `base` parameter

**State get/set** (`src/pico.c` + `src/pico.h`):
all of these lose their `const char* layer` first arg.

| group   | funcs | done | functions                                                   |
|---------|-------|------|-------------------------------------------------------------|
| pencil  | 8     | [x]  | get/set: `pencil`, `pencil_color`, `pencil_font`, `pencil_style` |
| effect  | 12    | [x]  | get/set: `effect`, `effect_alpha`, `effect_color`, `effect_flip`, `effect_grid`, `effect_rotate` |
| scene   | 14    | [x]  | get/set: `scene`, `scene_clip`, `scene_dim`, `scene_dst`, `scene_keep`, `scene_src`, `scene_tile` |

**CV / VS / mouse** (`src/pico.c` + `src/pico.h`):
~15 helpers from `2026-05-layer-utils.md`.
Per Decision 5, **deferred** — keep `Pico_Abs_Rect* base` for now.
Mouse migration still happens as part of plan C (`'w'` → `window`).

### B. Predefined layers `world` and `window`

Created in `pico_init`, never destroyable, names reserved:

| name  | type           | tex                       | purpose                        |
|-------|----------------|---------------------------|--------------------------------|
| `world` | existing root  | (current `G.root`)        | world/root render target       |
| `window` | new            | `NULL` → `SDL_SetRenderTarget(ren, NULL)` selects window | physical-pixel target |

`G.root` keeps its struct slot but its `name` becomes `"world"`.
`pico.set.layer("window")` calls `SDL_SetRenderTarget(NULL)` and uses
window dim as scene.dim — output and input both work in window
pixels.

`_pico_layer_name("world" | "window")` resolves the predefined slots;
user `pico_layer_*` creators must reject these two names.

### C. Remove `'w'` mode and `_win_*` helpers

Mode parser (`src/aux.hc:106-111`, `:153-158`, `:205-212`,
`:260-264`, `:298-302`, `:336-342`):
delete the `case 'w'` branches.

Then delete `_dim_win_to_wld` / `_dim_wld_to_win` /
`_pos_win_to_wld` / `_pos_wld_to_win` /
`_rect_win_to_wld` / `_rect_wld_to_win`
(`src/aux.hc:43`, `:53`, `:63`, `:73`, `:83`, `:89`).

Public `pico_cv_pos_rel_win` / `pico_cv_pos_win_rel`
(`src/pico.c:104-112`):
delete — callers express the same thing by setting layer = `window`
and using mode `'!'`.

Mouse: `pico_get_mouse(char mode, …)` keeps mode arg (still wants
`'!' '%' '#'`) but no longer accepts `'w'` —
to read window-pixel mouse, set layer = `window` first, mode = `'!'`.

### D. Remove `_pico_layer_null` and `set.layer()` no-arg

`src/layers.hc:61-66`: delete `_pico_layer_null`.
Remaining call sites (none, after part A) → use
`_pico_layer_name` directly.

`pico_set_layer(NULL)` becomes an `assert(name != NULL)`;
`pico.set.layer()` with no Lua arg becomes a `lua_error`.

## Files to modify

| file                         | place                                  | description                                                |
|------------------------------|----------------------------------------|------------------------------------------------------------|
| `src/pico.h`                 | state get/set declarations             | drop `const char* layer` first param (~24 funcs)           |
| `src/pico.h`                 | CV/VS/mouse declarations               | drop `base` / `layer` extra param (~15 funcs)              |
| `src/pico.h`                 | `pico_cv_pos_rel_win`, `_win_rel`      | delete                                                     |
| `src/pico.c`                 | matching definitions                   | drop params, replace `_pico_layer_null(L)` with `S.layer`  |
| `src/pico.c:104-112`         | `pico_cv_pos_*_win`                    | delete                                                     |
| `src/pico.c` `pico_init`     | layer setup                            | name root `"world"`, create predefined `"window"` slot          |
| `src/pico.c` `pico_set_layer`| body                                   | special-case `"window"` → `SDL_SetRenderTarget(NULL)`; assert non-NULL |
| `src/pico.c` `pico_get_mouse`| `'w'` branch                           | delete                                                     |
| `src/pico.c` `pico_set_mouse`| body                                   | drop `pico_cv_pos_rel_win` call; resolve through `S.layer` |
| `src/aux.hc:43,53,63,73,83,89`| `_*_win_to_wld`, `_*_wld_to_win`      | delete (6 helpers)                                         |
| `src/aux.hc:106-342`         | `case 'w'` in `_sdl_*` / `_rel_*`      | delete                                                     |
| `src/layers.hc:34,61-66`     | `_pico_layer_null`                     | delete decl + body                                         |
| `src/layers.hc` create funcs | `_pico_layer_pixmap/image/text/sub`    | reject keys `"world"` and `"window"`                            |
| `lua/pico.c`                 | `l_get_*` / `l_set_*` wrappers         | drop the `NULL` first arg in C calls (~24 sites)           |
| `lua/pico.c` `l_set_layer`   | body                                   | error if no string arg                                     |
| `lua/pico.c` `C_mode_opt`    | mode set                               | drop `'w'` from accepted modes                             |
| `valgrind.supp`              | `sdl-init` clause                      | update `src:pico.c:N` after `pico_init` line shift         |

## Tests

### Will fail (must be migrated)

| file                              | line(s)                            | symptom / fix                                                                          |
|-----------------------------------|------------------------------------|----------------------------------------------------------------------------------------|
| `lua/tst/mouse.lua`               | 10,17,24,38,45,52,66,73,94,101,110 | `pico.set.mouse({'w', …})` → `pico.set.layer("window"); pico.set.mouse({'!', …})`         |
| `lua/tst/mouse-rect-click.lua`    | 36,44,51,59,66,74,81,89            | same — both `set.mouse({'w', …})` and `output.draw.pixel({'w', …})`                    |
| `lua/tst/mouse-rect-click.lua`    | 18,40,52                           | `pico.set.layer()` (NULL) → `pico.set.layer("world")`                                    |
| `lua/tst/tiles.lua`               | 10,40,80                           | `set.mouse({'w', …})` → set `window` layer + `'!'`                                        |
| `lua/tst/cv.lua`                  | 71                                 | `{'w', w=250, h=500}` → use `window` layer + `'!'`                                        |
| `lua/tst/layers.lua`              | 16,18                              | `pico.set.layer()` → `pico.set.layer("world")`                                           |
| `lua/tst/todo/mouse-rect-click.lua` | —                                | `pico.get.mouse('w')` → `pico.set.layer("window")` + `pico.get.mouse('!')`                |
| `lua/tst/todo/mouse-layer.lua`    | —                                  | same as above                                                                          |
| `tst/mouse-w-click.c`             | 17,34,41                           | rename test to `mouse-win.c`; `'w'` → layer `"window"` + `'!'`; `pico_set_layer(NULL)` → `"world"` |
| `tst/mouse.c`                     | 157,169,178                        | drop `pico_set_mouse('%')` (already removed); `pico_get_mouse('w', …)` → `window` + `'!'` |
| `tst/tiles.c`                     | 9,66,76,85                         | same pattern as `mouse.c`                                                              |
| `tst/cv.c`                        | mode 'w' loop                      | substitute `window` layer iteration                                                       |
| `tst/layers.c`                    | 16,40,52,59                        | `pico_set_layer(NULL)` → `pico_set_layer("world")`                                       |

### Visual regression — `asr/` regen needed

Run `make gen T=<name>` after manual inspection
(`make int T=<name>`):

`mouse`, `mouse-rect-click`, `mouse-win` (renamed from
`mouse-w-click`), `tiles`, `cv`, `layers`, plus any
`guide-*` checks that reference `'w'` mode or NULL-layer fallback
(spot-check via `grep -n "'w'\\|layer()" lua/tst/guide.lua`).

### New tests

| file                              | covers                                                                  |
|-----------------------------------|-------------------------------------------------------------------------|
| `tst/predef-layers.c`             | `pico_get_layer()` after init = `"world"`; `window` exists; both reserved    |
| `lua/tst/predef-layers.lua`       | same on Lua side; reject `pico.layer.empty(_,"window", …)`                 |
| `tst/win-layer-draw.c`            | drawing on `window` produces same pixels as old `'w'`-mode drawing         |
| `lua/tst/win-layer-draw.lua`      | visual regression of mouse + draw with `window` layer                      |
| `tst/set-layer-required.c`        | `pico_set_layer(NULL)` asserts; `pico.set.layer()` errors               |

## Verification

```bash
# 1. C tests
make tests

# 2. Lua tests
cd lua/ && make tests && cd ..

# 3. Visual smoke set (after asr/ regen)
make int T=mouse
make int T=mouse-rect-click
make int T=tiles
make int T=cv
make int T=layers

# 4. Predefined layers
pico-lua lua/tst/predef-layers.lua
make test T=predef-layers
```

After all changes settle, update `valgrind.supp` `sdl-init`
clause's `src:pico.c:N` to the new `SDL_Init` line in
`pico_init`.

## Out of scope

- `up` parameter on layer creators stays (Decision 3).
- Doc rewrite (api.md, guide) — track in
  `2026-05-layer-utils.md` "Backlog" (mode table, layer-arg
  rewording).
- Any `Mode` type unification in `lua/doc/api.md`.

## Implications summary

- API surface contracts by ~40 parameters
  (24 state + 15 CV/VS/mouse + 1 implicit `layer` on 1 mouse helper).
- `'w'` becomes invalid input;
  six `_win_*` helpers + two public `pico_cv_pos_*_win` deleted.
- Two layer names (`world`, `window`) are now reserved.
- All call sites that relied on
  *"don't switch layer, just pass the name explicitly"*
  must be rewritten as
  `set.layer(L); call(); set.layer(prev)` —
  i.e. callers pay the cost of being explicit about state.

## Progress

- [x] Drop `layer` param from state get/set — **34 funcs done** (8 pencil + 12 effect + 14 scene)
- [x] Delete `_pico_layer_null` (brought forward to fix unused-function build error)
- [x] `pico_set_layer` returns previous layer name (`const char*`); 8 C tests + 3 Lua tests adopt the `old = set(X); …; set(old)` pattern
- [x] Rename root → world: enum `PICO_LAYER_WORLD`, field `G.world`, string `"world"` — applied to `src/`, `tst/*.c`, `lua/`, `lua/doc/guide.md`
- [x] Make `pico_set_layer(NULL)` an error (NULL fast-path removed; `memcmp` segfaults naturally in `realm_find` — Decision 9, "let it crash")
- [x] Make `pico.set.layer()` / `pico.set.layer(nil)` an error (`l_set_layer` uses `luaL_checkstring`)
- [x] Migrate `pico_set_layer(NULL)` / `pico.set.layer(nil|())` test sites to `"world"`
- [ ] ~~Drop `base`/`layer` param from CV/VS/mouse helpers~~ — **deferred** per Decision 5; only migrate non-NULL callers to NULL where possible
- [ ] Add `window` (window target) at init
- [ ] Migrate screenshot non-NULL `base` (`src/pico.c:1658`) to use `window` layer
- [ ] Reject reserved names in layer creators (`world`, `window`)
- [ ] Delete `'w'` mode branches in `aux.hc`
- [ ] Delete `_*_win_to_wld` / `_*_wld_to_win` (6 helpers)
- [ ] Delete `pico_cv_pos_rel_win` / `pico_cv_pos_win_rel`
- [ ] Migrate mouse / `'w'` test sites (table above)
- [ ] Regenerate `asr/` for visual tests
- [ ] Add 5 new tests
- [ ] Update `valgrind.supp:sdl-init` line `N`

## Next iteration: Step B+D combined (world rename + NULL → realm error)

User direction: hard rename **everywhere** (string, enum, C field,
comments, vars) AND centralize NULL-as-error in the realm
(single check site). Step D's `pico_set_layer(NULL)` → error
fold-in is therefore folded into step B.

### Decisions (added now)

| #  | Question                              | Choice                                      |
|----|---------------------------------------|---------------------------------------------|
| 6  | Hard rename or alias?                 | **hard rename** — `"root"` stops working    |
| 7  | C field name capitalization           | **`G.world`** (lowercase, matches C convention) |
| 8  | Enum / string keep uppercase          | `PICO_LAYER_WORLD`, `"world"` (matches public name) |
| 9  | Single place for NULL key check       | **none added** — let `memcmp(NULL, …)` segfault naturally inside `realm_find`. Stack trace points to the misuse; no need for a polite assert. |
| 10 | Lua scope                             | **included** — Lua tests + `lua/doc/guide.md` migrate too |

### A. C source rename + NULL guard

| file              | place                               | change                                                     |
|-------------------|-------------------------------------|------------------------------------------------------------|
| src/layers.hc     | 5                                   | `PICO_LAYER_ROOT,` → `PICO_LAYER_WORLD,`                     |
| src/pico.c        | `G` struct (~39)                    | rename `Pico_Layer root;` field → `Pico_Layer wld;`        |
| src/pico.c        | 283                                 | `.type = PICO_LAYER_ROOT` → `PICO_LAYER_WORLD`               |
| src/pico.c        | 284                                 | `.name = "root"` → `.name = "world"`                         |
| src/pico.c        | 320–321                             | `realm_put(..., "root", &G.root)` → `..., "world", &G.wld`   |
| src/pico.c        | ~25 sites (`G.root.* ` / `&G.root`) | bulk replace `G.root` → `G.world`                            |
| src/pico.c        | 656–673 (`pico_set_layer`)          | drop `if (key == NULL) S.layer = &G.wld;` fast-path; let `realm_get` assert |
| src/pico.c        | 282–321 init comment                | `// realm_get("root") resolves` → `"world"`                  |
| src/pico.c        | assert messages mentioning "root"/"main" | freshen wording (`"can only set dim from main layer"` etc.) |
| src/pico.h        | any doc lines mentioning root       | swap to `world`                                              |

### B. C tests

| file                              | sites                 | change                                                |
|-----------------------------------|-----------------------|-------------------------------------------------------|
| tst/keep.c                        | 13, 14                | `"root"` → `"world"`                                    |
| tst/layer-hier.c                  | 6 (comment), 16, 32, 44, 53, 64 | same                                        |
| tst/layers.c                      | 12, 16, 18, 40, 52, 59, 61, 84, 106 | string `"root"` → `"world"`; `set_layer(NULL)` → `set_layer("world")` |
| tst/clear_alpha.c                 | 22                    | `set_layer(NULL)` → `set_layer("world")`                |
| tst/sheet.c                       | 27                    | same                                                  |
| tst/video.c                       | 32                    | same                                                  |
| tst/image_raw.c                   | 83                    | same                                                  |
| tst/todo/mouse-rect.c             | 12                    | same                                                  |
| tst/todo/mouse-rect-click.c       | 19                    | same                                                  |

### C. Lua tests + guide

| file                              | sites                                                   | change                          |
|-----------------------------------|---------------------------------------------------------|---------------------------------|
| lua/tst/guide.lua                 | 291, 296                                                | `"root"` → `"world"`              |
| lua/tst/layers.lua                | 8, 14, 57                                               | `"root"` → `"world"`              |
| lua/tst/layer-hier.lua            | 12, 26, 30, 36, 40, 49, 54, 60, 67, 74, 76, 95          | `"root"` → `"world"` (×12)        |
| lua/doc/guide.md                  | 847, 852                                                | `"root"` → `"world"`              |
| lua/tst/clear_alpha.lua, layer-empty-tile.lua, sheet.lua, video.lua, rot-flip.lua, todo/mouse-layer.lua | various | `pico.set.layer(nil)` / `pico.set.layer()` → `pico.set.layer("world")` |
| lua/pico.c (`l_set_layer`)        | ~973                                                    | reject nil/no-arg with `luaL_error` (Decision 2 of master plan) |

### What remains untouched

- `&G.wld` pointer references — pointer semantics unchanged.
- `up=NULL` on layer creators (Decision 3).
- `Pico_Abs_Rect* base=NULL` (Decision 5).
- `font=NULL`, `pico_get_image(path, NULL)`, `pico_input_event(NULL, …)`,
  and other genuine semantic NULLs — none of these touch realm or layers.

### NULL elimination — explicit answer

This combined step removes **exactly one** NULL-meaning:
`pico_set_layer(NULL) ≡ "go to root"`.
After this:

- `pico_set_layer(NULL)` aborts at `realm_find`'s assert.
- `pico.set.layer(nil)` / `pico.set.layer()` errors at the Lua wrapper.

Other NULLs listed in the master plan stay (`up`, `base`, `font`,
image-dim override, event-target). These are **not** "current-layer"
sugar — they encode genuine optionality.

### Why no explicit NULL check?

- `realm_find` already calls `memcmp(e->key, key, n)` — passing `NULL`
  segfaults at first hash collision (or earlier in `realm_djb2`).
- Stack trace already points at the caller; an `assert` would just be
  prettier output, not better diagnostics.
- Removing `pico_set_layer`'s NULL fast-path still simplifies that
  function.

### Verification (combined step B+D)

```bash
# 1. C tests (32 files; 7 touched above; rest unaffected)
make tests

# 2. Lua tests
cd lua/ && make tests && cd ..

# 3. Negative test — confirm NULL aborts
# (manual: pico_set_layer(NULL) should fire realm assert)
```

Pass criteria:
- All 32 C tests + Lua test suite pass.
- `pico_set_layer(NULL)` segfaults inside `realm_find` (let-it-crash).
- No remaining `"root"` literals in `src/`, `tst/`, `lua/`.

### Notes from execution

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
  rewrite to `pico_set_scene_keep(1)` was behavior-preserving.
- During the world-rename sweep, `lua/tst/guide.lua:306` had a user
  text layer keyed `"world"` (matching the literal `"World!"`
  content). After the rename, this collided with the predefined
  `"world"` slot at realm_put time. Resolved by renaming the user
  layer to `"World"` (capital W) — case-sensitive realm lookup
  keeps it distinct.
- Lua wrapper `l_set_layer` was simplified twice in the same step:
  first to drop the NULL fast-path (`luaL_checkstring`), then to
  expose `pico_set_layer`'s return value (`lua_pushstring; return 1`).

## Next iteration: Architectural shift — `window` as the new root

User direction (2026-05-07): elevate `window` to a real layer and
**make it the root** of the hierarchy. `world` keeps its name and
default-current status, but structurally it becomes a child of
`window` like any other layer.

### Decisions (added)

| #  | Question                                  | Choice                                  |
|----|-------------------------------------------|-----------------------------------------|
| 11 | Is `window` really a layer?               | **yes** — but a degenerate one: many `Pico_Layer` fields don't apply (see § Field applicability below). Holds a layer's worth of state with several no-ops. |
| 12 | What does `pico_set_layer("window")` do?  | `SDL_SetRenderTarget(G.ren, NULL)` — draws straight to the window framebuffer |
| 13 | Does `world` survive?                     | **yes** — auto-created child of `window` at `pico_init`; same role as today (default current after init), only structurally now a child rather than the root |
| 14 | Predefined enum                           | add `PICO_LAYER_WINDOW`; keep `PICO_LAYER_WORLD` |
| 15 | Reserve `"window"` against user creators  | **inherit current `"world"` behavior** — register with realm mode `'!'`. `pico_layer_*(_, "window", …)` aborts via the same `realm: exclusive key exists` assert that already protects `"world"`. **No explicit creator guards.** Pre-existing `'='`-mode foot-gun (silently returns the predefined slot) is left as-is — same risk for both reserved keys. |
| 16 | `Pico_Window` ↔ `Pico_Layer` relation     | **contains** (composition, not subtype prefix): `struct Pico_Window { Pico_Layer layer; int fs; int show; const char* title; }`. Realm registers `&G.window.layer`. Explicit C `has-a` over implicit `is-a`. |
| 17 | Migrate `Pico_Window`'s `color`/`dim`     | move into `G.window.layer`: `color` → `effect.color`, `dim` → `scene.dim`. `fs`/`show`/`title` stay on `Pico_Window` — pure SDL window state. |
| 18 | Desktop-as-parent unification             | **out of scope** for now. Conceptually, `effect.alpha` ↔ `SDL_SetWindowOpacity`, `scene.dst` ↔ `SDL_SetWindowPosition`+`SDL_SetWindowSize`, `Pico_Window.fs` ↔ `scene.dst = full desktop`. Wiring those is a follow-up; this iteration only models window as a render target. |

### Field applicability for `window`

The window has no parent inside pico (the OS desktop is its conceptual parent, but pico doesn't model that yet — Decision 18). Several `Pico_Layer` fields are no-ops:

| field                   | window | reason                                                |
|-------------------------|--------|-------------------------------------------------------|
| `effect.flip`/`rotate`  | N/A    | OS WM doesn't flip/rotate windows                     |
| `effect.alpha`          | N/A *  | * unless wired to `SDL_SetWindowOpacity` (Decision 18) |
| `scene.dst`             | N/A *  | * unless wired to `SDL_SetWindow{Position,Size}`       |
| `scene.src`             | N/A    | can't show only a slice of our window to the OS       |
| `scene.keep`            | N/A    | no compositor we control                              |
| `hier.up`               | N/A    | no parent in pico's tree                              |
| `hier.nxt`              | N/A    | siblings = other apps' windows, outside scope         |

Fields that **do** apply: `name`, `type`, `tex (=NULL)`, `pencil`, `effect.color` (clear color), `effect.grid`, `scene.dim`, `scene.tile`, `scene.clip`, `hier.dn` (child list).

For `world`: every `Pico_Layer` field applies (it's a fully-typical layer). Window is degenerate; world is canonical.

### Architecture

Before:

```
G.world  (PICO_LAYER_WORLD, tex≠NULL, name="world")  ← root
  └─ user layers
```

After:

```
G.window (PICO_LAYER_WINDOW, tex=NULL, name="window") ← new root
  └─ G.world  (PICO_LAYER_WORLD, tex≠NULL, name="world")  ← default child
      └─ user layers
```

### A. C source

| file              | place                            | change                                                              |
|-------------------|----------------------------------|---------------------------------------------------------------------|
| `src/layers.hc`   | enum `PICO_LAYER`                | add `PICO_LAYER_WINDOW`                                             |
| `src/pico.h`      | `Pico_Window` typedef            | restructure: `struct Pico_Window { Pico_Layer layer; int fs; int show; const char* title; }`. Drop `Pico_Color color` and `Pico_Abs_Dim dim` from the struct — read them off `layer.effect.color` and `layer.scene.dim`. |
| `src/pico.c`      | `G` struct                       | replace `Pico_Layer world; SDL_Window* win;` with `Pico_Window window; SDL_Window* sdl_win; Pico_Layer world;` (or similar). Realm key `"window"` resolves to `&G.window.layer`. |
| `src/pico.c`      | `pico_init` (~282)               | initialize `G.window.layer` (`name="window"`, `tex=NULL`, `scene.dim` from physical window dim, `type=PICO_LAYER_WINDOW`) and `G.window.{fs,show,title}` from SDL state; register `&G.window.layer` in realm; `_layer_attach("window","world")` so world is its child |
| `src/pico.c`      | `pico_get_window` / `pico_set_window` | adapt to new `Pico_Window` shape: read/write `.layer.effect.color`, `.layer.scene.dim`, `.fs`, `.show`, `.title` |
| `src/pico.c`      | `S.win.{color,dim,fs}`           | retire — fold `color` and `dim` into `G.window.layer`; `fs` into `G.window.fs`. (`S.win` either disappears or shrinks.) |
| `src/pico.c`      | `pico_init` (~310)               | `S.layer = &G.world` (unchanged: world is still the default current after init) |
| `src/pico.c`      | `pico_set_layer`                 | no special-case needed — `SDL_SetRenderTarget(G.ren, S.layer->tex)` already does the right thing when `tex==NULL` (renders to window framebuffer) |
| `src/pico.c`      | `_pico_output_present` (~1547–1623) | retire the bespoke world→window blit; the scene-graph traverse now naturally composites world (and any sibling/child of window) onto the window framebuffer |
| `src/pico.c`      | `_layer_traverse` start          | start from `&G.window` (was `&G.world`) for the present walk        |
| `src/pico.c`      | resize handler                   | when window resizes, update `G.window.scene.dim` to match           |
| `src/pico.c`      | asserts `S.layer == &G.world`    | keep — world remains the "logical main" for present-/dim-required ops |
| `src/pico.c:1658` | screenshot non-NULL `base`       | replace with `pico_set_layer("window"); pico_cv_rect_rel_abs(rect, NULL); pico_set_layer(prev);` (eliminates the only non-NULL CV `base` caller in src) |

The big content of A is **retiring the present() special-case**.
Right now `_pico_output_present` does:

1. `_layer_traverse(&G.world)` (children → world.tex)
2. `SDL_SetRenderTarget(NULL); SDL_RenderClear` (window background)
3. Custom aux-clip + `SDL_RenderCopy(G.world.tex, src, dst)` (world.tex → window framebuffer with src/dst transform)
4. `SDL_RenderPresent`

After the shift:

1. `_layer_traverse(&G.window)` walks one level deeper:
   - SetRenderTarget(window.tex=NULL); clear with window color
   - For each child (including world): `_pico_output_draw_layer(child, NULL)` blits onto window framebuffer using the child's `scene.dst` / `scene.src`
2. `SDL_RenderPresent`

The aux-clipping in step 3 is **subsumed** by the existing
`_pico_output_draw_layer` when world's `scene.dst` is set to fill
window dim. Need to verify edge cases (out-of-bounds src/dst when
user shifts world's scene); if behavior diverges, port aux-clip
into `_pico_output_draw_layer`'s code path.

### B. Tests

| file                              | change                                                              |
|-----------------------------------|---------------------------------------------------------------------|
| `tst/window-layer.c` (new)        | predefined slot exists; `pico_set_layer("window")` switches; `pico_layer_empty(_, "window", …)` rejected; drawing on window goes to physical pixels |
| `lua/tst/window-layer.lua` (new)  | mirror                                                              |
| Existing tests                    | no changes — all use `world` (default current) and any user layers; `window` is opt-in |

### C. Visual regression risk

- Auto-composite walk changes (start from `window` instead of `world`).
  If `_pico_output_draw_layer` semantics diverge from the bespoke
  aux-clipping in current `present`, world rendering shifts and
  many `asr/` images need regeneration.
- Mitigation: spot-check 2–3 visual tests with non-trivial
  `scene.dst`/`scene.src` (e.g. `view-target.c`, `clip_pct.c`)
  before bulk-regenerating.

### B'. Event-name rename `PICO_EVENT_WIN_*` → `PICO_EVENT_WINDOW_*`

Bundled with this iteration since `win` is being phased out as an
abbreviation. Lua-facing event tags get the same treatment.

| file              | place                                  | change                                   |
|-------------------|----------------------------------------|------------------------------------------|
| `src/events.h`    | enum (line 10)                         | `PICO_EVENT_WIN_RESIZE` → `PICO_EVENT_WINDOW_RESIZE` |
| `src/pico.c`      | 3 sites (1000, 1117, 1133)             | enum-name update                         |
| `lua/pico.c`      | 1341 (case in event dispatch)          | enum-name update                         |
| `lua/pico.c`      | 1778 (register integer)                | enum-name update                         |
| `lua/pico.c`      | 1342, 1779 (Lua-facing event tag)      | `"win.resize"` → `"window.resize"`       |
| Lua tests + docs  | any handler matching `"win.resize"`    | sweep                                    |

Search guard: `grep -rE 'PICO_EVENT_WIN_|"win\.resize"' src/ lua/ tst/`
should be empty after the rename.

### D. Out-of-scope follow-ups (still pending)

- `'w'` mode + 6 `_win_*` helpers + `pico_cv_pos_*_win` deletion
  (plan step C of master plan) — needs `window` layer first; then
  callers move to `set.layer("window")` + mode `'!'`.
- CV/VS `base` arg removal (Decision 5) — still deferred.
- **Desktop-as-parent unification** (Decision 18) — wire
  `G.window.layer.effect.alpha` → `SDL_SetWindowOpacity`,
  `G.window.layer.scene.dst` → `SDL_SetWindow{Position,Size}`, and
  fold `Pico_Window.fs` into `scene.dst = full desktop`. After
  that, `Pico_Window` collapses to `Pico_Layer + SDL_Window*`.

### Verification

```bash
make tests
cd lua/ && make tests && cd ..
make int T=window-layer
make int T=view-target
make int T=clip_pct
```

Pass criteria:
- All existing C + Lua tests pass with no `asr/` regen, OR a small
  documented regen list (max 3–5 visual tests) due to traverse-walk
  change.
- New `window-layer` tests assert the predefined slot's existence
  and behavior.
