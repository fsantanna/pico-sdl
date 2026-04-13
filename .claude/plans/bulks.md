# Bulk Getters/Setters + Named Initializers

## Goals

1. Switch all struct init to designated (named) initializers
2. Bulk getters/setters receive `T*` instead of individual args
3. Lua provides only bulk getters/setters, no per-field
4. Review all groups for consistency

## Decisions

1. **Bulk setter = full replace.** No partial update, no
   sentinel values. Individual setters remain for convenience.
2. **`dim` is part of bulk view.** Bulk `pico_set_view` always
   recreates texture. Use individual `pico_set_view_dst` etc.
   to avoid texture recreation.

## 1. Named initializers

Two sites use positional initializers for
`Pico_Layer_Draw`, `Pico_Layer_Show`, `Pico_Layer_View`:

| File | Place | Current |
|---|---|---|
| `src/pico.c:256` | `G.root.draw` | positional |
| `src/pico.c:257` | `G.root.show` | positional |
| `src/mem.hc:98-99` | `_layer_new .draw` | positional |
| `src/mem.hc:101-102` | `_layer_new .show` | positional |
| `src/mem.hc:104-108` | `_layer_new .view` | positional |

Switch all to designated: `.alpha=`, `.color=`, etc.

`Pico_Layer_View` in `G.root` (pico.c:259-265) already uses
designated — keep as-is.

## 2. Bulk signatures → struct

### New bulk signatures

```c
void pico_get_show(const char* layer,
                   Pico_Layer_Show* show);
void pico_set_show(const char* layer,
                   Pico_Layer_Show show);

void pico_get_draw(const char* layer,
                   Pico_Layer_Draw* draw);
void pico_set_draw(const char* layer,
                   Pico_Layer_Draw draw);

void pico_get_view(const char* layer,
                   Pico_Layer_View* view);
void pico_set_view(const char* layer,
                   Pico_Layer_View view);
```

Getter returns full struct via pointer.
Setter receives struct by value (full replace).
Both call `_pico_output_present(0)`.

Individual setters remain unchanged.

### Export structs to pico.h

`Pico_Layer_Draw`, `Pico_Layer_Show`, `Pico_Layer_View`
must move from `src/layers.hc` to `src/pico.h` so users
can construct them for bulk set/get.

### Call sites for old bulk signatures

| File | Function | Caller |
|---|---|---|
| `lua/pico.c:828` | `pico_get_show` | `l_get_show` |
| `lua/pico.c:1123` | `pico_set_show` | `l_set_show` |
| `tst/get-set.c:99` | `pico_set_show` | test |
| `tst/get-set.c:107` | `pico_get_show` | test |
| `tst/get-set.c:116` | `pico_set_show` | restore |
| `tst/get-set.c:55` | `pico_set_draw` | test |
| `tst/get-set.c:60` | `pico_get_draw` | test |
| `tst/get-set.c:66` | `pico_set_draw` | restore |

## 3. Lua: only bulk getters/setters

### Remove individual Lua functions

| Current Lua API | Action |
|---|---|
| `pico.get.font()` | remove → `pico.get.draw().font` |
| `pico.get.style()` | remove → `pico.get.draw().style` |
| `pico.get.color.draw()` | remove → `pico.get.draw().color` |
| `pico.get.color.clear()` | remove → `pico.get.show().color` |
| `pico.set.font(f)` | remove → `pico.set.draw{font=f}` |
| `pico.set.style(s)` | remove → `pico.set.draw{style=s}` |
| `pico.set.color.draw(c)` | remove → `pico.set.draw{color=c}` |
| `pico.set.color.clear(c)` | remove → `pico.set.show{color=c}` |

### Add bulk draw getter/setter

- `pico.get.draw()` → returns `{color, font, style}`
- `pico.set.draw{color=c, font=f, style=s}`

### Add missing show fields to Lua

- `pico.get.show()` → add `color`, `keep`
- `pico.set.show{...}` → add `color`, `keep`

### Add missing view fields to Lua getter

- `pico.get.view()` → uncomment `target`, `source`, `clip`

### Registration tables after cleanup

```
ll_get = { draw, image, keyboard, layer, mouse, now,
           show, text, video, view, window }
ll_set = { dim, draw, expert, layer, mouse, show,
           video, view, window }
```

Remove `ll_get_color`, `ll_set_color` tables entirely.

### Lua test migration

- `pico.set.color.draw(c)` → `pico.set.draw{color=c}`
- `pico.set.color.clear(c)` → `pico.set.show{color=c}`
- `pico.set.font(f)` → `pico.set.draw{font=f}`
- `pico.set.style(s)` → `pico.set.draw{style=s}`
- `pico.get.color.draw()` → `pico.get.draw().color`
- `pico.get.color.clear()` → `pico.get.show().color`

## Steps

### Step A: named initializers [x]
- [x] `src/pico.c` | `G.root.draw`, `G.root.show`
- [x] `src/mem.hc` | `_layer_new` `.draw`, `.show`, `.view`

### Step B: export structs + bulk C signatures [x]
- [x] `src/pico.h` | move structs from `layers.hc`
- [x] `src/pico.h` | new bulk declarations
- [x] `src/pico.c` | new bulk implementations
- [x] `tst/get-set.c` | update bulk tests
- [x] `lua/pico.c` | update `l_get/set_show`, `l_get/set_view`
- [ ] compile + test

### Step C: Lua bulk draw + complete show/view [x]
- [x] `lua/pico.c` | add `l_get_draw`, `l_set_draw`
- [x] `lua/pico.c` | add color/keep to show getter+setter
- [x] `lua/pico.c` | view getter: target/source/clip via
  `L_push_rel_rect` helper
- [x] `lua/pico.c` | registration: `draw` in `ll_get`/`ll_set`
- [x] show getter fields alpha-ordered: alpha, color, flip,
  grid, keep, rotate
- [ ] compile + test

### Step D: remove Lua individual setters/getters
- [ ] `lua/pico.c` | remove individual functions
- [ ] `lua/pico.c` | remove `ll_get_color`, `ll_set_color`
- [ ] update registration tables
- [ ] migrate all Lua tests + docs
- [ ] compile + test

### Step E: update docs
- [ ] `lua/doc/api.md`
- [ ] `lua/doc/guide.md`
- [ ] `lua/doc/gen-guide-images.lua`

## TODO

- Lua test: use bulk combined with `pico.set { ... }`:
  ```lua
  pico.set {
      view = {...},
      draw = {...},
  }
  ```
