# Plan: Refactor c_color → c_color_s + c_color_st

## Goal

Remove the 3-argument `(r,g,b)` form from `c_color`. Split into:
- `c_color_s(L, i)` — string lookup (e.g., `'red'`)
- `c_color_st(L, i)` — string or table, dispatches to `c_color_s` or `c_color_t`

All functions take an explicit `int i` index parameter.

## File: `lua/pico.c`

### 1. Extract `c_color_s(L, i)` from the string branch of `c_color`

```c
static Pico_Color c_color_s (lua_State* L, int i) {
    assert(i > 0);
    assert(lua_type(L,i) == LUA_TSTRING);
    lua_pushlightuserdata(L, (void*)&KEY);
    lua_gettable(L, LUA_REGISTRYINDEX);
    lua_getfield(L, -1, "colors");
    lua_pushvalue(L, i);
    lua_gettable(L, -2);
    int ok = lua_islightuserdata(L, -1);
    if (!ok) {
        luaL_error(L, "invalid color \"%s\"",
            lua_tostring(L,i));
    }
    Pico_Color* clr = lua_touserdata(L, -1);
    lua_pop(L, 3);
    return *clr;
}
```

### 2. Rename `c_color` → `c_color_st(L, i)`, remove `(r,g,b)` branch

```c
static Pico_Color c_color_st (lua_State* L, int i) {
    assert(i > 0);
    if (lua_type(L,i) == LUA_TSTRING) {
        return c_color_s(L, i);
    } else {
        luaL_checktype(L, i, LUA_TTABLE);
        return c_color_t(L, i);
    }
}
```

### 3. Update 6 callers

| Caller | Old | New |
|--------|-----|-----|
| `l_color_darker` | `c_color(L)` | `c_color_st(L, 1)` |
| `l_color_lighter` | `c_color(L)` | `c_color_st(L, 1)` |
| `l_color_mix` (1st) | `c_color(L)` | `c_color_st(L, 1)` |
| `l_color_mix` (2nd) | `lua_remove(L,1); c_color(L)` | `c_color_st(L, 2)` |
| `l_set_color_clear` | `c_color(L)` | `c_color_st(L, 1)` |
| `l_set_color_draw` | `c_color(L)` | `c_color_st(L, 1)` |

Rewrite `l_color_mix` (remove `lua_remove` hack):

```c
static int l_color_mix (lua_State* L) {
    Pico_Color c1 = c_color_st(L, 1);
    Pico_Color c2 = c_color_st(L, 2);
    Pico_Color ret = pico_color_mix(c1, c2);
    L_push_color(L, ret);
    return 1;
}
```

## Lua test files — convert `(r,g,b)` to string/table

33 instances across 7 files. Use named colors where exact:

| `(r,g,b)` | Replacement |
|-----------|-------------|
| `255, 0, 0` | `'red'` |
| `255, 255, 255` | `'white'` |
| `200, 0, 0` | `{'!', r=200, g=0, b=0}` |
| `0, 200, 0` | `{'!', r=0, g=200, b=0}` |
| `0, 0, 200` | `{'!', r=0, g=0, b=200}` |

Files:
- `lua/tst/blend_pct.lua` — 7× `(255,0,0)` → `'red'`
- `lua/tst/blend_raw.lua` — 7× `(255,0,0)` → `'red'`
- `lua/tst/dim.lua` — 1× white, 1× red
- `lua/tst/collide_pct.lua` — 3× white, 3× red
- `lua/tst/collide_raw.lua` — 2× white, 2× red
- `lua/tst/view_raw.lua` — 1× white, 1× red
- `lua/tst/shot.lua` — 3× non-standard values → table form

## Verification

- `make tests` (or `xvfb-run make tests`) should pass unchanged
