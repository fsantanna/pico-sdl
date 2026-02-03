Layers feature for pico-sdl.

## Status

### Done

| Item | Notes |
|------|-------|
| `pico_set_window(title, fs, dim)` | Window settings split from view |
| `pico_get_window(title, fs, dim)` | |
| `pico_set_dim(dim)` | Window + main layer same dim, asserts layer==NULL |
| `pico_set_view(grid, dim, target, source, clip, tile)` | Renamed params |
| `pico_get_view(...)` | |
| `pico_get_layer()` | Returns G.layer |
| `pico_set_layer(name)` | Switches render target, stores in G.layer |
| `pico_layer_empty(name, dim)` | Creates texture, stores in hash (Pico_Abs_Dim) |
| `pico_output_present()` constraint | Asserts layer==NULL |
| `pico_set_dim()` constraint | Asserts layer==NULL |
| Auto-present skip | Skips when G.layer != NULL |
| Hash key rename | `Pico_Key` with `PICO_KEY_*` types |
| `pico_output_draw_layer(name, rect)` | Composite layer to current layer |
| Per-layer view settings | `Pico_View` struct, `S.layer->view` |
| Refactor S/G structs | `S.win`, `S.layer`, `S.grid`, `G.main` |
| `pico_layer_image(name, path)` | Returns `path` when name=NULL (simplified) |
| Unified images as layers | Removed `PICO_KEY_IMAGE`, all images are `PICO_KEY_LAYER` |
| `pico_output_draw_image` refactor | Uses `pico_layer_image` + `pico_output_draw_layer` |
| `pico_layer_buffer(name, dim, pixels)` | Create layer from buffer, reuses if exists |
| `pico_output_draw_buffer` refactor | Uses `pico_layer_buffer` + `pico_output_draw_layer` |
| `pico_layer_empty` reuse fix | Check-first pattern like `_pico_layer_image` |
| Lua bindings for layers | `pico.get.layer`, `pico.set.layer`, `pico.layer.empty/image/buffer`, `pico.output.draw.layer` |
| Lua API rename | `pico.set.view{world=}` → `{dim=}` |
| Lua test fixes | Split `set.view{title,window}` → `set.window` + `set.view` (12 files) |
| Lua `set.layer(nil)` fix | Handle nil argument to switch to main layer |
| `pico_output_draw_buffer(name,...)` | Name now required (was pointer-based, broken in Lua) |
| Lua `c_buffer_dim`/`c_buffer_fill` | Extracted helpers for buffer parsing |
| `pico_layer_text(name, height, text)` | Create layer from text (uses current font/color) |
| `pico_output_draw_text` refactor | Uses `pico_layer_text` + `pico_output_draw_layer` |
| Lua `pico.layer.text(name, height, text)` | Lua binding for text layers |
| Simplified image key | Image uses `path` directly (no `/image/` prefix) |
| Removed `/` assertions | User can use any name, collision risk accepted |

### Remaining

| Item | Description |
|------|-------------|
| Optimize `pico_output_draw_image` | Remove redundant layer lookup (see below) |

### TODO: Optimize `pico_output_draw_image`

Current implementation does two layer lookups:
1. `pico_get_image` → `_pico_layer_image` (creates/caches)
2. `pico_layer_image` → looks up same layer again

The `pico_get_image` call exists to update `rect->w`/`rect->h` for caller.

**Proposed fix:** Use `_pico_layer_image` directly, update rect from `layer->view.dim`:
```c
void pico_output_draw_image (const char* path, Pico_Rel_Rect* rect) {
    Pico_Layer* layer = _pico_layer_image(NULL, path);
    if (rect->w == 0) rect->w = layer->view.dim.w;  // mode-aware?
    if (rect->h == 0) rect->h = layer->view.dim.h;
    pico_output_draw_layer(layer->key->key, rect);
}
```

**Issue:** rect dimensions are mode-dependent (`!`, `%`, `#`). Need to convert
`layer->view.dim` (absolute) to rect's mode. Current `pico_get_image` handles
this via `_sdl_dim`. May need similar logic or accept absolute-only update.

---

## Design

### Concept

- Layers are named off-screen textures with independent views
- NULL layer is main/default (active at start)
- Stored in TTL hash with `PICO_KEY_LAYER` type
- Manual compositing via `pico_output_draw_layer()`

### Naming Convention

| Source | Key when name=NULL | Example |
|--------|-------------------|---------|
| Image file | `path` directly | `hero.png` |
| Text render | `/text/<font>/<height>/<r>.<g>.<b>/<text>` | `/text/null/12/255.255.255/Hello` |
| Buffer | N/A (name required) | `buf1` |
| Empty | N/A (name required) | `background` |

- Image keys use path directly for simplicity
- Text auto-naming uses `/text/...` for caching (font/height/color/text)
- Buffer and empty layers require explicit name
- No `/` prefix restrictions (user collision risk accepted)

### Constraints

- `pico_output_present()` → asserts layer is NULL ✓
- `pico_set_dim()` → asserts layer is NULL ✓
- `pico_set_layer(name)` → asserts layer exists in hash ✓
- Auto-present only when drawing to main layer (NULL) ✓

### API - New

| Function | Returns | Description | Status |
|----------|---------|-------------|--------|
| `pico_set_window(title, fs, dim)` | void | Window settings | ✓ |
| `pico_set_dim(dim)` | void | Window + main layer same dim | ✓ |
| `pico_set_layer(name)` | void | Switch to existing layer | ✓ |
| `pico_get_layer()` | name | Get current layer name | ✓ |
| `pico_layer_empty(name, dim)` | name | Create empty layer | ✓ |
| `pico_layer_image(name, path)` | name | Create from image | ✓ |
| `pico_layer_buffer(name, dim, pixels)` | name | Create from buffer | ✓ |
| `pico_layer_text(name, height, text)` | name | Create from text | ✓ |
| `pico_output_draw_layer(name, rect)` | void | Draw layer to current | ✓ |

### Destination Logic

- `pico_set_view(..., target, ...)` - stored in layer (can be NULL)
- `pico_output_draw_layer(name, rect)` - per-call (can be NULL)
- rect takes precedence if set
- Falls back to layer's target from view
- Assert fails if both are NULL

### Global State

- alpha, color, style, expert, font

### Per-Layer State (future)

- dim
- target (where to draw)
- source (crop)
- clip, tile
- rotation, flip, grid

### Workflow Example

```c
// User layer (empty, configurable)
pico_layer_empty("bg", (Pico_Abs_Dim){64, 64});
pico_set_layer("bg");
pico_output_draw_rect(...);

// Content layer (auto-sized)
const char* p = pico_layer_image(NULL, "hero.png");  // returns "hero.png"

// Compose
pico_set_layer(NULL);
pico_output_draw_layer("bg", rect);
pico_output_draw_layer(p, rect);
pico_output_present();
```
