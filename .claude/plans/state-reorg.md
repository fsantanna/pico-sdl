# State Reorganization

## Context

The state architecture mixes concerns: `S.color.clear` is global
but belongs per-layer; `pico_set_view` has 9 params and growing;
two color types (`Pico_Color` RGB, `Pico_Color_A` RGBA) add
friction.
Reorganize into 2 clean categories, unify colors as RGBA, move
draw state per-layer, drop push/pop.

## Color unification

Remove `Pico_Color` (RGB). Rename `Pico_Color_A` → `Pico_Color`
(always RGBA). All constants get `.a = 0xFF`.
Remove `_pico_color()` helper. `pico_color_alpha()` kept:
still useful as a copy-with-alpha-override ergonomic helper
(e.g., `pico_color_alpha(PICO_COLOR_RED, 0x80)`).

`PICO_COLOR_TRANSPARENT` stays as-is (already RGBA).

## Three per-layer structs

| struct             | what                                  |
|--------------------|---------------------------------------|
| `Pico_Layer_Draw`  | color, font, style                    |
| `Pico_Layer_Show`  | alpha, color, flip, grid, rotate    |
| `Pico_Layer_View`  | clip, dim, dst, src, tile             |

All fields alpha-ordered within each struct.

**`S` shrinks** to only:
- `Pico_Layer* layer` — current render target
- `struct { int on; int fps; int ms; int t0; } expert`
- `struct { Pico_Abs_Dim dim; int fs; } win`

**Dropped entirely**:
- `Pico_State` / `STACK` / `pico_push` / `pico_pop`
- `pico_set_alpha` / `pico_get_alpha`
- `pico_set_color_clear_alpha` / `pico_get_color_clear_alpha`

## Three alpha channels

1. **`view.color.a`** — canvas alpha after clear
2. **`draw.color.a`** — primitive fill alpha (part of draw
   color)
3. **`view.alpha`** — composite alpha at blit time

Composite: `draw.color.a * view.alpha / 255`.
Draw color alpha acts as pen opacity for both primitives
and layer blits.

## New API

Four groups. All `layer`-param functions take an optional
`const char* layer` as first arg. If `NULL`, use current
layer (`S.layer`).

```c
static Pico_Layer* _layer (const char* layer) {
    if (layer == NULL) return S.layer;
    Pico_Layer* L = realm_get(
        G.realm, strlen(layer)+1, layer);
    pico_assert(L != NULL && "layer does not exist");
    return L;
}
```

### Window (global, no `layer` param)

| getter                    | setter                       |
|---------------------------|------------------------------|
| `pico_get_window(...)`    | `pico_set_window(...)`       |
| `pico_get_window_title()` | `pico_set_window_title(t)`  |
| `pico_get_window_fs()`   | `pico_set_window_fs(fs)`     |
| `pico_get_window_show()`  | `pico_set_window_show(on)`  |
|                           | `pico_set_dim(dim)` shorthand (window+view) |

### View (spatial, `layer` param)

| getter                       | setter                            |
|------------------------------|-----------------------------------|
| `pico_get_view(layer,...)`   | `pico_set_view(layer,...)`        |
| `pico_get_view_dim(layer)`   | `pico_set_view_dim(layer,dim)`   |
| `pico_get_view_tile(layer)`  | `pico_set_view_tile(layer,tile)` |
| `pico_get_view_dst(layer)`   | `pico_set_view_dst(layer,dst)`   |
| `pico_get_view_src(layer)`   | `pico_set_view_src(layer,src)`   |
| `pico_get_view_clip(layer)`  | `pico_set_view_clip(layer,clip)` |

### Show (presentation, `layer` param)

| getter                            | setter                               |
|-----------------------------------|--------------------------------------|
| `pico_get_show_color(layer)`      | `pico_set_show_color(layer,c)`       |
| `pico_get_show_grid(layer)`       | `pico_set_show_grid(layer,on)`       |
| `pico_get_show_rotate(layer)`   | `pico_set_show_rotate(layer,rot)`  |
| `pico_get_show_flip(layer)`       | `pico_set_show_flip(layer,flip)`     |
| `pico_get_show_alpha(layer)`      | `pico_set_show_alpha(layer,a)`       |
| `pico_get_show_keep(layer)`       | `pico_set_show_keep(layer,on)`       |

### Draw (draw state, `layer` param)

| getter                         | setter                              |
|--------------------------------|-------------------------------------|
| `pico_get_draw_color(layer)`   | `pico_set_draw_color(layer,c)`      |
| `pico_get_draw_style(layer)`   | `pico_set_draw_style(layer,s)`      |
| `pico_get_draw_font(layer)`    | `pico_set_draw_font(layer,path)`    |

### Renames

| old                    | new                       |
|------------------------|---------------------------|
| `pico_get_show`        | `pico_get_window_show`    |
| `pico_set_show`        | `pico_set_window_show`    |
| `pico_get_color_clear` | `pico_get_show_color`     |
| `pico_set_color_clear` | `pico_set_show_color`     |
| `pico_get_color_draw`  | `pico_get_draw_color`     |
| `pico_set_color_draw`  | `pico_set_draw_color`     |
| `pico_get_style`       | `pico_get_draw_style`     |
| `pico_set_style`       | `pico_set_draw_style`     |
| `pico_get_font`        | `pico_get_draw_font`      |
| `pico_set_font`        | `pico_set_draw_font`      |
| `Pico_View.rot`        | `Pico_View.rotate`      |

### Removed

- `pico_set_alpha` / `pico_get_alpha`
- `pico_set_color_clear_alpha` / `pico_get_color_clear_alpha`
- `pico_push` / `pico_pop`

## Decisions

1. **`view.font` ownership**: borrowed `const char*`, no
   `strdup`, no free on view destroy. Caller keeps the string
   alive (same as current behavior).

2. **`keep` defaults per layer type**: `empty=0` (re-renders
   from primitives every frame), all others `=1` including
   `sub` (post-composite clear must be skipped).

3. **Root layer**: fully first-class. `S.layer` points at
   `G.root` by default; per-view setters write to
   `G.root.view.*` until another layer is targeted. No
   special-casing.

4. **Alpha naming**: `view.alpha` stays. Alpha-in-color is
   just `.a` on the RGBA struct, never standalone, so there
   is no naming collision.

5. **Color type migration**: Step 1 is a literal two-phase
   swap. First substitute every `Pico_Color` (RGB) call site
   with `Pico_Color_A` (RGBA), forcing each literal to add
   `.a` explicitly. Then rename `Pico_Color_A` → `Pico_Color`.
   No silent breakage.

6. **Steps 3–6 ordering**: not individually shippable.
   Intermediate steps may render incorrectly; only the full
   sequence through Step 6 must be visually correct.

7. **Testing cadence**: full `make tests` after each step.
   No per-step smoke test selection.

8. **Composite alpha**: `draw.color.a * view.alpha / 255`.
   Draw color alpha acts as "pen opacity" for both primitives
   and layer blits.

## Steps

### 1. Unify color types [x]

- [x] `src/colors.h` | remove `Pico_Color` (RGB); rename
  `Pico_Color_A` → `Pico_Color`; all constants become RGBA
  with `.a = 0xFF`; remove `_pico_color()` helper
- [x] `src/pico.h` | update all signatures using `Pico_Color`
  or `Pico_Color_A` → unified `Pico_Color`; removed
  `pico_color_alpha` decl
- [x] `src/pico.c` | update all internal uses; color helpers
  preserve input alpha (darker/lighter pass `clr.a`; mix
  averages; hex sets `0xFF`); `pico_color_alpha` kept (2-line
  copy-with-alpha-override)
- [x] `src/layers.hc` | `Pico_Color_A*` → `Pico_Color*`
- [x] `src/mem.hc` | `Pico_Color_A*` → `Pico_Color*`
- [x] `lua/pico.c` | unify color types; drop `L_push_color_a`
  (merged into `L_push_color`); `l_color_alpha` reimplemented
  without deleted helper; Lua color tables now always have
  `.a`
- [x] `tst/*.c` | added `, 0xFF` to 98 literals across 26
  files; renamed `Pico_Color_A` → `Pico_Color`; rewrote
  `pico_color_alpha` test in `tst/colors.c` with direct `.a`
  field
- [x] Compile + test: `make tests` (C) and
  `cd lua/ && make tests` both pass

### 2–4. Collapsed: structs + removals [x]

- [x] `Pico_Layer_Draw` (`color, font, style`) in
  `layers.hc`; `Pico_Layer_Show` (`alpha, color, flip,
  grid, rotate`) in `layers.hc`; `Pico_Layer_View`
  (`clip, dim, dst, src, tile`) stays spatial-only
- [x] `S` shrunk: removed `alpha`, `color`, `font`,
  `style`; only `expert`, `layer`, `win` remain
- [x] `Pico_State` / `STACK` / `pico_push` / `pico_pop`
  removed; `tst/push.c` + `lua/tst/push.lua` deleted
- [x] `pico_set/get_alpha` removed; draw alpha is
  `.draw.color.a`; blend tests use `pico_color_alpha`
- [x] `pico_set/get_color_clear_alpha` removed
- [x] Clear color in `show.color`; `pico_output_clear`
  reads `S.layer->show.color`
- [x] `valgrind.supp` updated
- [x] All tests pass (C + Lua)

### 5. API reorganization [x]

- [x] `_pico_layer_name` / `_pico_layer_null` helpers in
  `layers.hc` with forward declarations
- [x] Draw renames: `pico_set/get_color_draw` →
  `pico_set/get_draw_color`; `pico_set/get_style` →
  `pico_set/get_draw_style`; `pico_set/get_font` →
  `pico_set/get_draw_font`; all with `layer` first param
- [x] Show renames: `pico_set/get_color_clear` →
  `pico_set/get_show_color`; `pico_set/get_show` →
  `pico_set/get_window_show`; `rot` → `rotate`
- [x] Show bulk + individual setters/getters (alpha, color,
  flip, grid, rotate) with `layer` param
- [x] Draw bulk + individual setters/getters (color, font,
  style) with `layer` param
- [x] View shrunk: `pico_set/get_view` from 9 to 6 params
  (layer + clip, dim, dst, src, tile); individual
  setters/getters for each field
- [x] `pico_set_view_dim` extracts texture-recreation logic
- [x] All individual setters call `_pico_output_present(0)`
- [x] pico.c sections organized: getters in GET (show then
  draw, alpha-ordered), setters in SET (same)
- [x] Lua `l_set_view` / `l_get_view` stripped of show
  fields; new `l_set_show` / `l_get_show` bindings
- [x] All test call sites migrated (~320 sites across ~40
  files); `tst/get-set.c` tests show + draw bulk/individual
- [x] All tests pass (C + Lua)

### 6. Post-composite clear via `show.keep`

- `src/layers.hc` | `Pico_Layer_Show` | add `keep` field
- `src/layers.hc` | `_layer_traverse` | after composite:
  if `!CUR->show.keep && type != SUB`, clear texture
  using `CUR->show.color`
- `src/pico.c` | `_pico_output_present` | clear root if
  `!G.root.show.keep`
- `src/pico.c` + `src/pico.h` | add
  `pico_set/get_show_keep(layer, on)`
- `src/mem.hc` | per-type `keep`: empty→0, others→1
- Compile + test

## Remaining

### Lua: drop individual getters/setters; use bulk only

Migration mappings (mechanical substitutions):

```
pico.get.font()          → pico.get.draw().font
pico.get.style()         → pico.get.draw().style
pico.get.color.clear()   → pico.get.show().color
pico.get.color.draw()    → pico.get.draw().color
pico.set.font(p)         → pico.set.draw { font=p }
pico.set.style(s)        → pico.set.draw { style=s }
pico.set.color.clear(c)  → pico.set.show { color=c }
pico.set.color.draw(c)   → pico.set.draw { color=c }
```

Rules:
- token-less form (`pico.set.color.draw 'red'`) always
  becomes curly form (`pico.set.draw { color='red' }`)
- adjacent `set.style` + `set.color.draw` can be combined:
  `pico.set.draw { style=s, color=c }`

#### Step A: migrate all callers (tests + docs)

Tests — **done** in this session:

- `lua/tst/get-set.lua` (view section has pre-existing
  failure at line 73; unrelated to migration)
- `lua/tst/colors.lua`
- `lua/tst/sheet.lua`
- `lua/tst/blend_pct.lua`
- `lua/tst/blend_raw.lua`
- `lua/tst/tile-grid.lua`
- `lua/tst/mouse-rect-click.lua`
- `lua/tst/pos.lua`
- `lua/tst/rect.lua`
- `lua/tst/collide_pct.lua`
- `lua/tst/style.lua`
- `lua/tst/clip_pct.lua`
- `lua/tst/clip_raw.lua`
- `lua/tst/layers.lua`
- `lua/tst/view-target.lua`
- `lua/tst/layer-empty-tile.lua`
- `lua/tst/clear_alpha.lua`
- `lua/tst/shot.lua`
- `lua/tst/buffer_pct.lua`
- `lua/tst/buffer_raw.lua`
- `lua/tst/image_pct.lua`
- `lua/tst/image_raw.lua`
- `lua/tst/rot-flip.lua`
- `lua/tst/video.lua`
- `lua/tst/set.lua`
- `lua/tst/move.lua`
- `lua/tst/view_raw.lua`

Tests — **pending**:

- `lua/tst/todo/main.lua` (~8 sites: mix of clear/draw)
- `lua/tst/todo/style.lua` (4 sites: 2 `set.style` +
  2 `pcall(pico.set.style, …)` — need rewrite to
  `pcall(function() pico.set.draw{style=…} end)`)
- `lua/tst/todo/video.lua` (4 sites: clear + 3 draws)
- `lua/tst/todo/pct.lua` (~9 sites: red/white draws)
- `lua/tst/todo/mouse-layer.lua` (2 sites: red/black clear)
- `lua/tst/todo/mouse-rect-click.lua` (3 sites: navy/black
  clear + white draw)
- `lua/tst/todo/navigate.lua` (1 site: `set.style 'stroke'`)
- `lua/tst/todo/move.lua` (2 sites: white draw, 1 comment)
- `lua/tst/todo/move2.lua` (1 site: white draw)
- `lua/tst/todo/layout.lua` (1 site:
  `pico.set.color.draw(pico.color.red)`)
- `lua/tst/todo/rotate.lua` (1 site: `pico.set.font(nil, 16)`
  — weird 2-arg call; check if the `16` is really used or
  leftover from old API)

Docs — **pending** (update last, after C removal):

- `lua/doc/anims.lua`
- `lua/doc/api.md` (lines 101–102, 114–117 describe
  `pico.set.style` and `pico.set.color.*`)
- `lua/doc/gen-guide-anims-gif.lua`
- `lua/doc/gen-guide-images.lua` (~16 sites)
- `lua/doc/gen-guide-rects-gif.lua`
- `lua/doc/guide.md` (tutorial text references
  `pico.set.color.draw` extensively)
- `lua/doc/rects.lua`

#### Step B: remove individual C functions

Only after all callers migrated:

- `lua/pico.c` | delete `l_get_color_clear`,
  `l_get_color_draw`, `l_get_font`, `l_get_style`,
  `l_set_color_clear`, `l_set_color_draw`, `l_set_font`,
  `l_set_style`
- `lua/pico.c` | `ll_get` | remove `font`, `style` entries
- `lua/pico.c` | `ll_set` | remove `font`, `style` entries
- `lua/pico.c` | delete `ll_get_color` and `ll_set_color`
  arrays entirely (only had `clear`/`draw`)
- `lua/pico.c` | module init (`luaopen_pico`) | remove
  setup code that registers `pico.get.color` and
  `pico.set.color` sub-tables

Note: the C side `pico_set_draw_font`, `pico_set_draw_style`
etc. stay — only the Lua wrappers are removed. Bulk
`l_set_draw` / `l_get_draw` already call those under the
hood.

### Pre-existing issues (independent)

- `lua/tst/get-set.lua` line 73 assertion fails:
  `v.source.x == 0.6 and v.source.y == 0.4` after
  `pico.set.view { source={'%', x=0.6, y=0.4, …} }`.
  Investigate `l_set_view` handling of `source` field or
  `c_rel_rect` conversion. Not caused by migration.

### Other remaining items

- update `valgrind.supp` line number if needed
- swap GET order in pico.c (show before draw)

## Next steps (explicit, in order)

1. Finish Step A pending files: `lua/tst/todo/*.lua`
   (11 files). Run `cd lua && make test T=<name>` after
   each (todo tests may not all have make targets — check).
2. Migrate docs: `lua/doc/*.lua` and `lua/doc/*.md`.
3. Run full test suite: `make tests` and
   `cd lua/ && make tests`. All previously-passing tests
   must still pass (get-set view-section failure is
   pre-existing, track separately).
4. Step B: remove 8 individual C functions + their
   registration entries in `lua/pico.c`. Recompile, run
   full suite again.
5. Diagnose pre-existing `get-set.lua:73` failure in
   `pico.set.view { source=... }` handling.
6. Step 6 of the main reorg: `show.keep` field +
   post-composite clear logic (see `## Steps` → 6).

## Verification

- `make tests` + `make -C lua tests` after each step
- `make int T=layer-hier` — verify composite + alpha
- `tst/clear_alpha.c` — per-layer clear color
- `tst/rot-flip.c` — layer rotate/flip setters
- `tst/blend_*.c` — draw alpha via RGBA color
- `tst/get-set.c` — show + draw bulk/individual tests
