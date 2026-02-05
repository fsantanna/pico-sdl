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

1. **Remove `_crop()` and use `view.src`**
   - Files: pico.c, pico.h

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

5. **Store src/dst/clip as Pico_Rel_Rect** ← NEXT
   - Change Pico_View struct
   - ~34 access points to update
   - Files: pico.c, pico.h

6. **Fix navigation to use '%' read-modify-write**
   - Depends on step 5
   - Simple `src.x -= 0.1` pattern
   - Files: pico.c:1188-1281

7. **Independent grid per layer**
   - Files: pico.c (`_pico_output_present()`)

8. **Move alpha/rotate/flip to view** (future)

9. **Fix todo_* files**
   - Files: tst/todo_*.c

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
