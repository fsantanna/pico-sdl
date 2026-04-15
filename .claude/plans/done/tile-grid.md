# Plan: tile-grid

## Context

In tile mode (`'#'`), the view defines a grid of tiles (e.g., 4x4 tiles
of 4x4 pixels each).
The current `_show_grid()` draws pixel-boundary lines as a window overlay
but has no awareness of tile boundaries.
Goal: add a tile grid that draws brighter lines at tile boundaries, working
per-layer (any layer with tile mode and grid enabled shows its tile grid).

## Design decisions

- **Pixel grid** stays in `_show_grid()`, window-level, main layer only
  (unchanged).
- **Tile grid** is drawn per-layer in `_pico_output_draw_layer()` after
  `SDL_RenderCopyEx`, using the computed `dst` rect.
- Tile grid defaults to **off** for non-main layers (`mem.hc:85` already
  has `.grid = 0`).
- **'G' key** only toggles main layer's pixel grid (unchanged).
  Per-layer tile grid is controlled via `pico_set_view(grid, ...)`.
- Tile grid color: white `{0xFF, 0xFF, 0xFF}` at alpha `0xAA`.

## Changes

### 1. `src/layers.hc` | `_pico_output_draw_layer()` (after RenderCopyEx)

Tile grid for non-main layers, using the computed `dst` rect.

### 2. `src/pico.c` | `_show_grid()` (after pixel grid block)

Tile grid for the main layer in window space.

## Files

| File             | Place                       | Description                  |
|------------------|-----------------------------|------------------------------|
| `src/layers.hc`  | `_pico_output_draw_layer()` | tile grid after RenderCopyEx |
| `src/pico.c`     | `_show_grid()`              | tile grid for main layer     |

## Status

- [x] Add tile grid in `_show_grid()` for main layer
- [x] Add tile grid in `_pico_output_draw_layer()` for non-main layers
