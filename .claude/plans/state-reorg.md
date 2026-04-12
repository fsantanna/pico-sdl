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

## Two state categories

| category  | struct       | what                                     |
|-----------|--------------|------------------------------------------|
| internal  | `G`          | init, realm, ren, win, root, presenting  |
| per-layer | `Pico_View`  | all drawing + compositing state          |

**`S` shrinks** to only:
- `Pico_Layer* layer` — current render target
- `struct { int on; int fps; int ms; int t0; } expert`
- `struct { Pico_Abs_Dim dim; int fs; } win`

**`Pico_View` gains** (from S):
- `Pico_Color draw` — RGBA draw color (was `S.color.draw` +
  `S.alpha`)
- `Pico_Color clear` — RGBA clear color (was `S.color.clear`)
- `PICO_STYLE style` — fill/stroke (was `S.style`)
- `const char* font` — font path (was `S.font`)

**`Pico_View` keeps**:
- `dim`, `dst`, `src`, `clip`, `tile`, `rot`, `flip`, `grid`
- `unsigned char alpha` — composite-only

**`Pico_View` adds**:
- `unsigned char keep` — skip post-composite clear

**Drop entirely**:
- `Pico_State` struct
- `STACK` struct
- `pico_push()` / `pico_pop()`
- `pico_set_alpha()` / `pico_get_alpha()` — alpha is `.a` of
  draw color
- `pico_set_color_clear_alpha()` / `pico_get_color_clear_alpha()`
  — clear is already RGBA

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
| `pico_get_show_rotation(layer)`   | `pico_set_show_rotation(layer,rot)`  |
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
| `Pico_View.rot`        | `Pico_View.rotation`      |

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

### 2. Move draw state to `Pico_Draw` on `Pico_Layer` [x]

- [x] `src/pico.c` | new `Pico_Draw` type | `{ color,
  alpha, style, font }` — separate `alpha` field kept
  (avoids `pico_set_alpha`/`pico_set_color_draw` clobber
  until Step 4 merges into `.color.a`)
- [x] `src/layers.hc` | `Pico_Layer` struct | added
  `Pico_Draw draw;` field
- [x] `src/mem.hc` | `_layer_new` | init `.draw` defaults
  (white, alpha=0xFF, FILL, font=NULL)
- [x] `src/pico.c` | `S` struct | removed `alpha`,
  `color.draw`, `font`, `style`; kept `color_clear`
- [x] `src/pico.c` | `G.root` init | added `.draw` defaults
- [x] `src/pico.c` | set/get alpha, color_draw, style, font
  | redirect to `S.layer->draw.*`
- [x] `src/pico.c` | all primitives | `S.color.draw.*` →
  `S.layer->draw.color.*`; `S.alpha` →
  `S.layer->draw.alpha`; `S.style` →
  `S.layer->draw.style`
- [x] `src/pico.c` | `Pico_State` → `_Stack_Entry` | save/
  restore `S.layer->draw` + `S.color_clear` + `S.layer`
- [x] `src/layers.hc` | `_pico_layer_text` + draw_layer |
  `S.font` → `S.layer->draw.font`; composite alpha uses
  `S.layer->draw.alpha`
- [x] `src/mem.hc` | `_tex_text` | `S.color.draw.*` →
  `S.layer->draw.color.*`; `S.font` →
  `S.layer->draw.font`
- [x] Compile + test: `make tests` (C) and Lua both pass

### 3. Collapsed into Step 2 [x]

Clear color moved to `Pico_View.color`. `keep` deferred to
Step 6. `Pico_Draw.alpha` removed (alpha lives in
`draw.color.a`). Push/pop/stack removed entirely.
`pico_set/get_alpha`, `pico_set/get_color_clear_alpha`
removed from C and Lua. Tests updated: `tst/push.c` deleted,
`lua/tst/push.lua` deleted, blend tests use `color.a`,
`valgrind.supp` line updated.

### ~~3.~~ Add `clear` and `keep` to `Pico_View`

- `src/pico.c` | `Pico_View` | add
  `Pico_Color clear; unsigned char keep;`
- `src/mem.hc` | `_view_new` |
  `.clear = {0,0,0,0xFF}, .keep = 0`
- `src/mem.hc` | each `_alloc_layer_*` | set per-type `keep`:
  empty→0, buffer→1, image→1, sub→1, text→1, video→1
- `src/pico.c` | `pico_output_clear` | read
  `S.layer->view.clear`
- `src/pico.c` | `pico_set_color_clear` | write to
  `S.layer->view.clear`
- Remove `S.color.clear` from S
- Compile + test

### ~~4.~~ Drop push/pop (collapsed into Step 2)

- `src/pico.c` | remove `Pico_State`, `STACK`,
  `pico_push`, `pico_pop`
- `src/pico.h` | remove declarations
- `src/pico.c` | remove `pico_set_alpha` / `pico_get_alpha`
- `src/pico.h` | remove declarations
- `tst/push.c` | remove or rewrite (test no longer applies)
- `tst/*.c` | remove all `pico_push()` / `pico_pop()` calls;
  replace `pico_set_alpha(a)` with
  `pico_set_color_draw({r,g,b,a})`
- Compile + test

### 5. Add new layer setters / shrink `pico_set_view`

- `src/pico.h` | new declarations: `pico_set_layer_alpha`,
  `pico_set_layer_keep`, `pico_set_layer_rot`,
  `pico_set_layer_flip`, `pico_set_layer_grid` + getters
- `src/pico.c` | implementations
- `src/pico.h` | `pico_set_view` sig: 9 → 5 params
- `src/pico.h` | `pico_get_view` sig: same
- `src/pico.c` | implementations; internal callers updated
- `tst/*.c` | 9-arg calls → 5-arg + individual setters
- Compile + test

### 6. Post-composite clear via `view.keep`

- `src/layers.hc` | `_layer_traverse` | after composite:
  if `!CUR->view.keep && type != SUB`, clear texture
  using `CUR->view.clear`
- `src/pico.c` | `_pico_output_present` | clear root if
  `!G.root.view.keep`
- Compile + test

## Files

| file            | changes                                         |
|-----------------|-------------------------------------------------|
| `src/colors.h`  | unify types; constants → RGBA                   |
| `src/pico.h`    | signatures; remove push/pop/alpha; new setters  |
| `src/pico.c`    | Pico_View gains draw/clear/keep/style/font;     |
|                 | S loses draw state; remove push/pop/stack;      |
|                 | primitives read from view; set/get_view shrinks |
| `src/mem.hc`    | _view_new defaults; per-type keep               |
| `src/layers.hc` | post-composite clear in _layer_traverse         |
| `tst/*.c`       | color type, push/pop removal, set_view shrink   |
| `valgrind.supp` | bump sdl-init line N                            |

## Struct field renames

| struct       | old   | new        |
|--------------|-------|------------|
| `Pico_View`  | `rot` | `rotation` |

## Verification

- `make tests` after each step
- `make int T=layer-hier` — verify composite + alpha
- `tst/clear_alpha.c` — per-layer clear color
- `tst/rot-flip.c` — layer rot/flip setters
- `tst/blend_*.c` — draw alpha via RGBA color
- Verify no `pico_push`/`pico_pop` references remain
