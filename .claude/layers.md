# Layers

Off-screen textures with independent views for compositing.

## Status

### Complete

- Window/view API split: `pico_set_window`, `pico_get_window`, `pico_set_dim`
- View API: `pico_set_view`, `pico_get_view`
- Layer API: `pico_set_layer`, `pico_get_layer`
- Layer creation: `pico_layer_empty`, `pico_layer_image`, `pico_layer_buffer`,
  `pico_layer_text`
- Layer compositing: `pico_output_draw_layer`
- Lua bindings for all layer functions

### TODO: Per-Layer State

Currently global state applies to all layers. Future work: per-layer settings.

- target (where to draw)
- source (crop)
- clip, tile
- rotation, flip, grid

---

## Design

### Concept

- Layers are named off-screen textures
- NULL layer is main/default (active at start)
- Stored in TTL hash with `PICO_KEY_LAYER` type
- Manual compositing via `pico_output_draw_layer()`

### Naming Convention

| Source | Key when name=NULL | Example |
|--------|-------------------|---------|
| Image file | `path` directly | `hero.png` |
| Text render | `/text/<font>/<height>/<r>.<g>.<b>/<text>` | `/text/null/12/...` |
| Buffer | N/A (name required) | `buf1` |
| Empty | N/A (name required) | `background` |

### Constraints

- `pico_output_present()` asserts layer is NULL
- `pico_set_dim()` asserts layer is NULL
- `pico_set_layer(name)` asserts layer exists
- Auto-present only when drawing to main layer

### API

| Function | Description |
|----------|-------------|
| `pico_set_window(title, fs, dim)` | Window settings |
| `pico_set_dim(dim)` | Window + main layer dimensions |
| `pico_set_layer(name)` | Switch to existing layer |
| `pico_get_layer()` | Get current layer name |
| `pico_layer_empty(name, dim)` | Create empty layer |
| `pico_layer_image(name, path)` | Create from image |
| `pico_layer_buffer(name, dim, pixels)` | Create from buffer |
| `pico_layer_text(name, height, text)` | Create from text |
| `pico_output_draw_layer(name, rect)` | Draw layer to current |

### Example

```c
// Create layers
pico_layer_empty("bg", (Pico_Abs_Dim){64, 64});
const char* hero = pico_layer_image(NULL, "hero.png");

// Draw to bg layer
pico_set_layer("bg");
pico_output_draw_rect(...);

// Compose to main
pico_set_layer(NULL);
pico_output_draw_layer("bg", rect);
pico_output_draw_layer(hero, rect);
pico_output_present();
```
