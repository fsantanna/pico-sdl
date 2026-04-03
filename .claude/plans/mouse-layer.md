# Plan: Layer-Aware Mouse Coordinates

## Context

`pico.get.mouse` returns coordinates in the main layer's space.
When a layer is drawn distorted (different size/position) on
the main layer, there's no way to get mouse coordinates in
that layer's own coordinate system.

**Goal**: add `pico_get_mouse_rect` so mouse coordinates are
inversely transformed through a rect's `up` chain — reusing
the existing inverse transform in `_cv_pos_flt_rel`.

## API Design

**C** — two separate functions:

```c
// existing, unchanged:
Pico_Mouse pico_get_mouse (char mode);

// new — mode comes from rect->mode, rect required:
Pico_Mouse pico_get_mouse_rect (Pico_Rel_Rect* rect);
```

**Lua** — overloaded `pico.get.mouse`:

```lua
-- without rect (unchanged):
local m = pico.get.mouse('%')

-- with rect (mode from rect):
local r = {'%', x=0.7, y=0.3, w=0.6, h=0.4, anc='C'}
pico.output.draw.layer('A', r)
local m = pico.get.mouse(r)

-- nested (up chain):
local outer = {'%', x=0.1, y=0.1, w=0.8, h=0.8}
local inner = {'%', x=0.2, y=0.2, w=0.5, h=0.5, up=outer}
local m = pico.get.mouse(inner)
```

## How It Works

The inverse transform already exists in `_cv_pos_flt_rel`
(`src/pico.c:373`). It:

1. Resolves `to->up` chain via `_f1()` to absolute coords
2. Inverts: `to->x = (mouse.x - rect.x) / rect.w` (for %)

So `pico_get_mouse_rect` sets `rel.up = rect` before calling
`_cv_pos_flt_rel`. No new math needed.

## Implementation Steps

### 1. `src/pico.h` — declare new function

Add after `pico_get_mouse` declaration:

```c
Pico_Mouse pico_get_mouse_rect (Pico_Rel_Rect* rect);
```

### 2. `src/pico.c` — implement

Add `pico_get_mouse_rect` (similar to `pico_get_mouse` but
mode from `rect->mode`, `rel.up = rect`):

```c
Pico_Mouse pico_get_mouse_rect (Pico_Rel_Rect* rect) {
    _pico_guard();
    assert(rect != NULL);
    char mode = rect->mode;
    SDL_Point phy;
    Uint32 masks = SDL_GetMouseState(&phy.x, &phy.y);
    Pico_Mouse m = {
        .left   = !!(masks & SDL_BUTTON(SDL_BUTTON_LEFT)),
        .right  = !!(masks & SDL_BUTTON(SDL_BUTTON_RIGHT)),
        .middle = !!(masks & SDL_BUTTON(SDL_BUTTON_MIDDLE)),
    };
    if (mode == 'w') {
        m.x = phy.x;
        m.y = phy.y;
    } else {
        SDL_FPoint log = _cv_phy_log(phy);
        Pico_Rel_Pos rel = {
            .mode = mode, .anchor = PICO_ANCHOR_NW, .up = rect
        };
        _cv_pos_flt_rel(log, &rel, NULL);
        m.x = rel.x;
        m.y = rel.y;
    }
    return m;
}
```

`pico_get_mouse(mode)` stays unchanged.

### 3. `lua/pico.c` — duck-type the argument

Modify `l_get_mouse` (~line 849):
- String arg → `pico_get_mouse(mode)` (existing behavior)
- Table arg → `c_rel_rect` + `pico_get_mouse_rect(rect)`

```c
static int l_get_mouse (lua_State* L) {
    Pico_Mouse m;
    if (lua_gettop(L) >= 1 && lua_type(L, 1) == LUA_TTABLE) {
        Pico_Rel_Rect* rect = c_rel_rect(L, 1);
        m = pico_get_mouse_rect(rect);
    } else {
        char mode = 0;
        if (lua_gettop(L) >= 1) {
            const char* s = luaL_checkstring(L, 1);
            mode = s[0];
        }
        m = pico_get_mouse(mode);
    }
    lua_newtable(L);
    L_set_mouse(L, lua_gettop(L), &m);
    return 1;
}
```

### 4. Update `valgrind.supp`

Check if `SDL_Init` line moved; update `src:pico.c:N`.

### 5. Update test `lua/tst/todo/mouse-layer.lua`

Use the new API to print layer-local mouse coordinates.

### 6. Run `make tests`

Verify no regressions.

---

### 7. (Breaking) Remove `pico_set_mouse`

Remove `pico_set_mouse(mode)` and `S.mouse` global state.
`pico_get_mouse(mode)` always requires an explicit mode arg
(mode=0 is now an assert error).

In Lua: remove `pico.set.mouse`.

#### Changes

| File | Place | Description |
|---|---|---|
| `src/pico.h:519-521` | `pico_set_mouse` | delete declaration + doc |
| `src/pico.c:88` | `S.mouse` | remove field from `S` struct |
| `src/pico.c:105` | `Pico_State.mouse` | remove field from stack state |
| `src/pico.c:578` | `S` init | remove `.mouse = '!'` |
| `src/pico.c:702-706` | `pico_get_mouse` | replace `S.mouse` fallback with assert |
| `src/pico.c:915-919` | `pico_set_mouse` | delete function |
| `src/pico.c:937` | `pico_push` | remove `.mouse = S.mouse` |
| `src/pico.c:951` | `pico_pop` | remove `S.mouse = st->mouse` |
| `lua/pico.c:945-949` | `l_set_mouse` | delete function |
| `lua/pico.c:1562` | `ll_set` | remove `{ "mouse", l_set_mouse }` |
| `tst/mouse.c:157` | test | remove `pico_set_mouse('%')` |
| `tst/mouse.c:169,178` | test | `pico_get_mouse(0, NULL)` → `pico_get_mouse('%', NULL)` |
| `tst/tiles.c:9` | test | remove `pico_set_mouse('#')` |
| `tst/tiles.c:66,76,85` | test | `pico_get_mouse(0, NULL)` → `pico_get_mouse('#', NULL)` |

### 8. (Breaking) Mouse events carry window-only positions

Mouse events (`mouse.button.dn`, `mouse.button.up`,
`mouse.motion`) carry only window-based (physical pixel)
positions. Remove `mode` field from `Pico_Mouse` struct.
Users call `pico_get_mouse` / `pico_get_mouse_rect` to get
positions in the desired coordinate space.

#### Changes

| File | Place | Description |
|---|---|---|
| `src/events.h:30` | `Pico_Mouse` | remove `mode` field |
| `src/pico.c:712` | `pico_get_mouse` | remove `.mode = mode` from initializer |
| `src/pico.c:1321` | `sdl_to_pico` | `pico_get_mouse(0, NULL)` → `pico_get_mouse('w', NULL)` |
| `lua/pico.c:842-844` | `L_set_mouse` | remove mode push (index 1) |
| `lua/pico.c:876-889` | `l_get_mouse` | push mode string into result table directly |

## Key Files

- `src/pico.h` — function declaration
- `src/pico.c:683` — `pico_get_mouse` (unchanged)
- `src/pico.c:373` — `_cv_pos_flt_rel` (reused, not modified)
- `lua/pico.c:849` — `l_get_mouse` (duck-type arg)
- `lua/tst/todo/mouse-layer.lua` — test

## Verification

```bash
# 1. Regression — existing tests still pass:
make tests

# 2. Manual — run mouse-layer test:
pico-lua lua/tst/todo/mouse-layer.lua
# Move mouse over the distorted layer
# Coordinates should map to layer's local space (0-1 inside)
# Outside the layer: extrapolated values (>1 or <0)

# 3. Identity — mouse('%') unchanged behavior
```

## Progress

- [x] Created test `lua/tst/todo/mouse-layer.lua`
- [x] Created `tst/todo/segfault.c`
- [x] Added `_pico_guard()` to all 63 public API functions
- [x] Updated `valgrind.supp` (520 → 527)
- [ ] Add `pico_get_mouse_rect` to `src/pico.h`
- [ ] Implement `pico_get_mouse_rect` in `src/pico.c`
- [ ] Modify `l_get_mouse` in `lua/pico.c` (duck-type)
- [ ] Update test
- [ ] Run `make tests`
- [ ] (Breaking) Remove `pico_set_mouse`
- [ ] (Breaking) Mouse events carry window-only positions
