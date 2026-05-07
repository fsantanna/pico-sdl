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
The window becomes a predefined layer (`"WIN"`),
the world/root becomes a predefined layer (`"WLD"`).

## Decisions (locked)

| # | Question                          | Choice                              |
|---|-----------------------------------|-------------------------------------|
| 1 | `WIN` role                        | drawable render target              |
| 2 | `pico.set.layer()` no-arg         | error / required                    |
| 3 | layer-creation `up` parameter     | keep explicit (out of scope)        |
| 4 | NULL layer arg                    | moot — argument removed entirely    |
| 5 | CV/VS `Pico_Abs_Rect* base` arg   | **keep for now** — real semantic override, not dead code. Proactively migrate non-NULL callers to NULL where a layer can replace the explicit base (e.g. screenshot → WIN). Revisit dropping the param later. |

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
Mouse migration still happens as part of plan C (`'w'` → `WIN`).

### B. Predefined layers `WLD` and `WIN`

Created in `pico_init`, never destroyable, names reserved:

| name  | type           | tex                       | purpose                        |
|-------|----------------|---------------------------|--------------------------------|
| `WLD` | existing root  | (current `G.root`)        | world/root render target       |
| `WIN` | new            | `NULL` → `SDL_SetRenderTarget(ren, NULL)` selects window | physical-pixel target |

`G.root` keeps its struct slot but its `name` becomes `"WLD"`.
`pico.set.layer("WIN")` calls `SDL_SetRenderTarget(NULL)` and uses
window dim as scene.dim — output and input both work in window
pixels.

`_pico_layer_name("WLD" | "WIN")` resolves the predefined slots;
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
delete — callers express the same thing by setting layer = `WIN`
and using mode `'!'`.

Mouse: `pico_get_mouse(char mode, …)` keeps mode arg (still wants
`'!' '%' '#'`) but no longer accepts `'w'` —
to read window-pixel mouse, set layer = `WIN` first, mode = `'!'`.

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
| `src/pico.c` `pico_init`     | layer setup                            | name root `"WLD"`, create predefined `"WIN"` slot          |
| `src/pico.c` `pico_set_layer`| body                                   | special-case `"WIN"` → `SDL_SetRenderTarget(NULL)`; assert non-NULL |
| `src/pico.c` `pico_get_mouse`| `'w'` branch                           | delete                                                     |
| `src/pico.c` `pico_set_mouse`| body                                   | drop `pico_cv_pos_rel_win` call; resolve through `S.layer` |
| `src/aux.hc:43,53,63,73,83,89`| `_*_win_to_wld`, `_*_wld_to_win`      | delete (6 helpers)                                         |
| `src/aux.hc:106-342`         | `case 'w'` in `_sdl_*` / `_rel_*`      | delete                                                     |
| `src/layers.hc:34,61-66`     | `_pico_layer_null`                     | delete decl + body                                         |
| `src/layers.hc` create funcs | `_pico_layer_pixmap/image/text/sub`    | reject keys `"WLD"` and `"WIN"`                            |
| `lua/pico.c`                 | `l_get_*` / `l_set_*` wrappers         | drop the `NULL` first arg in C calls (~24 sites)           |
| `lua/pico.c` `l_set_layer`   | body                                   | error if no string arg                                     |
| `lua/pico.c` `C_mode_opt`    | mode set                               | drop `'w'` from accepted modes                             |
| `valgrind.supp`              | `sdl-init` clause                      | update `src:pico.c:N` after `pico_init` line shift         |

## Tests

### Will fail (must be migrated)

| file                              | line(s)                            | symptom / fix                                                                          |
|-----------------------------------|------------------------------------|----------------------------------------------------------------------------------------|
| `lua/tst/mouse.lua`               | 10,17,24,38,45,52,66,73,94,101,110 | `pico.set.mouse({'w', …})` → `pico.set.layer("WIN"); pico.set.mouse({'!', …})`         |
| `lua/tst/mouse-rect-click.lua`    | 36,44,51,59,66,74,81,89            | same — both `set.mouse({'w', …})` and `output.draw.pixel({'w', …})`                    |
| `lua/tst/mouse-rect-click.lua`    | 18,40,52                           | `pico.set.layer()` (NULL) → `pico.set.layer("WLD")`                                    |
| `lua/tst/tiles.lua`               | 10,40,80                           | `set.mouse({'w', …})` → set `WIN` layer + `'!'`                                        |
| `lua/tst/cv.lua`                  | 71                                 | `{'w', w=250, h=500}` → use `WIN` layer + `'!'`                                        |
| `lua/tst/layers.lua`              | 16,18                              | `pico.set.layer()` → `pico.set.layer("WLD")`                                           |
| `lua/tst/todo/mouse-rect-click.lua` | —                                | `pico.get.mouse('w')` → `pico.set.layer("WIN")` + `pico.get.mouse('!')`                |
| `lua/tst/todo/mouse-layer.lua`    | —                                  | same as above                                                                          |
| `tst/mouse-w-click.c`             | 17,34,41                           | rename test to `mouse-win.c`; `'w'` → layer `"WIN"` + `'!'`; `pico_set_layer(NULL)` → `"WLD"` |
| `tst/mouse.c`                     | 157,169,178                        | drop `pico_set_mouse('%')` (already removed); `pico_get_mouse('w', …)` → `WIN` + `'!'` |
| `tst/tiles.c`                     | 9,66,76,85                         | same pattern as `mouse.c`                                                              |
| `tst/cv.c`                        | mode 'w' loop                      | substitute `WIN` layer iteration                                                       |
| `tst/layers.c`                    | 16,40,52,59                        | `pico_set_layer(NULL)` → `pico_set_layer("WLD")`                                       |

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
| `tst/predef-layers.c`             | `pico_get_layer()` after init = `"WLD"`; `WIN` exists; both reserved    |
| `lua/tst/predef-layers.lua`       | same on Lua side; reject `pico.layer.empty(_,"WIN", …)`                 |
| `tst/win-layer-draw.c`            | drawing on `WIN` produces same pixels as old `'w'`-mode drawing         |
| `lua/tst/win-layer-draw.lua`      | visual regression of mouse + draw with `WIN` layer                      |
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
- Two layer names (`WLD`, `WIN`) are now reserved.
- All call sites that relied on
  *"don't switch layer, just pass the name explicitly"*
  must be rewritten as
  `set.layer(L); call(); set.layer(prev)` —
  i.e. callers pay the cost of being explicit about state.

## Progress

- [x] Drop `layer` param from state get/set — **34 funcs done** (8 pencil + 12 effect + 14 scene)
- [x] Delete `_pico_layer_null` (brought forward to fix unused-function build error)
- [ ] ~~Drop `base`/`layer` param from CV/VS/mouse helpers~~ — **deferred** per Decision 5; only migrate non-NULL callers to NULL where possible
- [ ] Add `WLD` (rename root) and `WIN` (window target) at init
- [ ] Migrate screenshot non-NULL `base` (`src/pico.c:1658`) to use `WIN` layer
- [ ] Reject reserved names in layer creators
- [ ] Make `pico_set_layer(NULL)` / Lua no-arg an error
- [ ] Delete `'w'` mode branches in `aux.hc`
- [ ] Delete `_*_win_to_wld` / `_*_wld_to_win` (6 helpers)
- [ ] Delete `pico_cv_pos_rel_win` / `pico_cv_pos_win_rel`
- [ ] Migrate mouse / `'w'` test sites (table above)
- [ ] Migrate `pico_set_layer(NULL)` / `set.layer()` test sites to `"WLD"`
- [ ] Regenerate `asr/` for visual tests
- [ ] Add 5 new tests
- [ ] Update `valgrind.supp:sdl-init` line `N`

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
