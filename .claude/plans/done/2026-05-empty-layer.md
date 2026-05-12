# 2026-05 empty-layer — DONE

Two breaking changes to `pico_layer_empty`:

1. Add `int clear` parameter (sets `scene.clear`).
2. Switch `dim` from `Pico_Abs_Dim` to `Pico_Rel_Dim`.

## Why

- `clear` is currently hardcoded to `1` in `_alloc_layer_empty`
  (`src/mem.hc:144`).
    - Callers wanting `clear=0` must call `pico_set_scene_clear`
      after creation. Make it primary.
- `Pico_Abs_Dim` forces pixel sizing.
    - Rel dim lets the empty layer be sized as a fraction of its
      parent (`%`), of the parent's tile grid (`#`), or as raw
      pixels (`!`).
    - Example: `pico.layer.empty('up', 'me', true, {'%',w=.5,h=.5})`

## Decisions

| topic         | choice                                            |
|---------------|---------------------------------------------------|
| % base        | parent (`up`) layer's `scene.dim`; NULL up = current `G.layer` |
| tile          | stays `Pico_Abs_Dim` (grid count, not a size)     |
| Lua dim form  | keep `{mode, w=, h=}` (no array shorthand)        |

## C API

### Before

```c
void pico_layer_empty (
    const char* up, const char* key,
    Pico_Abs_Dim dim, Pico_Abs_Dim* tile
);
void pico_layer_empty_mode (
    int mode,
    const char* up, const char* key,
    Pico_Abs_Dim dim, Pico_Abs_Dim* tile
);
```

### After

```c
void pico_layer_empty (
    const char* up, const char* key,
    int clear,
    Pico_Rel_Dim dim, Pico_Abs_Dim* tile
);
void pico_layer_empty_mode (
    int mode,
    const char* up, const char* key,
    int clear,
    Pico_Rel_Dim dim, Pico_Abs_Dim* tile
);
```

## Implementation

### `src/mem.hc`

- `_alloc_empty_t` (line 13):
    - `dim` field: `Pico_Abs_Dim` -> `Pico_Rel_Dim`
    - add `int clear`
    - add `const char* up` (to look up % base parent)
- `_alloc_layer_empty` (line 137):
    - resolve `arg->dim` to `Pico_Abs_Dim`:
        - if `arg->up != NULL`: look up parent via
          `realm_get(G.realm, arg->up)` and use its `scene.dim`
          as `Pico_Abs_Rect` base
        - if `arg->up == NULL`: use `G.layer->scene.dim` (the
          current layer)
        - resolve with `_sdl_dim` / `_rnd_dim`
    - apply tile multiplier on the resolved abs dim (same as
      today)
    - pass `arg->clear` to `_layer_new` (replaces hardcoded `1`)

### `src/pico.c`

- `pico_layer_empty` (line 760): add `int clear` arg, change
  `dim` type, forward to `_mode`.
- `pico_layer_empty_mode` (line 765): same; build
  `_alloc_empty_t` including `clear` and `up`.

### `src/pico.h`

- Update prototypes (lines 369-378) and doc comments — note
  that `dim` mode `%` resolves against `up`'s scene.dim, and
  that `clear` sets `scene.clear`.

### `lua/pico.c`

- `l_layer_empty` (line 1128):
    - parse `clear` as bool argument **between key and dim**
    - parse `dim` with `C_rel_dim(L, i+3)` instead of manual
      `{w,h}` extraction
    - forward to `pico_layer_empty_mode(m, up, key, clear,
      dim, ptr)`

New Lua signature:

```lua
pico.layer.empty([mode,] up, key, clear, dim [, tile])
-- ex: pico.layer.empty('up', 'me', true, {'%',w=.5,h=.5})
```

## Progress

### Done

- [x] `src/pico.h` — prototypes + docs.
- [x] `src/mem.hc` — `_alloc_empty_t` + `_alloc_layer_empty`
      (rel→abs via `_sdl_dim`, base from `realm_get(up)`).
- [x] `src/pico.c` — `pico_layer_empty` + `_mode` (forward
      `up`+`clear` into the ctx).
- [x] `lua/pico.c` — `l_layer_empty` (parses `clear` bool +
      `C_rel_dim`).
- [x] Compile-checked: `gcc -c src/pico.c -I src` clean;
      `lua/Makefile -> pico.o` clean.
- [x] `valgrind.supp` `sdl-init` still at `pico.c:117` (no
      change needed).

### Done — test caller updates

Preserve prior runtime behavior:

- old default was `clear = 1` (hardcoded in
  `_alloc_layer_empty`) → used `1` in C / `true` in Lua.
- old `dim` was pixel-typed → wrapped with mode `'!'`.

Plan-listed callers (all done):

| file                          | line(s)        |
|-------------------------------|----------------|
| tst/keep.c                    | 17, 18         |
| tst/layers.c                  | 22, 29         |
| tst/layers.c                  | 77, 83 (_mode) |
| tst/layer-empty-tile.c        | 16, 48         |
| lua/tst/layers.lua            | 18, 25         |
| lua/tst/cv.lua                | 27             |
| lua/tst/layer-empty-tile.lua  | 12, 44         |

Additional callers missed by plan (found via grep, all done):

| file                              | line(s)        |
|-----------------------------------|----------------|
| tst/clear_alpha.c                 | 18             |
| tst/shot.c                        | 82             |
| tst/mouse-rect-click.c            | 13             |
| tst/view-target.c                 | 46, 65         |
| tst/sheet.c                       | 9              |
| tst/cv.c                          | 40             |
| tst/layer-hier.c                  | 16, 64         |
| tst/vs.c                          | 84             |
| tst/layer-clip.c                  | 83             |
| tst/tile-grid.c                   | 82, 96, 110    |
| tst/mouse-w-click.c               | 11             |
| tst/todo/mouse-rect-click.c       | 11             |
| tst/todo/mouse-rect.c             | 7              |
| lua/tst/sheet.lua                 | 6              |
| lua/tst/vs.lua                    | 51, 72, 73     |
| lua/tst/tile-grid.lua             | 73, 86, 99     |
| lua/tst/view-target.lua           | 44, 63         |
| lua/tst/todo/mouse-layer.lua      | 6              |
| lua/tst/guide.lua                 | 240, 300       |
| lua/tst/mouse-rect-click.lua      | 22             |
| lua/tst/layer-hier.lua            | 12, 67         |
| lua/tst/clear_alpha.lua           | 14             |
| lua/tst/layer-clip.lua            | 81             |
| lua/tst/todo/mouse-rect-click.lua | 10             |
| lua/tst/shot.lua                  | 78             |

`lua/doc/gen-guide-images.lua:299,305` — was 2-arg form
that never matched the old C signature (pre-existing
breakage). Updated to new sig:
`pico.layer.empty(nil, "bg"|"flag", true, {'!', w=, h=})`.

### Done — verify

- [x] `make tests` (C) — all passed, no `asr/` mods.
- [x] `cd lua/ && make tests` (Lua) — all passed.
- [x] Spot-check (skipped per user instruction; tests cover
      the regression visually).

## Phase 2: Lua-only rect convenience (post-base-refactor)

After the C-level breaking change shipped, a second wave of
Lua-only ergonomics was added. **No further C API changes.**

### Feature A: rect-as-dim shortcut for `pico.layer.empty`

If the 4th arg is a Rect (table with `x` field) instead of a
Dim, the wrapper derives the layer's Dim from its `mode/w/h`
AND sets the new layer's `scene.target` to the full rect.

```lua
pico.layer.empty(nil, "panel", true,
    {'%', x=0.5, y=0.5, w=0.5, h=0.5, anchor='C'})
-- 50%×50% layer placed centered, in one call
```

Implementation:
- `C_rel_dim(L, i+3)` reads `mode/w/h` regardless of whether
  the table is a Dim or Rect (Rect has these fields too).
- `L_opt_target(L, i+3, key)` detects Rect (by `x` field) and
  applies `scene.dst` to the new layer.

### Feature B: trailing optional rect on all other ctors

`image`, `pixmap`, `text`, `video`, `sub` accept an optional
trailing `Rect` that sets `scene.target` of the new layer.
`images` (which generates multiple sub-layers) is excluded.

```lua
pico.layer.image("world", "img", "open.png",
    {'%', x=0.7, y=0.7, w=0.4, h=0.4, anchor='C'})

pico.layer.sub("world", "blue", "buf",
    {'!', x=0, y=1, w=1, h=1, anchor='NW'},   -- crop
    {'%', x=0.20, y=0.9, w=0.10, h=0.10})     -- target
```

For `image`/`video` (which already support an optional
implicit `key`), the trailing rect requires an explicit
`key` (the wrapper can't set `scene.dst` on an unnamed
layer).

### Feature C: uniform check-and-set helper

```c
// If slot `i` holds a Rect (table with `x`), set layer
// `key`'s scene.target. No-op otherwise. Preserves cur.
static void L_opt_target (lua_State* L, int i, const char* key);
```

Each layer ctor ends with a single call:
`L_opt_target(L, slot, key);`

- `empty`: slot `i+3` (the dim/rect slot)
- `image`/`video`: slot `i+3` (when explicit key)
- `pixmap`: slot `i+3`
- `text`: slot `i+4`
- `sub`: slot `i+4`

Safe when `key` is NULL (image/video implicit-key case): the
helper short-circuits before touching `key` because the slot
will either be absent or non-table.

### Feature D: `w`/`h` default to 0 in `C_rel_rect` / `C_rel_dim`

`Pico_Rel_Rect` semantics: `w=0` or `h=0` means "infer from
source aspect" (already exercised by view-target-05/06/07).
The Lua parsers now default missing fields to 0 — matching
the C convention and unlocking concise rect forms like
`{'%', x=0.3, y=0.3, w=0.4}` (h inferred).

- New helper `C_optfieldnum(L, i, k)` — sibling of
  `C_checkfieldnum`, returns 0 on missing, errors on
  non-numeric.
- `C_rel_rect`: x/y stay required (`C_checkfieldnum`); w/h
  optional (`C_optfieldnum`).
- `C_rel_dim`: w/h optional.
- `L_dim_default_wh` helper removed entirely; its 9 call
  sites also removed (defaulting now lives in the parsers).

Trade-off: typos on `w=`/`h=` no longer raise — silently
default to 0.

### Tests added / simplified

- `lua/tst/layers.lua` — new programmatic tests:
  - rect-as-dim shortcut for `empty` (verifies dim
    derivation + `scene.target`).
  - pixmap trailing-rect (verifies `scene.target`).
- `lua/tst/view-target.lua` — new visual test
  `view-target-08` (identical baseline to `view-target-04`)
  demonstrating the rect-as-dim shortcut.
- `lua/tst/layer-hier.lua` — 5 cases collapsed to one-call:
  image (02), pixmap (03), text (04), sub×2 (06).
- `lua/tst/guide.lua` — 3 cases in §7.4 collapsed using
  the w/h-omitted Rect form: image (line 295), text×2
  (305, 310).

### Lessons

- Many callers were missed by the initial plan inventory.
  Always `grep` for the symbol across the whole tree before
  declaring a refactor scope.
- Don't `cd lua/` in Bash — the cwd persists across calls
  and `lua/` lacks `.claude/one`, which wedges all subsequent
  Bash/Edit/Write. Use `make -C lua tests` instead.
- Slot indices in C wrappers are easy to mis-type. The
  `i+3` vs `i+2` bug in `l_layer_pixmap` slipped through
  static review because the wrong slot happened to be a
  table (the pixmap arg), so the rect-detect check passed
  silently. Visual regression caught it.

### Notes for cross-machine handoff

- Tool-call hook expects `.claude/one` or `.claude/all` to
  exist in the worktree; without it, all Bash/Edit/Write
  calls fail with "edits are blocked". Recreate the marker
  on the new machine before resuming (`touch .claude/one`).
- All source-side changes are in tree (uncommitted); all
  callers updated and tests pass.

## Resolved items

- Confirm `realm_get(G.realm, up)` returns the parent
  `Pico_Layer*` (read `scene.dim` from it).
  Verified: same pattern at `src/pico.c:810`
  (`pico_layer_sub_mode`).
- Confirm `_sdl_dim` accepts `Pico_Rel_Dim*` +
  `Pico_Abs_Rect*` base. Verified: `src/aux.hc:43`.
- `lua/doc/gen-guide-images.lua` form: updated to new
  signature.
- Visual spot-checks: skipped per user instruction
  (visual coverage exists via regression tests).
- C tests last re-run: green just before the
  `C_optfieldnum` refactor. Refactor itself only added
  field-defaulting (callers already pass `w`/`h`), so no
  C-side behavioral change.
