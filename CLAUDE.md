# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with
code in this repository.

## Overview

pico-sdl is a C graphics library for developing 2D games and applications,
targeting educational use.
It's implemented as a facade over SDL2, providing simplified APIs organized into three groups:

- `pico_output_*` for output operations (drawing shapes, playing audio)
- `pico_input_*` for input events (waiting time, key presses)
- `pico_get_*` and `pico_set_*` for library state management

The library features immediate display through single-buffer rendering,
allowing real-time visual feedback as operations execute.

## Build and Run Commands

### C Programs

Compile and run a C program:

```bash
./pico-sdl path/to/program.c
```

This script:

- Compiles the program with `src/pico.c` and `src/hash.c`
- Links against SDL2 libraries: `-lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -lSDL2_gfx`
- Runs the executable from the program's directory

Manual compilation:

```bash
gcc -Wall -g -o output program.c src/pico.c src/hash.c \
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
gcc -shared -o pico_native.so -fPIC src/pico.c src/hash.c lua/pico.c \
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

- `pico.h` - Main API header with all function declarations and type definitions
- `pico.c` - Core implementation wrapping SDL2 functionality
- `hash.c/hash.h` - Hash table for resource management (textures, fonts, sounds)
- `tiny_ttf.h` - Embedded font for default text rendering
- `keys.h` - Keyboard key constants (`PICO_KEY_*`)
- `events.h` - Event type constants (PICO_KEYUP, PICO_MOUSEBUTTONDOWN, etc.)

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
- Drawing style (`PICO_FILL` or `PICO_STROKE`)
- View configuration (physical/logical dimensions, fullscreen, clipping)

### Coordinate Systems

The library supports two coordinate systems, each with two API variants:

1. **Raw (Absolute)**: Functions ending in `_raw` use logical pixel coordinates
   - Example: `pico_output_draw_rect_raw(Pico_Rect rect)`

2. **Percentage-based**: Functions ending in `_pct` use normalized coordinates
   (0.0-1.0)
   - Example: `pico_output_draw_rect_pct(const Pico_Rect_Pct* rect)`
   - Supports hierarchical positioning with `anchor` and `up` (parent) fields
   - Anchor values: `PICO_ANCHOR_LEFT/CENTER/RIGHT` (0, 0.5, 1) and `TOP/MIDDLE/BOTTOM`

The library maintains both physical (window) and logical (game world)
dimensions, with automatic scaling between them.
Target mode (`TGT` variable) determines which coordinate space is active.

### Resource Management

Resources (images, fonts, sounds) are cached in a hash table (`_pico_hash`) to avoid reloading. The hash implementation uses:

- Size: `PICO_HASH` (128 by default)
- Key: Resource file path
- Value: SDL texture/font/sound pointer

### Rendering Pipeline

1. Operations draw to a logical texture (`TEX`) when in logical mode
2. In immediate mode (default), display updates instantly after each operation
3. In expert mode (`pico_set_expert(1)`), rendering is buffered until
   `pico_output_present()` is called
4. Grid overlay (enabled by default) shows logical pixel boundaries for debugging

### Test Structure

The `tst/` directory contains example programs demonstrating features:
- `main.c` - Comprehensive feature demonstration
- Individual test files for specific features:
    `anchor_pct.c`, `blend_pct.c`, `buffer_pct.c`, `collide_pct.c`, etc.
- Files with `_pct` suffix test percentage-based APIs
- Files with `_raw` suffix test absolute coordinate APIs

## Key Design Principles

- **Single-buffer rendering**: Changes are visible immediately, enabling step-by-step visual debugging
- **Sensible defaults**: Black background, white foreground, built-in font, grid enabled
- **Standardized naming**: Consistent `pico_output_*`, `pico_input_*`, `pico_get_*`, `pico_set_*` patterns
- **Educational focus**: Visual aids (grid), zoom/scroll support, immediate feedback prioritized over performance
- **SDL2 facade**: Simplifies SDL2 while allowing direct SDL2 fallback when needed

## Dependencies

Required SDL2 libraries:

- `libsdl2-dev` - Core SDL2
- `libsdl2-image-dev` - Image loading
- `libsdl2-mixer-dev` - Audio playback
- `libsdl2-ttf-dev` - TrueType font rendering
- `libsdl2-gfx-dev` - Additional graphics primitives

Install on Ubuntu/Debian:

```bash
sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev libsdl2-gfx-dev
```

## Code Style

- Comments only before blocks or functions, never inline
- If lines require comments, create an explicit block for them
