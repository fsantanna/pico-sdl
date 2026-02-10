# video-lua.md — Lua bindings for video API

## Goal

Add Lua bindings for the four public video functions, following existing
patterns in `lua/pico.c`.

## Lua API mapping

| C function              | Lua call                          | returns          |
|-------------------------|-----------------------------------|------------------|
| `pico_layer_video`      | `pico.layer.video(name, path)`    | name (string)    |
| `pico_output_draw_video`| `pico.output.draw.video(path, rect)` | ok (boolean)  |
| `pico_get_video`        | `pico.get.video(path, [rect])`    | info (table)     |
| `pico_video_sync`       | `pico.video.sync(name, frame)`    | ok (boolean)     |

### pico.video subtable

`pico_video_sync` doesn't fit `get`/`set`/`input`/`output`/`layer`. Create a
new `pico.video` subtable with `sync`. This mirrors the C namespace
`pico_video_*` and is extensible.

## Binding functions

### 1. l_layer_video — follows l_layer_image pattern

```c
static int l_layer_video (lua_State* L) {
    const char* name = luaL_checkstring(L, 1);
    const char* path = luaL_checkstring(L, 2);
    const char* ret = pico_layer_video(name, path);
    lua_pushstring(L, ret);
    return 1;
}
```

Add `{ "video", l_layer_video }` to `ll_layer`.

### 2. l_output_draw_video — follows l_output_draw_image pattern

```c
static int l_output_draw_video (lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    luaL_checktype(L, 2, LUA_TTABLE);
    L_dim_default_wh(L, 2);
    Pico_Rel_Rect* rect = c_rel_rect(L, 2);
    int ok = pico_output_draw_video(path, rect);
    lua_pushboolean(L, ok);
    return 1;
}
```

Add `{ "video", l_output_draw_video }` to `ll_output_draw`.

### 3. l_get_video — follows l_get_window pattern (returns table)

```c
static int l_get_video (lua_State* L) {
    const char* path = luaL_checkstring(L, 1);

    Pico_Rel_Rect* rect = NULL;
    if (lua_gettop(L) >= 2 && lua_istable(L, 2)) {
        L_dim_default_wh(L, 2);
        rect = c_rel_rect(L, 2);
    }

    Pico_Video vid = pico_get_video(path, rect);

    lua_newtable(L);                        // T

    lua_newtable(L);                        // T | dim
    lua_pushinteger(L, vid.dim.w);
    lua_setfield(L, -2, "w");
    lua_pushinteger(L, vid.dim.h);
    lua_setfield(L, -2, "h");
    lua_setfield(L, -2, "dim");             // T

    lua_pushinteger(L, vid.fps);
    lua_setfield(L, -2, "fps");
    lua_pushinteger(L, vid.frame);
    lua_setfield(L, -2, "frame");
    lua_pushboolean(L, vid.done);
    lua_setfield(L, -2, "done");

    return 1;
}
```

Add `{ "video", l_get_video }` to `ll_get`.

### 4. l_video_sync — new subtable

```c
static int l_video_sync (lua_State* L) {
    const char* name = luaL_checkstring(L, 1);
    int frame = luaL_checkinteger(L, 2);
    int ok = pico_video_sync(name, frame);
    lua_pushboolean(L, ok);
    return 1;
}
```

New registration table:

```c
static const luaL_Reg ll_video[] = {
    { "sync", l_video_sync },
    { NULL, NULL }
};
```

In `luaopen_pico_native`, after `ll_layer` block:

```c
luaL_newlib(L, ll_video);               // pico | video
lua_setfield(L, -2, "video");           // pico
```

## Usage examples

```lua
-- Manual control
pico.layer.video("vid", "video.y4m")
local info = pico.get.video("video.y4m")
print(info.dim.w, info.dim.h, info.fps)
pico.video.sync("vid", 0)
pico.output.draw.layer("vid", {'%', x=.5, y=.5, w=1, h=1, anchor='C'})

-- Auto-play (handles timing + drawing)
local ok = pico.output.draw.video("video.y4m",
    {'%', x=.5, y=.5, w=1, h=1, anchor='C'})
```

## Changes

| file        | location          | change                              |
|-------------|-------------------|-------------------------------------|
| `lua/pico.c`| after l_layer_text | add `l_layer_video`                |
| `lua/pico.c`| `ll_layer`        | add `{ "video", l_layer_video }`   |
| `lua/pico.c`| after l_output_draw_text | add `l_output_draw_video`    |
| `lua/pico.c`| `ll_output_draw`  | add `{ "video", l_output_draw_video }` |
| `lua/pico.c`| after l_get_window | add `l_get_video`                 |
| `lua/pico.c`| `ll_get`          | add `{ "video", l_get_video }`     |
| `lua/pico.c`| new section       | add `l_video_sync` + `ll_video`    |
| `lua/pico.c`| `luaopen_pico_native` | add `ll_video` → `pico.video`  |

## Order

Apply after video-layer.md.
