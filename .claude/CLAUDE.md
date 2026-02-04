# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with
code in this repository.

## Overview

pico-sdl is a C graphics library for developing 2D games and applications,
targeting educational use.
It's implemented as a facade over SDL2, providing simplified APIs organized
into three groups:

- `pico_output_*` for output operations (drawing shapes, playing audio)
- `pico_input_*` for input events (waiting time, key presses)
- `pico_get_*` and `pico_set_*` for library state management

The library features immediate display through single-buffer rendering,
allowing real-time visual feedback as operations execute.

## Key Design Principles

- **Single-buffer rendering**: Changes are visible immediately, enabling
  step-by-step visual debugging
- **Sensible defaults**: Black background, white foreground, built-in font,
  grid enabled
- **Standardized naming**: Consistent `pico_output_*`, `pico_input_*`,
  `pico_get_*`, `pico_set_*` patterns
- **Educational focus**: Visual aids (grid), zoom/scroll support, immediate
  feedback prioritized over performance
- **SDL2 facade**: Simplifies SDL2 while allowing direct SDL2 fallback when
  needed

## Build and Run Commands

### C Programs

Compile and run a C program:

```bash
./pico-sdl path/to/program.c
```

This script:

- Compiles the program with `src/pico.c`
- Links against SDL2 libraries:
  `-lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -lSDL2_gfx`
- Runs the executable from the program's directory

Manual compilation:

```bash
gcc -Wall -g -o output program.c src/pico.c \
    -I src \
    -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -lSDL2_gfx
```

### Lua Bindings

The `lua/` directory contains Lua bindings for pico-sdl.

Run a Lua program:

```bash
pico-lua path/to/program.lua
```

Build Lua native module manually:

```bash
gcc -shared -o pico_native.so -fPIC src/pico.c lua/pico.c \
    -llua5.4 -lSDL2 -lSDL2_gfx -lSDL2_ttf -lSDL2_mixer -lSDL2_image
```

Install via luarocks:

```bash
sudo luarocks make pico-sdl-0.1-2.rockspec
```

### Documentation

Build API documentation (requires Doxygen <= 1.9.4):

```bash
cd docs
./build.sh
```

## Architecture

### Core Implementation

The library is implemented in `src/`:

- `pico.h` - Main API header with all function declarations and type
  definitions
- `pico.c` - Core implementation wrapping SDL2 functionality
- `hash.h` - Hash table for resource management (uses ttl-hash library)
- `tiny_ttf.h` - Embedded font for default text rendering
- `keys.h` - Keyboard key constants (`PICO_KEY_*`)
- `events.h` - Event type constants (PICO_KEYUP, PICO_MOUSEBUTTONDOWN, etc.)
- `colors.h` - Color types (`Pico_Color`, `Pico_Color_A`) and predefined color
  constants (`PICO_COLOR_RED`, `PICO_COLOR_BLUE`, etc.)
- `anchors.h` - Anchor constants for positioning (`PICO_ANCHOR_C`,
  `PICO_ANCHOR_NW`, etc.)

### State Management

The library maintains global state in a static struct `S` (in pico.c)
containing:

- Alpha blending value
- Rotation angle
- Colors (clear and draw)
- Crop rectangle
- Expert mode flag (disables automatic present)
- Flip state
- Font path
- Grid display flag
- Drawing style (`PICO_STYLE_FILL` or `PICO_STYLE_STROKE`)
- View configuration (physical/logical dimensions, fullscreen, clipping)

### Type System

The library uses two categories of types:

**Absolute types** (fixed pixel values):
- `Pico_Abs_Rect` - Rectangle in logical pixels (SDL_Rect alias)
- `Pico_Abs_Pos` - Position in logical pixels (SDL_Point alias)
- `Pico_Abs_Dim` - Dimensions in logical pixels (w, h)

**Relative types** (mode-aware, supports anchoring and hierarchy):
- `Pico_Rel_Rect` - Rectangle with mode, anchor, and parent reference
- `Pico_Rel_Pos` - Position with mode, anchor, and parent reference
- `Pico_Rel_Dim` - Dimensions with mode and parent reference

Relative types have a `mode` field (first field) that determines how values
are interpreted:
- `'!'` - Raw/absolute mode: values are logical pixel coordinates
- `'%'` - Percentage mode: values are normalized (0.0-1.0)
- `'#'` - Tile mode: 1-indexed grid coordinates (requires tile size in view)
- `'*'` - Mixed mode
- `'?'` - Unknown/error

Example usage:
```c
// Raw mode: 5,5 position with 4x4 size in logical pixels
Pico_Rel_Rect r = { '!', {5, 5, 4, 4}, PICO_ANCHOR_C, NULL };

// Percentage mode: centered at 50%,50% with 40%x40% size
Pico_Rel_Rect r = { '%', {0.5, 0.5, 0.4, 0.4}, PICO_ANCHOR_C, NULL };

// Tile mode: tile (2,3) with 1x1 tile size (requires pico_set_view with tile)
Pico_Rel_Rect r = { '#', {2, 3, 1, 1}, PICO_ANCHOR_NW, NULL };
```

### Tile Mode

Tile mode (`'#'`) enables grid-based positioning for tile-based games.
Coordinates are 1-indexed to match Lua table conventions.

**Setup:**
```c
// 4x4 grid of 4x4 pixel tiles = 16x16 logical world
Pico_Rel_Dim log  = { '#', {4, 4}, NULL };  // 4x4 tiles
Pico_Abs_Dim tile = { 4, 4 };               // each tile is 4x4 pixels
pico_set_window("Game", -1, NULL);
pico_set_view(-1, &log, NULL, NULL, NULL, &tile);
```

**Coordinate conversion (symmetric anchoring):**
- Position: `pixel = (tile - 1 + anchor) * tile_size`
- Rect: `pixel = (tile - 1 + anchor) * tile_size - anchor * object_size`
- Size: `pixels = tiles * tile_size`
- Mouse: `tile = (pixel / tile_size) + (1 - anchor)`

**Anchoring:** Anchor applies symmetrically to both the tile cell reference
point and the object alignment point. With anchor C, tile 1 references the
center of the first tile cell, and drawing aligns the object's center there.

### Color Types

The library provides two color types (defined in `colors.h`):

- **Pico_Color**: RGB color without alpha (3 bytes: r, g, b)
    - Used for setting drawing and clear colors via `pico_set_color_draw()`
      and `pico_set_color_clear()`
    - Predefined constants: `PICO_COLOR_RED`, `PICO_COLOR_GREEN`,
      `PICO_COLOR_BLUE`, `PICO_COLOR_WHITE`, `PICO_COLOR_BLACK`, etc.

- **Pico_Color_A**: RGBA color with per-pixel alpha (4 bytes: r, g, b, a)
    - Used for buffer drawing functions: `pico_output_draw_buffer()`
    - Each pixel can have its own alpha value (0-255)
    - Alpha values are applied directly from the buffer data (global alpha
      from `pico_set_alpha()` is not used)

### Coordinate Systems and API Suffixes

The library supports two coordinate systems with corresponding API suffixes:

1. **Raw (Absolute)**: Functions ending in `_raw` use logical pixel coordinates
   - Example: `pico_output_draw_image_raw(path, rect)`
   - Uses `Pico_Rel_*` types with mode `'!'`

2. **Percentage-based**: Functions ending in `_pct` use normalized coordinates
   (0.0-1.0)
   - Example: `pico_output_draw_image_pct(path, rect)`
   - Uses `Pico_Rel_*` types with mode `'%'`
   - Supports hierarchical positioning with `anchor` and `up` (parent) fields

Some functions accept `Pico_Rel_*` types directly and interpret the mode field:
- `pico_output_draw_rect(Pico_Rel_Rect* rect)` - mode determines coordinate
  interpretation

### Anchors

Anchors define the reference point for positioning (defined in `anchors.h`):

**Basic values** (can be combined):
- `PICO_ANCHOR_LEFT` (0), `PICO_ANCHOR_CENTER` (0.5), `PICO_ANCHOR_RIGHT` (1)
- `PICO_ANCHOR_TOP` (0), `PICO_ANCHOR_MIDDLE` (0.5), `PICO_ANCHOR_BOTTOM` (1)

**Cardinal direction presets** (`Pico_Pct` structs):
- `PICO_ANCHOR_NW`, `PICO_ANCHOR_N`, `PICO_ANCHOR_NE`
- `PICO_ANCHOR_W`, `PICO_ANCHOR_C`, `PICO_ANCHOR_E`
- `PICO_ANCHOR_SW`, `PICO_ANCHOR_S`, `PICO_ANCHOR_SE`

**Special values**:
- `PICO_ANCHOR_X` - Unset/invalid anchor (-1, -1)
- `PICO_ANCHOR_C` - Center (equivalent to CENTER, MIDDLE)

The library maintains both physical (window) and logical (game world)
dimensions, with automatic scaling between them.
Target mode (`TGT` variable) determines which coordinate space is active.

### Resource Management

Resources (images, sounds) are cached in a hash table (`_pico_hash`) with
TTL-based eviction to avoid reloading. The hash implementation uses:

- Buckets: `PICO_HASH_BUK` (128 by default)
- TTL: `PICO_HASH_TTL` (1000 ticks)
- Key: `Pico_Res` struct containing resource type and file path
- Value: SDL texture or Mix_Chunk pointer

### Rendering Pipeline

1. Operations draw to a logical texture (`TEX`) when in logical mode
2. In immediate mode (default), display updates instantly after each operation
3. In expert mode (`pico_set_expert(1)`), rendering is buffered until
   `pico_output_present()` is called
4. Grid overlay (enabled by default) shows logical pixel boundaries for
   debugging

### Utility Functions

The library provides utility functions for common operations:

**Conversion functions** (`pico_cv_*`):
- `pico_cv_rect_pct_raw()` - Convert percentage rect to absolute
- `pico_cv_pos_rel_abs()` - Convert relative position to absolute
- `pico_cv_rect_rel_abs()` - Convert relative rect to absolute

**Collision detection** (`pico_vs_*`):
- `pico_vs_pos_rect()` - Check if point is inside rectangle
- `pico_vs_rect_rect()` - Check if two rectangles overlap
- `pico_vs_rect_rect_pct()` - Same with percentage coordinates

**Color manipulation**:
- `pico_color_darker(color, pct)` - Make color darker
- `pico_color_lighter(color, pct)` - Make color lighter

### Test Structure

The `tst/` directory contains example programs demonstrating features:
- Individual test files for specific features:
    `anchor_abs.c`, `anchor_pct.c`, `blend_abs.c`, `blend_pct.c`,
    `buffer_abs.c`, `collide_abs.c`, `colors.c`, `cv.c`, `vs.c`,
    `tiles.c`, etc.
- Files with `_abs` suffix test raw/absolute coordinate APIs (mode `'!'`)
- Files with `_pct` suffix test percentage-based APIs (mode `'%'`)
- `tiles.c` tests tile mode APIs (mode `'#'`)
- Files prefixed with `todo_` are work-in-progress tests

### Lua Bindings

The `lua/` directory contains Lua 5.4 bindings implemented in `lua/pico.c`.

**Type Mapping:**

Lua tables map to C relative types with the following conventions:
- `{'!', x=10, y=20, w=30, h=40}` → `Pico_Rel_Rect` with mode `'!'`
- `{'%', x=0.5, y=0.5, anc='C'}` → `Pico_Rel_Pos` with anchor
- `{'#', x=2, y=3, w=1, h=1, anc='NW'}` → `Pico_Rel_Rect` with tile mode
- `{'!', w=0, h=0}` → `Pico_Rel_Dim` for dimension queries

Conversion functions in `lua/pico.c`:
- `c_rel_rect()` - Converts Lua table to `Pico_Rel_Rect*`
- `c_rel_pos()` - Converts Lua table to `Pico_Rel_Pos*`
- `c_rel_dim()` - Converts Lua table to `Pico_Rel_Dim*`
- `c_anchor()` - Extracts anchor from table's `anc` field (string or table)
- `c_color()` - Parses color as string (`'red'`), table (`{r,g,b}`), or args

**API Structure:**

The Lua API mirrors the C API with nested tables:
- `pico.get.image(path, [dim])` - Get image dimensions
- `pico.get.mouse(pos)` - Get mouse position (updates pos.x, pos.y based on
  pos mode: `'!'`, `'%'`, or `'#'`)
- `pico.get.text(text, dim)` - Get text dimensions
- `pico.get.view()` - Get view settings (returns table with tile field)
- `pico.set.color.draw(color)` - Set drawing color
- `pico.set.view({...})` - Set view (accepts tile={w,h} for tile mode)
- `pico.output.draw.rect(rect)` - Draw rectangle
- `pico.input.event([filter], [timeout])` - Wait for input event

**Tile Mode Example:**
```lua
-- Setup 4x4 grid with 4x4 pixel tiles
pico.set.view {
    window = {'!', w=160, h=160},
    world  = {'#', w=4, h=4},
    tile   = {w=4, h=4}
}

-- Draw at tile position (2,3)
local r = {'#', x=2, y=3, w=1, h=1, anc='NW'}
pico.output.draw.rect(r)

-- Get mouse in tile coordinates
local pos = {'#', x=0, y=0}
pico.get.mouse(pos)
print(pos.x, pos.y)  -- 1-indexed tile position
```

**Constants:**

Predefined constants are stored in the Lua registry and accessed by name:
- Anchors: `'C'`, `'NW'`, `'N'`, `'NE'`, `'E'`, `'SE'`, `'S'`, `'SW'`, `'W'`
- Colors: `'red'`, `'green'`, `'blue'`, `'white'`, `'black'`, etc.
- Styles: `'fill'`, `'stroke'`
- Events: `'quit'`, `'key.dn'`, `'key.up'`, `'mouse.button.dn'`

**Memory Management:**

Relative type structs are allocated as Lua userdata via `lua_newuserdata()`,
ensuring proper lifetime management by the Lua garbage collector.

## Dependencies

Required SDL2 libraries:

- `libsdl2-dev` - Core SDL2
- `libsdl2-image-dev` - Image loading
- `libsdl2-mixer-dev` - Audio playback
- `libsdl2-ttf-dev` - TrueType font rendering
- `libsdl2-gfx-dev` - Additional graphics primitives

Install on Ubuntu/Debian:

```bash
sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev \
    libsdl2-ttf-dev libsdl2-gfx-dev
```

## Testing

### Running Tests

```bash
make tests
```

For headless environments (CI/CD, servers without displays), use Xvfb:

```bash
xvfb-run make tests
```

### Visual Regression Testing

Visual tests use `check.h` to compare rendered output against expected images.

**Compilation modes** (can be combined):

- No defines - Generate mode: writes to `out/`, no pause, no assert
- `-DPICO_CHECK_INT` - Interactive: pauses for visual inspection
- `-DPICO_CHECK_ASR` - Assert: compares against expected images in `asr/`
- Both defines - Interactive with assertion: pause AND assert

**Directory structure:**
- `tst/asr/` - Expected (reference) images (manually maintained)
- `tst/out/` - Generated output images (auto-created during tests)

**Naming convention:**
- Format: `filename-XX.png` (e.g., `anchor_pct-01.png`, `anchor-02.png`)
- `filename` matches the test file name (or a common prefix for shared tests)
- `XX` is the sequential test number (01, 02, 03, ...)

**Workflow:**

1. **Developing new tests** (generate mode):
   ```bash
   PICO_CHECK_INT= ./pico-sdl tst/newtest.c
   # Review images in tst/out/
   # If correct, manually copy to asr/:
   cp tst/out/newtest-*.png tst/asr/
   ```

2. **Interactive development** (default):
   ```bash
   ./pico-sdl tst/anchor_pct.c  # Pauses for visual inspection
   ```

3. **Automated testing** (CI/CD):
   ```bash
   PICO_CHECK_INT= PICO_CHECK_ASR=1 ./pico-sdl tst/anchor_pct.c
   make tests  # uses ASR mode
   ```

4. **Interactive with validation**:
   ```bash
   PICO_CHECK_ASR=1 ./pico-sdl tst/anchor_pct.c
   # Pauses AND asserts against asr/ files
   ```

**Headless Testing:**

Xvfb (X Virtual Frame Buffer) provides a virtual display server for headless
environments:
- Performs full graphical rendering in memory
- Produces real pixel data for screenshot comparison
- Essential for CI/CD pipelines and automated testing

```bash
xvfb-run ./pico-sdl tst/anchor_pct.c
make tests  # automatically uses xvfb and ASR mode
```

**In test code:**

```c
#include "pico.h"
#include "../check.h"

int main(void) {
    pico_init(1);
    pico_output_clear();

    // Using percentage mode
    Pico_Rel_Rect r = { '%', {0.5, 0.5, 0.4, 0.4}, PICO_ANCHOR_C, NULL };
    pico_output_draw_rect(&r);
    _pico_check("anchor_pct-01");  // Writes to out/, compares with asr/

    pico_init(0);
    return 0;
}
```

### Valgrind

- `valgrind.supp` contains the `sdl-init` suppression that references a
  specific line in `src/pico.c` (the `SDL_Init` call in `pico_init`).
- Whenever that line number changes, update the `src:pico.c:<line>` reference
  in `valgrind.supp` accordingly.