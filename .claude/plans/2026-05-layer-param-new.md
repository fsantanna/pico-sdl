# Layer Param for All S.layer Readers

## Context

Today, the public draw API (`pico_output_draw_*`, `pico_output_clear`)
and many internal helpers in `aux.hc`, `mem.hc`, `layers.hc` read the
implicit `S.layer` global to fetch pencil color/style, scene dim/tile,
effect alpha/grid/color, and clip rect. This forces clients to call
`pico_set_layer` before every draw to retarget — and prevents drawing to
a non-current layer in a single call.

The state API (`pico_get_pencil_color`, `pico_set_scene_dim`, …) already
follows the convention `(const char* layer, …)` with `NULL = current`,
resolved through `_pico_layer_null(layer)`. Goal: extend this convention
to **every reader of `S.layer`** so layer access is explicit.

## Decisions (confirmed)

| Decision         | Choice                                              |
|------------------|-----------------------------------------------------|
| Public param     | `const char* layer` (matches existing pattern)      |
| Static param     | `Pico_Layer* L` — pre-resolved pointer              |
| Scope            | All readers; writers and assertion-only checks stay |
| Position         | First parameter                                     |
| Lua bindings     | Expose as optional first arg (`nil` → current)      |
| Public boundary  | `Pico_Layer* L = _pico_layer_null(layer);` once, then call statics with `L` |
| Static contract  | `assert(L != NULL);` at top; never call `_pico_layer_null` |
| Static→public callbacks | static helpers calling `pico_cv_*` pass `L->name` (root.name = `"root"`, set at `pico.c:255` and registered in realm at `pico.c:292`) |

## Functions Changed

### Public API — `src/pico.c` + `src/pico.h`

| function                      | new signature (layer first)                                              |
|-------------------------------|--------------------------------------------------------------------------|
| `pico_output_clear`           | `(const char* layer)`                                                    |
| `pico_output_draw_line`       | `(const char* layer, Pico_Rel_Pos* p1, Pico_Rel_Pos* p2)`                |
| `pico_output_draw_oval`       | `(const char* layer, Pico_Rel_Rect* rect)`                               |
| `pico_output_draw_pixel`      | `(const char* layer, Pico_Rel_Pos* pos)`                                 |
| `pico_output_draw_pixels`     | `(const char* layer, int n, const Pico_Rel_Pos* ps)`                     |
| `pico_output_draw_rect`       | `(const char* layer, Pico_Rel_Rect* rect)`                               |
| `pico_output_draw_poly`       | `(const char* layer, int n, const Pico_Rel_Pos* ps)`                     |
| `pico_output_draw_tri`        | `(const char* layer, Pico_Rel_Pos* p1, p2, p3)`                          |
| `pico_output_draw_text`       | `(const char* layer, const char* text, Pico_Rel_Rect* rect)`             |
| `pico_output_draw_text_mode`  | `(const char* layer, int mode, const char* key, const char* text, Pico_Rel_Rect* rect)` |
| `pico_output_draw_image`      | `(const char* layer, const char* path, Pico_Rel_Rect* rect)`             |
| `pico_output_draw_pixmap`     | `(const char* layer, const char* key, Pico_Abs_Dim dim, const Pico_Color pixmap[], const Pico_Rel_Rect* rect)` |
| `pico_output_draw_layer`      | `(const char* layer, const char* key, Pico_Rel_Rect* rect)` — `layer` = destination, `key` = source |
| `pico_layer_text*`            | `(const char* layer, … )` — needed by `_pico_layer_text` for auto-key when `key==NULL` |
| `pico_get_text` / `_mode`     | `(const char* layer, … )` — call `_sdl_dim` / `_sdl_pos` chain          |
| `pico_cv_*` (all 11)          | `(const char* layer, … )` — propagate to `_sdl_*` / `_rel_*` helpers    |
| `pico_vs_pos_rect`            | `(const char* layer, … )` — calls `_sdl_*`                              |
| `pico_vs_rect_rect`           | `(const char* layer, … )` — calls `_sdl_*`                              |
| `pico_get_mouse`              | `(const char* layer, char mode, Pico_Rel_Rect* rect)` — calls `pico_cv_pos_win_rel` |
| `pico_set_mouse`              | `(const char* layer, Pico_Rel_Pos* pos)` — calls `_sdl_pos` chain       |

Resolver pattern at top of each function:
```c
Pico_Layer* L = _pico_layer_null(layer);
```
…and replace every `S.layer` read in the body with `L`.

### Static helpers (param must propagate)

| file        | function                  | new first param      |
|-------------|---------------------------|----------------------|
| layers.hc   | `_pico_layer_text`        | `const char* layer`  |
| layers.hc   | `_pico_output_draw_layer` | `const char* layer` (rename existing `Pico_Layer* layer` → `src`) |
| mem.hc      | `_tex_text`               | `const char* layer`  |
| aux.hc      | `_dim_win_to_wld`         | `const char* layer`  |
| aux.hc      | `_dim_wld_to_win`         | `const char* layer`  |
| aux.hc      | `_pos_win_to_wld`         | `const char* layer`  |
| aux.hc      | `_pos_wld_to_win`         | `const char* layer`  |
| aux.hc      | `_sdl_dim`                | `const char* layer`  |
| aux.hc      | `_sdl_pos`                | `const char* layer`  |
| aux.hc      | `_sdl_rect`               | `const char* layer`  |
| aux.hc      | `_rel_dim`                | `const char* layer`  |
| aux.hc      | `_rel_pos`                | `const char* layer`  |
| aux.hc      | `_rel_rect`               | `const char* layer`  |
| pico.c      | `_show_grid`              | `const char* layer`  |

The chain `pico_output_draw_oval` → `_sdl_rect` → `_f1`/`_f3`: each
helper accepts `layer`, calls `_pico_layer_null(layer)` once, threads
the same `layer` to nested helpers.

### NOT changed (intentional)

| function                                    | why                              |
|---------------------------------------------|----------------------------------|
| `pico_get_layer()`                          | returns the *current* layer name |
| `pico_set_layer(key)`                       | writer of `S.layer`              |
| `_layer_traverse()` in layers.hc            | saves/restores `S.layer` for recursion |
| `pico_event_handler` / `_pico_output_present` / `pico_output_present` / `pico_set_dim` | only `assert(S.layer == &G.root)` — checks global state, not reading layer fields |

## Lua Bindings — `lua/pico.c`

Default is `NULL` (→ current `S.layer` at C resolve time), not a fixed
key. No individual setters are exposed in Lua — `pico_set_*_<field>` is
only invoked internally from the bulk-setter bindings (see
`lua/pico.c:1013-1119`), so only the **bulk** setter pattern needs the
table-idx-1 form.

Two helpers cover every binding shape (`*_text_mode` and `get.text_mode`
are **not** bound to Lua — only their non-`_mode` variants are — so no
mode-vs-layer disambiguation is needed):

```c
// table-form bulk setters: set.pencil/effect/scene { [1]=layer, ... }
static const char* LC_layer_idx_1 (lua_State* L, int i) {
    assert(i > 0);
    const char* layer = NULL;
    lua_geti(L, i, 1);                  // T | T[1]
    if (!lua_isnil(L, -1)) {
        layer = luaL_checkstring(L, -1);
    }
    lua_pop(L, 1);                      // T
    return layer;
}

// positional: layer is arg 1 iff arg 1 is a string AND arg 2 is NOT.
// Used for fns whose original arg 1 is a non-string (table / nothing).
static const char* LC_layer_arg_1 (lua_State* L) {
    const char* layer = NULL;
    if (lua_type(L,1) == LUA_TSTRING && lua_type(L,2) != LUA_TSTRING) {
        layer = lua_tostring(L, 1);
        lua_remove(L, 1);
    }
    return layer;
}

// positional: layer is arg 1 iff arg 1 AND arg 2 are BOTH strings.
// Used for fns whose original arg 1 is itself a string (path / key /
// text / mode), so the disambiguator is "two strings up front".
static const char* LC_layer_arg_1_2 (lua_State* L) {
    const char* layer = NULL;
    if (lua_type(L,1) == LUA_TSTRING && lua_type(L,2) == LUA_TSTRING) {
        layer = lua_tostring(L, 1);
        lua_remove(L, 1);
    }
    return layer;
}
```

### Helper assignment per binding

| binding (Lua name)         | helper             | C call                       |
|----------------------------|--------------------|------------------------------|
| `set.pencil`               | `LC_layer_idx_1`   | `pico_set_pencil(layer, ...)` |
| `set.effect`               | `LC_layer_idx_1`   | `pico_set_effect(layer, ...)` |
| `set.scene`                | `LC_layer_idx_1`   | `pico_set_scene(layer, ...)`  |
| `output.clear`             | `LC_layer_arg_1`   | (no other args)              |
| `output.draw_line`         | `LC_layer_arg_1`   | arg1 is `Pos` table          |
| `output.draw_oval`         | `LC_layer_arg_1`   | arg1 is `Rect` table         |
| `output.draw_pixel`        | `LC_layer_arg_1`   | arg1 is `Pos` table          |
| `output.draw_pixels`       | `LC_layer_arg_1`   | arg1 is array table          |
| `output.draw_rect`         | `LC_layer_arg_1`   | arg1 is `Rect` table         |
| `output.draw_poly`         | `LC_layer_arg_1`   | arg1 is array table          |
| `output.draw_tri`          | `LC_layer_arg_1`   | arg1 is `Pos` table          |
| `set.mouse`                | `LC_layer_arg_1`   | arg1 is `Pos` table          |
| `cv_*` (all 11)            | `LC_layer_arg_1`   | arg1 is `Dim`/`Pos`/`Rect` table |
| `vs_pos_rect`/`vs_rect_rect` | `LC_layer_arg_1` | arg1 is `Pos`/`Rect` table   |
| `get.pencil[_*]`           | `LC_layer_arg_1`   | (only-arg case)              |
| `get.effect[_*]`           | `LC_layer_arg_1`   | (only-arg case)              |
| `get.scene[_*]`            | `LC_layer_arg_1`   | (only-arg case)              |
| `output.draw_image`        | `LC_layer_arg_1_2` | arg1=path, arg2=rect (table) |
| `output.draw_text`         | `LC_layer_arg_1_2` | arg1=text, arg2=rect (table) |
| `output.draw_pixmap`       | `LC_layer_arg_1_2` | arg1=key, arg2=dim (table)   |
| `output.draw_layer`        | `LC_layer_arg_1_2` | arg1=key, arg2=rect (table)  |
| `get.text`                 | `LC_layer_arg_1_2` | arg1=text, arg2=dim (table)  |
| `get.mouse`                | `LC_layer_arg_1_2` | arg1=mode, arg2=rect (table) |

### `*_text_mode` and `get.text_mode`

`pico_output_draw_text_mode` and `pico_get_text_mode` exist in C but
are **not** exposed to Lua — only their non-mode variants
(`l_output_draw_text` at `lua/pico.c:1500`, `l_get_text` at 808). At
the C level both still gain `const char* layer` as the new first
param; at the Lua level there is nothing to bind, so no disambiguator
is needed.

`pico_layer_text` reads `S.layer` only on the auto-key path (when
`key == NULL`). The Lua binding `l_layer_text` (line 1235) requires
`key` via `luaL_checkstring`, so that path is unreachable from Lua —
the binding always passes `NULL` for the new layer arg.

Apps and `tst/*` written before this change keep working because the
leading layer arg is always optional and defaults to `NULL`.

Bindings to update (line numbers in current `lua/pico.c`):
`output_clear` 1387, `output_draw_pixmap` 1402, `output_draw_image`
1414, `output_draw_layer` 1427, `output_draw_line` 1438,
`output_draw_oval` 1445, `output_draw_pixel` 1452, `output_draw_pixels`
1470, `output_draw_poly` 1489, `output_draw_rect` 1496,
`output_draw_text` 1505, `output_draw_tri` 1518, plus the `pico_cv_*`,
`pico_vs_*`, `pico_get_mouse`, `pico_set_mouse`, `pico_get_text*`,
`pico_layer_text*`, plus the bulk `set.pencil/effect/scene`.

## Reused utilities (no new code)

- **`_pico_layer_null`** at `src/layers.hc:61` — already returns
  `S.layer` when the passed name is `NULL`. Use it everywhere as the
  resolution point. **No new helper is needed.**
- **`_pico_guard()`** stays at the top of each public function,
  immediately followed by the `_pico_layer_null(layer)` resolution.

## Critical Files

- `src/pico.h` — public signature changes (the API break)
- `src/pico.c` — public function bodies + `_show_grid` + event handler
- `src/layers.hc` — `_pico_layer_text`, `_pico_output_draw_layer`
- `src/aux.hc` — all `_sdl_*` / `_rel_*` / win-wld helpers
- `src/mem.hc` — `_tex_text`
- `lua/pico.c` — Lua bindings

## Verification

1. **Build C:** `make` from worktree root (no warnings).
2. **C tests:** `make tests` — full `tst/` regression must pass; these
   exercise every public draw call, so signature breakage shows up
   immediately. Update the test files to pass `NULL` as the leading
   `layer` arg where they currently call `pico_output_draw_*` /
   `pico_output_clear` / `pico_cv_*` / `pico_vs_*` / `pico_get_mouse` /
   `pico_set_mouse` / `pico_get_text` / `pico_layer_text` without one.
3. **Visual regressions:** `make test T=<app>` for each `tst/<app>` —
   pixel-equality against `asr/` baselines (no baseline regen needed
   since rendered output is unchanged).
4. **Lua build:** `cd lua && make`.
5. **Lua tests:** `cd lua && make tests`.
6. **Targeted checks:**
   - Pass an explicit `layer` to `pico_output_draw_rect` for a layer
     that is NOT the current one; confirm pencil color / style / clip
     are taken from that layer, not from `S.layer`.
   - Pass `NULL`; confirm behavior is identical to before.
7. **valgrind smoke:** `make tests V=1` (uses `valgrind.supp`); ensure
   `src:pico.c:N` line is updated if `pico_init` shifted.

## Out of Scope

- No semantic change to `S.layer` itself — it remains the implicit
  current render target.
- No batching / no caching of `_pico_layer_null` results across calls.
- No change to existing getter/setter signatures (already conform).
