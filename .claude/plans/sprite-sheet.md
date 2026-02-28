# Plan: Sprite Sheet — `pico_layer_sub` + Lua `pico.layer.images`

## Context

pico-sdl needs sprite sheet support — load one image, create multiple layers
each showing a sub-region. Currently `pico_layer_image` creates one layer per
image with no cropping. This plan adds:

1. **C**: `pico_layer_sub(name, parent, crop)` — creates a sub-layer from
   any existing layer, sharing the parent's texture.
   Chaining supported (sub of sub).
2. **Lua**: `pico.layer.images(path, t)` — polymorphic wrapper (explicit
   name→rect + grid auto-split), returns list of names.

## Status

- [x] Step 1.1 — Add `parent` field to `Pico_Layer` struct
- [x] Step 1.2 — Add `_hash_get_layer` helper
- [x] Step 1.3 — Update `_pico_hash_clean` for sub-layers
- [x] Step 1.4 — Guard `pico_set_layer` against sub-layers
- [x] Step 1.5 — Add `_resolve_layer` + update `_pico_output_draw_layer`
- [x] Step 1.6 — Implement `pico_layer_sub`
- [x] Step 1.7 — Add `pico_layer_sub` declaration in `pico.h`
- [x] Step 1.8 — Update `valgrind.supp` line number
- [x] Step 2.1 — Add `l_layer_sub` in `lua/pico.c`
- [x] Step 2.2 — Register `sub` in `ll_layer[]`
- [x] Step 2.3 — Add `M.layer.images` in `lua/pico/init.lua`
- [x] Step 3   — Create test file `lua/tst/sheet.lua`
- [ ] Step 4   — Manual testing & verification

---

## Step 1: C Core Changes

### 1.1 — `Pico_Layer` struct (`src/pico.c:55-60`)

Add `const char* parent` field. Normal layers: `parent = NULL`.
Sub-layers: `parent = name` (pointer to hash key, no strdup).

All existing layer allocations use designated initializers, so the new
field is zero-initialized to `NULL` automatically — no changes needed
at those sites (main, empty, buffer, image, text, video).

### 1.2 — `_hash_get_layer` helper (`src/pico.c`, before `_pico_hash_clean`)

Extract a small helper to look up a layer by name in the hash. Used
in `pico_layer_sub`, `_resolve_layer`, and could replace the inline
lookups in `pico_output_draw_layer` / `pico_set_layer`.

### 1.3 — `_pico_hash_clean` (`src/pico.c:521-539`)

Skip `SDL_DestroyTexture` when `data->parent != NULL` (texture owned
by ancestor). No strdup to free either.

### 1.4 — `pico_set_layer` guard (`src/pico.c:857-875`)

After the existing assert, add:
```c
pico_assert(data->parent == NULL &&
    "cannot set render target to sub-layer");
```

### 1.5 — `_resolve_layer` + `_pico_output_draw_layer` (`src/pico.c:~1495`)

Recursive helper walks the parent chain, resolving each `view.src`
against its parent's resolved region. Returns root texture + final
absolute source rect. The `_hash_get_layer` calls refresh TTL for the
entire ancestor chain.

Update `_pico_output_draw_layer` to use `_resolve_layer` instead of
the inline `layer->tex` / `layer->view.src` access.

### 1.6 — `pico_layer_sub` (`src/pico.c`, after `pico_layer_image`)

```c
const char* pico_layer_sub (const char* name,
    const char* parent,
    const Pico_Rel_Rect* crop);
```

- Look up parent layer by name in hash
- Resolve parent's src to get absolute dimensions for sub-layer's dim
- Create Pico_Layer with tex=NULL, parent=parent_name, view.src=*crop
- Store in hash, return name

### 1.7 — `pico.h` declaration (`src/pico.h`, after `pico_layer_video`)

### 1.8 — `valgrind.supp`

Update `src:pico.c:N` after line shifts from inserted code.

---

## Step 2: Lua Bindings

### 2.1 — `l_layer_sub` in `lua/pico.c` (after `l_layer_video`)

```c
static int l_layer_sub (lua_State* L) {
    const char* name = luaL_checkstring(L, 1);
    const char* parent = luaL_checkstring(L, 2);
    luaL_checktype(L, 3, LUA_TTABLE);
    L_dim_default_wh(L, 3);
    Pico_Rel_Rect* crop = c_rel_rect(L, 3);
    const char* ret = pico_layer_sub(name, parent, crop);
    lua_pushstring(L, ret);
    return 1;
}
```

### 2.2 — Register in `ll_layer[]`

```c
{ "sub", l_layer_sub },
```

### 2.3 — `M.layer.images` in `lua/pico/init.lua`

Polymorphic wrapper:
- **Grid form** (`t.w` and `t.h`): auto-split image into grid cells
  with optional `t.n` (count) and `t.prefix`
- **Explicit form**: `name→rect` pairs

Returns list of sub-layer names.

---

## Step 3: Test File — `lua/tst/sheet.lua`

1. Create a colored quadrant test image or use an existing one
2. Test explicit form: split into named quadrants, draw each, check
3. Test grid form: auto-split with prefix, verify names, draw, check
4. Use `pico.check()` for visual regression

---

## Files Modified

| File | Change |
|------|--------|
| `src/pico.h` | Add `pico_layer_sub` declaration |
| `src/pico.c` | Add `parent` to `Pico_Layer`, `_hash_get_layer`, |
|              | `_resolve_layer`, `pico_layer_sub`, update |
|              | `_pico_hash_clean`, `pico_set_layer`, |
|              | `_pico_output_draw_layer` |
| `lua/pico.c` | Add `l_layer_sub`, register in `ll_layer[]` |
| `lua/pico/init.lua` | Add `M.layer.images` function |
| `lua/tst/sheet.lua` | **New** test file |
| `valgrind.supp` | Update `src:pico.c:N` if line shifts |
