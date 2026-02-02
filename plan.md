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

### Remaining

| Item | Description |
|------|-------------|
| `pico_output_draw_layer(name, rect)` | Composite layer to current layer |
| `pico_layer_image(name, path)` | Create layer from image (returns `/image/path`) |
| `pico_layer_buffer(name, dim, pixels)` | Create layer from buffer |
| `pico_layer_text(name, text)` | Create layer from text |
| Per-layer view settings | Currently view is global |
| Refactor draw_image/buffer/text | Make them aliases using layer_* + draw_layer |

---

## Design

### Concept

- Layers are named off-screen textures with independent views
- NULL layer is main/default (active at start)
- Stored in TTL hash with `PICO_KEY_LAYER` type
- Manual compositing via `pico_output_draw_layer()`

### Naming Convention: `/` prefix

| Prefix | Source | Dim | Example |
|--------|--------|-----|---------|
| `/image/...` | Image file | Fixed (from image) | `/image/hero.png` |
| `/text/...` | Text render | Fixed (from text) | `/text/NULL/12/Hello` |
| `/buffer/...` | Pixel buffer | Fixed (from dim) | `/buffer/0x7f...` |
| No `/` | Empty layer | Configurable | `background`, `ui` |

- `/` layers: auto-generated, dim from content, cannot change dim
- User layers: created via `pico_layer_empty()`, name must NOT start with `/`

### Constraints

- `pico_output_present()` → asserts layer is NULL ✓
- `pico_set_dim()` → asserts layer is NULL ✓
- `pico_set_layer(name)` → asserts layer exists in hash ✓
- `/` layers → cannot change dim
- Auto-present only when drawing to main layer (NULL) ✓

### API - New

| Function | Returns | Description | Status |
|----------|---------|-------------|--------|
| `pico_set_window(title, fs, dim)` | void | Window settings | ✓ |
| `pico_set_dim(dim)` | void | Window + main layer same dim | ✓ |
| `pico_set_layer(name)` | void | Switch to existing layer | ✓ |
| `pico_get_layer()` | name | Get current layer name | ✓ |
| `pico_layer_empty(name, dim)` | name | Create empty layer | ✓ |
| `pico_layer_image(name, path)` | name | Create from image | |
| `pico_layer_buffer(name, dim, pixels)` | name | Create from buffer | |
| `pico_layer_text(name, text)` | name | Create from text | |
| `pico_output_draw_layer(name, rect)` | void | Draw layer to current | |

### Destination Logic

- `pico_set_view(..., target, ...)` - stored in layer (can be NULL)
- `pico_output_draw_layer(name, rect)` - per-call (can be NULL)
- rect takes precedence if set
- Falls back to layer's target from view
- Assert fails if both are NULL

### Global State

- alpha, color, style, expert, font

### Per-Layer State (future)

- dim (fixed for `/` layers)
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

// Content layer (auto-sized) - future
const char* p = pico_layer_image(NULL, "hero.png");  // returns "/image/hero.png"

// Compose
pico_set_layer(NULL);
pico_output_draw_layer("bg", rect);
pico_output_draw_layer(p, rect);
pico_output_present();
```
