# View Refactoring Plan

## Goal

Refactor view system to move global state into views, clarify coordinate
semantics, and fix navigation aids.

---

## 1. Move Global State to View

Currently global in `S` (pico.c:71-87):
- `S.crop` - cropping region for textures
- `S.alpha` - alpha transparency (0-255)
- `S.angle` - rotation angle (stored but NOT used in rendering)
- `S.flip` - NOT implemented

**Move to `Pico_View`:**
- alpha, angle, flip → per-view attributes
- crop removed → use src instead

---

## 2. Independent Grid per Layer

Currently grid is only rendered for the main layer.
The `view.grid` flag exists in `Pico_View` but is ignored for non-main layers.

**Action:**
- Render grid overlay for each layer with `view.grid` enabled
- Grid should show the layer's own coordinate system

---

## 3. Remove `_crop()` - Use `view.src` Instead

- Remove `S.crop` from global state
- Remove `pico_set_crop()` and `pico_get_crop()` API
- Use `layer->view.src` for layer drawing

---

## 4. Clarify `%` Mode Semantics

**For win/log dimensions - REJECT `%` mode:**
- Nothing to be relative to at the top level

**For dst/src/clip - `%` relative to win/log:**
- `dst`: relative to window dimensions
- `src`/`clip`: relative to world dimensions
- NOT relative to itself

---

## 5. Store src/dst/clip as Pico_Rel_Rect in Pico_View

**Current** (pico.c:39-46):
```c
typedef struct {
    Pico_Abs_Dim  dim;
    Pico_Abs_Rect dst;    // always absolute
    int           grid;
    Pico_Abs_Rect src;    // always absolute
    Pico_Abs_Rect clip;   // always absolute
    Pico_Abs_Dim  tile;
} Pico_View;
```

**Proposed:**
```c
typedef struct {
    Pico_Abs_Dim  dim;
    Pico_Rel_Rect dst;    // stores mode + values
    int           grid;
    Pico_Rel_Rect src;    // stores mode + values
    Pico_Rel_Rect clip;   // stores mode + values
    Pico_Abs_Dim  tile;
} Pico_View;
```

**Default:** `{'%', {0, 0, 1, 1}, PICO_ANCHOR_NW, NULL}` (full extent)

**Access points (~34 places):**

| Category | Lines | Count |
|----------|-------|-------|
| Struct definition | 39-46 | 1 |
| Init (main layer) | 525-528 | 3 |
| Init (other layers) | 541, 994-1034, 1073, 1137 | ~5 |
| `pico_set_view()` stores | 854, 869, 874, 887, 890 | 5 |
| `pico_set_window()` dst | 936, 949 | 2 |
| `SDL_RenderSetClipRect` | 577, 817, 904, 1707, 1777 | 5 |
| Mouse transform | 671-676 | 2 |
| Navigation reads | 1201-1263 | 6 |
| Grid labels | 1617, 1629 | 2 |
| Present render | 1695-1696 | 2 |
| Draw layer dst | 1408 | 1 |

**Approach:**
1. Change struct fields
2. `pico_set_view()`: store `Pico_Rel_Rect` directly
3. Convert to `SDL_Rect` via `_sdl_rect()` + `_fi_rect()` where SDL needs it
4. Default init: `{'%', {0, 0, 1, 1}, PICO_ANCHOR_NW, NULL}`

---

## 6. Fix Navigation Aids

With `Pico_Rel_Rect` in view, navigation becomes simple read-modify-write:

```c
case SDLK_LEFT: {
    Pico_Rel_Rect src = S.layer->view.src;
    src.x -= 0.1;
    pico_set_view(-1, NULL, NULL, &src, NULL, NULL);
    break;
}

case SDLK_MINUS: {
    // Zoom out - keep center, expand by 10%
    Pico_Rel_Rect src = S.layer->view.src;
    float cx = src.x + src.w/2;
    float cy = src.y + src.h/2;
    src.w += 0.1;
    src.h += 0.1;
    src.x = cx - src.w/2;
    src.y = cy - src.h/2;
    pico_set_view(-1, NULL, NULL, &src, NULL, NULL);
    break;
}
```

**Navigation operations:** pico.c:1188-1281

---

## 7. Fix `tst/todo_*` Files

| File | Main Issues |
|------|-------------|
| `todo_rain.c` | Deprecated types, `pico_output_draw_rect_pct()` |
| `todo_rotate.c` | `pico_set_view_raw()`, old types |
| `todo_hide.c` | Missing window/view setup |
| `todo_scale.c` | `pico_set_scale()`, `pico_set_anchor_pos()` |
| `todo_control.c` | Wrong text drawing API |
| `todo_main.c` | Deprecated types throughout |

---

## 8. Implementation Order

1. **Remove `_crop()` and use `view.src`** [DONE]
   - Removed S.crop, _crop(), pico_get/set_crop from pico.c/pico.h
   - Replaced _crop() in _pico_output_draw_layer with pico_cv_rect_rel_abs(&layer->view.src, ...)
   - Removed l_set_crop from lua/pico.c and lua/doc/api.md
   - Rewrote tst/image_raw.c and lua/tst/image_raw.lua crop tests to use named layer + view.src
   - NOTE: image_raw-07 assertion diverges — old image lacked metric rules because
     the global _crop() leaked into _show_grid's pico_output_draw_text_raw calls,
     corrupting the metric labels. New per-layer approach fixes this bug.
     Update assertion: cp tst/out/image_raw-07.png tst/asr/
   - Files: pico.c, pico.h, lua/pico.c, lua/doc/api.md, tst/image_raw.c, lua/tst/image_raw.lua

2. **Reject `%` mode for win/log** [DONE]
   - Files: pico.c:882, pico.c:943

2b. **Fix `size_pct` tests** [DONE]
   - Files: tst/size_pct.c, lua/tst/size_pct.lua

3. **Fix `%` for src to be relative to world** [DONE]
   - Changed src '%' from `&view.src` to `NULL`
   - Files: pico.c:863

4. **Fix navigation to use read-compute-set** [DONE]
   - Uses `'!'` mode with world-relative delta
   - Files: pico.c:1188-1281

5. **Store src/dst/clip as Pico_Rel_Rect** [DONE]
   - Changed Pico_View struct fields to Pico_Rel_Rect
   - Read sites use `pico_cv_rect_rel_abs` with explicit base
   - pico_set_view stores Pico_Rel_Rect directly
   - pico_get_view now returns dst/src/clip (removed assert)
   - Default: {'%', {.5,.5,1,1}, PICO_ANCHOR_C, NULL}
   - Files: pico.c

6. **Fix navigation to use '%' read-modify-write** [DONE]
   - Mode-agnostic: convert to `%`/C, inc/dec, convert back via `pico_cv_rect_rel_rel`
   - Also: fixed merge errors, replaced remaining `_sdl_dim`+`_fi_dim`, simplified 7 `pico_cv_rect_rel_abs` sites to `NULL` base
   - Files: pico.c

7. **Independent grid per layer**
   - Files: pico.c (`_pico_output_present()`)

8. **Move rotate/flip to view** [DONE]
   - Added `Pico_Pct` typedef as alias for `Pico_Anchor` (pico.h)
   - Added `Pico_Rot` struct with angle and anchor (pico.h:90-93)
   - Added rot/flip fields to `Pico_View` struct (pico.c:46-47)
   - Removed `angle` and `crop` from `Pico_State` struct (pico.c:91-100)
   - Fixed `pico_push()` and `pico_pop()` to not use removed fields (pico.c)
   - Added rot/flip defaults to layer creation functions:
     `_pico_layer_buffer()`, `pico_layer_empty()`, `_pico_layer_image()`
   - Updated `_pico_output_draw_layer()` to use `SDL_RenderCopyEx` with
     layer's rot/flip settings (pico.c:1470-1475)
   - Updated `pico_get_view` and `pico_set_view` declarations in pico.h
   - Updated Lua bindings to pass NULL for rot/flip (lua/pico.c:667, 890)
   - Deleted deprecated `tst/todo_rotate.c` (uses removed APIs, can't
     rotate primitives with SDL_RenderCopyEx)
   - Created `tst/rot-flip.c` testing rotation (0°, 45°, 90°, 180°),
     anchor points (C, NW), flips (H, V, both), and combined rot+flip
   - Updated valgrind.supp line number (src:pico.c:524)
   - Files: pico.c, pico.h, lua/pico.c, valgrind.supp, tst/rot-flip.c

9. **Fix todo_* files**
   - Files: tst/todo_*.c

10. **Reorder pico_get/set_view parameters** [DONE]
    - Moved `tile` from position 6 to position 3
    - Old: grid, dim, target, source, clip, tile, rot, flip
    - New: grid, dim, tile, target, source, clip, rot, flip
    - Updated declarations (pico.h:341-350, 402-411)
    - Updated definitions (pico.c:757-766, 902-911)
    - Updated Lua bindings (lua/pico.c:669, 950)
    - Updated 31 test file call sites
    - Files: pico.h, pico.c, lua/pico.c, tst/*.c

---

## 9. Testing

```bash
make tests
./pico-sdl tst/navigate.c
```

---

## 10. Key Files

| File | Lines | Purpose |
|------|-------|---------|
| `src/pico.c` | 39-46 | `Pico_View` struct |
| `src/pico.c` | 71-87 | Global state `S` |
| `src/pico.c` | 525-528 | Main layer init |
| `src/pico.c` | 671-676 | Mouse transform |
| `src/pico.c` | 837-908 | `pico_set_view()` |
| `src/pico.c` | 910-954 | `pico_set_window()` |
| `src/pico.c` | 1188-1281 | Navigation handlers |
| `src/pico.c` | 1695-1699 | Present render |
| `src/pico.h` | | API declarations |
