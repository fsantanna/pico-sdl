# Plan: Layer-Aware Mouse Coordinates

## Context

`pico.get.mouse` returns coordinates in the main layer's space.
When a layer is drawn distorted (different size/position) on
the main layer, there's no way to get mouse coordinates in
that layer's own coordinate system.

**Goal**: extend `pico.get.mouse` with an optional rect
parameter so mouse coordinates are inversely transformed
through the rect chain — reusing the existing `up` mechanism.

## API Design

**C** — new function alongside existing one:

```c
// src/pico.h
Pico_Mouse pico_get_mouse_rect (char mode, Pico_Rel_Rect* rect);
```

Existing `pico_get_mouse(mode)` delegates to
`pico_get_mouse_rect(mode, NULL)`.

**Lua** — two-arg form:

```lua
-- without rect (unchanged):
local m = pico.get.mouse('%')

-- with rect:
local r = {'%', x=0.7, y=0.3, w=0.6, h=0.4, anc='C'}
pico.output.draw.layer('A', r)
local m = pico.get.mouse('%', r)

-- nested (up chain):
local outer = {'%', x=0.1, y=0.1, w=0.8, h=0.8}
local inner = {'%', x=0.2, y=0.2, w=0.5, h=0.5, up=outer}
local m = pico.get.mouse('%', inner)
```

## How It Works

The inverse transform already exists in `_cv_pos_flt_rel`
(`src/pico.c:373`). It:

1. Resolves `to->up` chain via `_f1()` to absolute coords
2. Inverts: `to->x = (mouse.x - rect.x) / rect.w` (for %)

So `pico_get_mouse_rect` just sets `rel.up = rect` before
calling `_cv_pos_flt_rel`. No new math needed.

## Implementation Steps

### 1. `src/pico.h` — declare new function

Add after `pico_get_mouse` declaration:

```c
Pico_Mouse pico_get_mouse_rect (char mode, Pico_Rel_Rect* rect);
```

### 2. `src/pico.c` — implement + refactor

Add `pico_get_mouse_rect` (same body as current
`pico_get_mouse` but with `rel.up = rect`):

```c
Pico_Mouse pico_get_mouse_rect (char mode, Pico_Rel_Rect* rect) {
    _pico_guard();
    if (mode == 0) { mode = S.mouse; }
    SDL_Point phy;
    Uint32 masks = SDL_GetMouseState(&phy.x, &phy.y);
    Pico_Mouse m = {
        .mode   = mode,
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

Refactor `pico_get_mouse` to delegate:

```c
Pico_Mouse pico_get_mouse (char mode) {
    return pico_get_mouse_rect(mode, NULL);
}
```

### 3. `lua/pico.c` — accept optional rect arg

Modify `l_get_mouse` (~line 849) to parse optional 2nd arg:

```c
static int l_get_mouse (lua_State* L) {
    char mode = 0;
    Pico_Rel_Rect* rect = NULL;
    if (lua_gettop(L) >= 1) {
        const char* s = luaL_checkstring(L, 1);
        mode = s[0];
    }
    if (lua_gettop(L) >= 2 && lua_type(L, 2) == LUA_TTABLE) {
        rect = c_rel_rect(L, 2);
    }
    Pico_Mouse m = pico_get_mouse_rect(mode, rect);
    lua_newtable(L);
    L_set_mouse(L, lua_gettop(L), &m);
    return 1;
}
```

`c_rel_rect` handles the `up` chain recursively via userdata
on the Lua stack — safe for the duration of this call.

### 4. Update `valgrind.supp`

Check if `SDL_Init` line moved; update `src:pico.c:N`.

### 5. Update test `lua/tst/todo/mouse-layer.lua`

Use the new API to print layer-local mouse coordinates.

## Key Files

- `src/pico.h` — function declaration
- `src/pico.c:683` — `pico_get_mouse` (refactor to delegate)
- `src/pico.c:373` — `_cv_pos_flt_rel` (reused, not modified)
- `lua/pico.c:849` — `l_get_mouse` (add rect param)
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

# 3. Identity — mouse('%', nil) == mouse('%'):
# Verify in Lua that omitting rect gives same result
```

## Progress

- [x] Created test `lua/tst/todo/mouse-layer.lua`
- [x] Created `tst/todo/segfault.c`
- [x] Added `_pico_guard()` to all 63 public API functions
- [x] Updated `valgrind.supp` (520 → 527)
- [ ] Add `pico_get_mouse_rect` to `src/pico.h`
- [ ] Implement `pico_get_mouse_rect` in `src/pico.c`
- [ ] Refactor `pico_get_mouse` to delegate
- [ ] Modify `l_get_mouse` in `lua/pico.c`
- [ ] Update test
- [ ] Run `make tests`
