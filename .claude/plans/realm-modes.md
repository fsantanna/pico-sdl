# Plan: Realm Modes — Hide with Sane Defaults

## Context

The realm allocator manages resources with three modes:
- `'!'` exclusive — asserts if key exists
- `'='` shared — returns existing if found, creates if new
- `'~'` replace — frees old, allocates new

All `pico_layer_*` C functions currently require `mode` as first parameter.
This is low-level detail most users shouldn't need.
Goal: hide realm modes behind sane defaults, expose `_mode` variants for
power users.

## Decisions

| Question                    | Answer                               |
|-----------------------------|--------------------------------------|
| `pico_layer_*` default      | `'!'` exclusive                      |
| `_mode` variants scope      | only `pico_layer_*` (6 functions)    |
| `_mode` naming              | `pico_layer_image_mode`              |
| `draw_text` / `get_text`    | add explicit key param, use `'~'`    |
| `draw_image` / `get_image`  | no API change (keep `'='` internal)  |
| Lua layer mode detection    | check for `'!'`/`'='`/`'~'` chars   |
| Lua text key                | explicit key param (like C)          |
| `layer.images` mode         | optional passthrough (parent only)   |
| `layer.images` subs         | always `'!'`, use prefix to avoid    |
|                             | collisions                           |

## Changes by File

### 1. `src/pico.h`

**pico_layer_* — split into default + _mode (6 functions)**

```c
// Default (mode '!'):
void pico_layer_buffer (const char* name,
    Pico_Abs_Dim dim, const Pico_Color_A* pixels);
void pico_layer_empty (const char* name, Pico_Abs_Dim dim);
void pico_layer_image (const char* name, const char* path);
void pico_layer_sub (const char* name,
    const char* parent, const Pico_Rel_Rect* crop);
void pico_layer_text (const char* name,
    int height, const char* text);
void pico_layer_video (const char* name, const char* path);

// Explicit mode:
void pico_layer_buffer_mode (int mode, const char* name,
    Pico_Abs_Dim dim, const Pico_Color_A* pixels);
void pico_layer_empty_mode (int mode,
    const char* name, Pico_Abs_Dim dim);
void pico_layer_image_mode (int mode,
    const char* name, const char* path);
void pico_layer_sub_mode (int mode, const char* name,
    const char* parent, const Pico_Rel_Rect* crop);
void pico_layer_text_mode (int mode, const char* name,
    int height, const char* text);
void pico_layer_video_mode (int mode,
    const char* name, const char* path);
```

**pico_output_draw_text — add key param**

```c
// Before:
void pico_output_draw_text (const char* text,
    Pico_Rel_Rect* rect);
// After:
void pico_output_draw_text (const char* key,
    const char* text, Pico_Rel_Rect* rect);
```

**pico_get_text — add key param**

```c
// Before:
Pico_Abs_Dim pico_get_text (const char* text,
    Pico_Rel_Dim* rel);
// After:
Pico_Abs_Dim pico_get_text (const char* key,
    const char* text, Pico_Rel_Dim* rel);
```

### 2. `src/pico.c`

**pico_layer_empty** (~line 990):
- Rename current → `pico_layer_empty_mode`
- New `pico_layer_empty` calls `_mode` with `'!'`
- Same for `pico_layer_sub` (~line 1010)

**pico_output_draw_text** (~line 1395):
- Add `key` param
- Change `_pico_layer_text('=', NULL, ...)` →
  `_pico_layer_text('~', key, ...)`

**pico_get_text** (~line 723):
- Add `key` param
- Replace temp texture approach with
  `_pico_layer_text('~', key, ...)` for caching
- Use cached layer's view dimensions

**Grid label calls** (~lines 1483-1496):
- Internal `pico_output_draw_text` calls need a key
- Use fixed keys like `"/grid/x"`, `"/grid/y"`

### 3. `src/layers.hc`

- Rename `_pico_layer_buffer` caller's wrapper →
  `pico_layer_buffer_mode`
- New `pico_layer_buffer` calls `_mode` with `'!'`
- Same for `_pico_layer_image` → `pico_layer_image_mode`,
  `_pico_layer_text` → `pico_layer_text_mode`
- Internal `_pico_layer_*` functions unchanged

### 4. `src/video.hc`

- Rename `pico_layer_video` → `pico_layer_video_mode`
- New `pico_layer_video` calls `_mode` with `'!'`

### 5. `lua/pico.c`

**Layer functions (6 bindings, ~lines 1028-1119)**:

Detect optional mode: if arg 1 is string of length 1 and one of
`'!'`, `'='`, `'~'`, treat as mode; otherwise default `'!'`.

```c
static int l_layer_empty (lua_State* L) {
    int mode = '!';
    int base = 1;
    if (lua_isstring(L, 1)) {
        const char* ms = lua_tostring(L, 1);
        if (strlen(ms)==1 && (ms[0]=='!'
                || ms[0]=='=' || ms[0]=='~')) {
            mode = ms[0];
            base = 2;
        }
    }
    const char* name = luaL_checkstring(L, base);
    luaL_checktype(L, base+1, LUA_TTABLE);
    // ... call pico_layer_empty_mode(mode, name, dim)
}
```

**l_output_draw_text** (~line 1349):
- Add key as first string arg:
  `pico.output.draw.text(key, text, rect)`

**l_get_text** (~line 636):
- Add key as first string arg:
  `pico.get.text(key, text, dim)`

### 6. `lua/pico/init.lua` — layer.images

- Optional mode as first arg (detect 1-char `'!'`/`'='`/`'~'`)
- Optional prefix for sub names (extra arg or field in `t`)
- Mode applies to `layer.image` call (parent)
- Subs always use `'!'` (exclusive)

```lua
function M.layer.images ([mode,] name, path, t)
    -- mode defaults to '!' if not provided
    -- t.name = optional sub prefix (defaults to name)
    M.layer.image(mode, name, path)
    -- subs:
    local pre = t.name or name
    local sub = pre .. "-" .. i
    M.layer.sub(sub, name, crop)  -- always '!' default
end
```

### 7. C tests — update call sites

**Remove mode from pico_layer_* calls:**

| File               | Call                           | Change          |
|--------------------|--------------------------------|-----------------|
| `tst/layers.c:22`  | `pico_layer_empty('!', ...)`  | remove `'!'`    |
| `tst/layers.c:29`  | `pico_layer_empty('!', ...)`  | remove `'!'`    |
| `tst/layers.c:77`  | `pico_layer_empty('=', ...)`  | use `_mode`     |
| `tst/layers.c:83`  | `pico_layer_empty('=', ...)`  | use `_mode`     |
| `tst/layers.c:96`  | `pico_layer_buffer('=', ...)`  | use `_mode`    |
| `tst/layers.c:97`  | `pico_layer_buffer('=', ...)`  | use `_mode`    |
| `tst/layers.c:101` | `pico_layer_buffer('=', ...)` | use `_mode`     |
| `tst/layers.c:102` | `pico_layer_buffer('=', ...)` | use `_mode`     |
| `tst/sheet.c:9`    | `pico_layer_empty('!', ...)`  | remove `'!'`    |
| `tst/sheet.c:36-42`| `pico_layer_sub('!', ...)`    | remove `'!'`    |
| `tst/image_raw.c:79`| `pico_layer_image('!', ...)`  | remove `'!'`   |
| `tst/rot-flip.c:10`| `pico_layer_image('!', ...)`  | remove `'!'`    |
| `tst/video.c:23`   | `pico_layer_video('!', ...)`  | remove `'!'`    |
| `tst/todo_video.c:41`| `pico_layer_video('!', ...)` | remove `'!'`   |

**Add key to draw_text/get_text calls:**

| File                  | Function              | Key to use       |
|-----------------------|-----------------------|------------------|
| `tst/font.c:10`       | `pico_get_text`      | `"font"`         |
| `tst/font.c:16`       | `pico_get_text`      | `"font"`         |
| `tst/font.c:23`       | `pico_get_text`      | `"font"`         |
| `tst/font.c:30`       | `pico_output_draw_text`| `"font"`        |
| `tst/font.c:35`       | `pico_output_draw_text`| `"font_pct"`    |
| `tst/blend_raw.c:24`  | `pico_get_text`      | `"blend"`        |
| `tst/blend_raw.c:30`  | `pico_output_draw_text`| `"blend"`       |
| `tst/blend_pct.c:45`  | `pico_output_draw_text`| `"blend"`       |
| `tst/todo_control.c:7`| `pico_output_draw_text`| `"ctl"`         |
| `tst/todo_video.c:121`| `pico_output_draw_text`| `"label"`       |
| `tst/view_raw.c:73`   | `pico_output_draw_text`| `"view"`        |

### 8. Lua tests — update call sites

**Remove mode from layer calls:**

| File                    | Change                              |
|-------------------------|-------------------------------------|
| `lua/tst/image_raw.lua` | `pico.layer.image("crop", ...)`    |
| `lua/tst/layers.lua`    | `pico.layer.empty("bg", ...)`      |
| `lua/tst/rot-flip.lua`  | `pico.layer.image("img", ...)`     |
| `lua/tst/sheet.lua`     | `pico.layer.empty/sub` — drop `'!'`|
| `lua/tst/video.lua`     | `pico.layer.video("vid", ...)`     |

### 9. `lua/doc/gen-guide-images.lua`

- `pico.layer.empty` (lines 291, 297): already no mode — needs
  check after Lua binding changes
- `pico.layer.sub` (lines 329-333): already no mode — OK
- `pico.output.draw.text` (lines 89, 100, 114): add key param

### 10. Documentation

- `lua/doc/api.md`: update signatures
- `lua/doc/guide.md`: update examples

### 11. `valgrind.supp`

- Update `src:pico.c:N` if `SDL_Init` line moves

## Verification

1. Compile all C tests with updated API
2. `make tests` (headless via xvfb)
3. Run Lua tests: each `lua/tst/*.lua` file
4. Check `gen-guide-images.lua` generates correctly
5. Valgrind: verify suppression line is correct

## Status

- [x] Requirements clarified
- [ ] Implementation
- [ ] Testing
