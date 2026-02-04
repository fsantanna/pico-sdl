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

**Move to `Pico_View` (pico.c:39-46):**
```c
typedef struct {
    Pico_Abs_Dim  dim;    // logical world dimensions
    Pico_Abs_Rect dst;    // destination in window
    int           grid;   // grid overlay flag
    Pico_Abs_Rect src;    // source/zoom viewport
    Pico_Abs_Rect clip;   // clipping region
    Pico_Abs_Dim  tile;   // tile size
    // NEW:
    int           alpha;  // per-view alpha
    int           angle;  // per-view rotation
    int           flip;   // per-view flip (H/V/HV)
    // crop removed - use src instead
} Pico_View;
```

**Rationale:**
- Each layer should have its own rendering attributes
- Allows different layers to have different transformations
- `crop` functionality is already covered by `view.src`

---

## 2. Independent Grid per Layer

Currently grid is only rendered for the main layer (pico.c `_pico_output_present()`).
The `view.grid` flag exists in `Pico_View` but is ignored for non-main layers.

**Action:**
- Render grid overlay for each layer that has `view.grid` enabled
- Grid should show the layer's own coordinate system (based on layer's `view.src`)
- Allow different layers to have grid on/off independently

---

## 3. Remove `_crop()` - Use `view.src` Instead

Current `_crop()` (pico.c:318-325):
```c
static SDL_Rect* _crop (void) {
    if (S.crop.w==0 || S.crop.h==0) {
        return NULL;  // no crop
    } else {
        return &S.crop;
    }
}
```

Used in `pico_output_draw_layer()` (pico.c:1294):
```c
SDL_RenderCopy(G.ren, layer->tex, _crop(), &ri);
```

**Action:**
- Remove `S.crop` from global state
- Remove `pico_set_crop()` and `pico_get_crop()` API
- Use `layer->view.src` as the source rect for layer drawing
- The `src` rect already defines "which part of texture to show"

---

## 4. Clarify `%` Mode Semantics

### Problem
Currently `%` mode means "relative to current value" which is confusing:
- Navigation uses `{'%', {0.5, 0.5, 0.9, 0.9}}` to zoom (relative to current src)
- But `%` should mean "percentage of parent/container"

### New Semantics

**For win/log dimensions - REJECT `%` mode:**
- Window dimensions: physical pixels only (`'!'` mode)
- World dimensions: pixels (`'!'`) or tiles (`'#'`) only
- Rationale: nothing to be relative to at the top level

**For dst/src/clip - `%` relative to win/log:**
- `dst` percentage: relative to window dimensions (`S.win.dim`)
- `src` percentage: relative to world dimensions (`view.dim`)
- `clip` percentage: relative to world dimensions (`view.dim`)
- NOT relative to itself (current value)

**File locations:**
- `pico_set_view()`: pico.c:734-804
- `_sdl_rect()`: pico.c:208-276 (coordinate conversion)

---

## 5. Store src/dst/clip as Pico_Rel_Rect in Pico_View

**Current:**
```c
typedef struct {
    Pico_Abs_Dim  dim;
    Pico_Abs_Rect dst;    // always absolute
    Pico_Abs_Rect src;    // always absolute
    Pico_Abs_Rect clip;   // always absolute
    ...
} Pico_View;
```

**Proposed:**
```c
typedef struct {
    Pico_Abs_Dim  dim;
    Pico_Rel_Rect dst;    // stores mode + values
    Pico_Rel_Rect src;    // stores mode + values
    Pico_Rel_Rect clip;   // stores mode + values
    ...
} Pico_View;
```

**Default:** `{'%', {0, 0, 1, 1}, PICO_ANCHOR_NW, NULL}` (full world/window)

**Rationale:**
- Preserves how user specified values (%, !, #)
- Enables clean read-modify-write in same mode
- Navigation becomes trivial

---

## 6. Fix Navigation Aids

With `Pico_View` storing `Pico_Rel_Rect`, navigation becomes simple:

```c
case SDLK_LEFT: {
    // Scroll left by 10% of world
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

Or with ANCHOR_C for zoom:
```c
case SDLK_MINUS: {
    Pico_Rel_Rect src = S.layer->view.src;
    src.anchor = PICO_ANCHOR_C;
    src.x = src.x + src.w/2;  // convert to center
    src.y = src.y + src.h/2;
    src.w += 0.1;
    src.h += 0.1;
    pico_set_view(-1, NULL, NULL, &src, NULL, NULL);
    break;
}
```

**Navigation operations:**
- Zoom in/out: pico.c:1095-1120
- Scroll left/right/up/down: pico.c:1121-1167

---

## 7. API Changes

### Remove
- `pico_set_crop(Pico_Abs_Rect)` - pico.h, pico.c:681-683
- `pico_get_crop(Pico_Abs_Rect*)` - pico.h, pico.c:547-549
- `_crop()` helper - pico.c:318-325

### Add
- `pico_set_alpha()` - keep but per-view option in future
- `pico_set_rotate()` - implement actual rotation rendering
- `pico_set_flip()` - add flip state (SDL_FLIP_HORIZONTAL, SDL_FLIP_VERTICAL)

### Modify
- `pico_set_view()` - reject `%` for dim parameter
- `pico_output_draw_layer()` - use `layer->view.src` instead of `_crop()`

---

## 8. Fix `tst/todo_*` Files

| File | Main Issues |
|------|-------------|
| `todo_rain.c` | Uses deprecated `Pico_Rect_Pct`, `pico_output_draw_rect_pct()` |
| `todo_rotate.c` | Uses `pico_set_view_raw()`, old types, rotation not implemented |
| `todo_hide.c` | Missing window/view setup |
| `todo_scale.c` | Uses `pico_set_view_raw()`, `pico_set_scale()`, `pico_set_anchor_pos()` |
| `todo_control.c` | Wrong text drawing API signature |
| `todo_main.c` | Uses deprecated types, wrong API calls throughout |

**Common fixes needed:**
1. Replace `Pico_Rect_Pct` → `Pico_Rel_Rect` with mode `'%'`
2. Replace `Pico_Pos_Pct` → `Pico_Rel_Pos` with mode `'%'`
3. Replace `pico_set_view_raw()` → `pico_set_view()`
4. Add proper `pico_set_window()` + `pico_set_view()` init
5. Update drawing function calls to use `Pico_Rel_*` types

---

## 9. Implementation Order

1. **Remove `_crop()` and use `view.src`**
   - Simplest change, establishes pattern
   - Files: pico.c, pico.h

2. **Reject `%` mode for win/log** [DONE]
   - Add validation in `pico_set_window()` and `pico_set_view()` dim param
   - Files: pico.c

2b. **Fix `size_pct` tests** [DONE]
   - Changed to read-compute-set pattern with multipliers
   - Files: tst/size_pct.c, lua/tst/size_pct.lua

3. **Fix `%` for src to be relative to world** [DONE]
   - Changed `pico_set_view()` src '%' from `&view.src` to `NULL`
   - Files: pico.c:760

4. **Store src/dst/clip as Pico_Rel_Rect**
   - Change Pico_View to use Pico_Rel_Rect instead of Pico_Abs_Rect
   - Default: `{'%', {0, 0, 1, 1}, PICO_ANCHOR_NW, NULL}`
   - Files: pico.c, pico.h

5. **Fix navigation aids**
   - With Pico_Rel_Rect in view, navigation is simple read-modify-write
   - Use '%' mode, just add/subtract 0.1 for scroll/zoom
   - Files: pico.c:1095-1167

6. **Independent grid per layer**
   - Render grid for each layer with `view.grid` enabled
   - Files: pico.c (`_pico_output_present()`)

7. **Move alpha/rotate/flip to view** (future)
   - Larger change, can be done incrementally
   - Start with implementing rotation rendering

8. **Fix todo_* files**
   - Update to use modern API
   - Files: tst/todo_*.c

---

## 10. Testing

After each change:
```bash
# Run existing tests
make tests

# Manual test navigation
./pico-sdl tst/navigate.c
# Use Ctrl+arrows, Ctrl+minus/equals
```

---

## 11. Key Files

| File | Lines | Purpose |
|------|-------|---------|
| `src/pico.c` | 39-46 | `Pico_View` struct |
| `src/pico.c` | 71-87 | Global state `S` |
| `src/pico.c` | 318-325 | `_crop()` helper |
| `src/pico.c` | 681-683 | `pico_set_crop()` |
| `src/pico.c` | 734-804 | `pico_set_view()` |
| `src/pico.c` | 1093-1145 | Navigation handlers |
| `src/pico.c` | 1280-1309 | Layer drawing |
| `src/pico.h` | API declarations |
| `tst/todo_*.c` | Tests to fix |
