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
- [ ] ~~Drop `base`/`layer` param from CV/VS/mouse helpers~~ — **deferred** per Decision 5; only migrate non-NULL callers to NULL where possible
- [ ] Add `world` (rename root) and `window` (window target) at init
- [ ] Migrate screenshot non-NULL `base` (`src/pico.c:1658`) to use `window` layer
- [ ] Reject reserved names in layer creators
- [ ] Make `pico_set_layer(NULL)` / Lua no-arg an error
- [ ] Delete `'w'` mode branches in `aux.hc`
- [ ] Delete `_*_win_to_wld` / `_*_wld_to_win` (6 helpers)
- [ ] Delete `pico_cv_pos_rel_win` / `pico_cv_pos_win_rel`
- [ ] Migrate mouse / `'w'` test sites (table above)
- [ ] Migrate `pico_set_layer(NULL)` / `set.layer()` test sites to `"world"`
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
