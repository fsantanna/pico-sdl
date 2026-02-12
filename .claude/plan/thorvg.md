# Plan: ThorVG + pico-sdl Integration

## Repository
- https://github.com/fsantanna/pico-sdl

---

## 1. pico-sdl Architecture Summary

### Rendering Pipeline
- Uses `SDL_Renderer` (accelerated or software)
- All drawing targets are `SDL_Texture` (`SDL_TEXTUREACCESS_TARGET`)
- Pixel format: `SDL_PIXELFORMAT_RGBA32` throughout
- Flow: draw to layer texture -> composite to screen

### Layer System (`Pico_Layer`)
- Each layer = `SDL_Texture` + view config (dim, src, dst, clip, etc.)
- Main layer: `G.main` (renders to screen via `_pico_output_present`)
- Sub-layers: cached in a hash table by name

### Drawing Primitives (current)
- **Basic** (SDL_Renderer): pixels, lines, rects
- **Complex** (SDL2_gfxPrimitives): ellipses, triangles, polygons
- **Raster** (SDL_image): PNG/JPG image loading
- **Text** (SDL_ttf): font rendering to surface -> texture
- **Buffers**: raw RGBA pixel data -> surface -> texture

### Key Entry Points for Integration
- `pico_output_draw_buffer(name, dim, buffer, rect)` — draws RGBA
  buffer as a layer
- `pico_layer_buffer(name, dim, pixels)` — creates a cached layer
  from RGBA pixel data

## 2. ThorVG Overview

- Lightweight C/C++ vector graphics library
- Renders SVG, Lottie animations, TVG (binary vector format)
- **SW Canvas**: renders to a raw `uint32_t*` pixel buffer (ARGB8888)
- MIT licensed, ~1MB footprint
- C API: `thorvg_capi.h`

## 3. Integration Strategy

### Approach A: Buffer Bridge (Recommended)

ThorVG renders to a CPU buffer, pico-sdl consumes it via the
existing `pico_layer_buffer` / `pico_output_draw_buffer` path.

```
ThorVG SW Canvas -> uint32_t* buffer (ARGB8888)
                        |
                   color convert (ARGB -> RGBA)
                        |
                   pico_layer_buffer(name, dim, pixels)
                        |
                   pico_output_draw_layer(name, rect)
```

**Pros:**
- Zero changes to pico-sdl core rendering
- Clean separation — ThorVG is an optional add-on
- Works with both HW and SW renderers
- Fits pico-sdl's existing caching model

**Cons:**
- CPU-only rendering (no GPU acceleration for vector ops)
- Color space conversion overhead (ARGB -> RGBA)
- Buffer copy on every frame (for animations)

### Approach B: Direct SDL Texture (Alternative)

ThorVG renders to a locked SDL_Texture directly.

```
SDL_LockTexture(tex) -> pixel buffer
ThorVG SW Canvas renders to buffer
SDL_UnlockTexture(tex)
SDL_RenderCopy(ren, tex, ...)
```

**Pros:**
- Avoids one buffer copy
- Slightly better performance

**Cons:**
- Requires `SDL_TEXTUREACCESS_STREAMING` (pico-sdl uses `_TARGET`)
- Breaks the layer system (layers need to be render targets)
- More invasive changes to pico-sdl internals

### Recommendation: **Approach A**

It aligns with pico-sdl's design philosophy of simplicity and
requires no changes to the core library. ThorVG becomes an
external utility that feeds into the existing buffer/layer pipeline.

## 4. Proposed API

### New Functions (in a separate `pico_tvg.h` / `pico_tvg.c`)

```c
// Load and render an SVG file as a pico-sdl layer
const char* pico_layer_svg (
    const char* name,
    const char* path,
    Pico_Abs_Dim dim
);

// Load and render a Lottie JSON as a pico-sdl layer
const char* pico_layer_lottie (
    const char* name,
    const char* path,
    Pico_Abs_Dim dim
);

// Update a Lottie animation to a specific frame
int pico_set_lottie (const char* name, int frame);

// Draw SVG directly (convenience wrapper)
void pico_output_draw_svg (
    const char* path,
    Pico_Rel_Rect* rect
);
```

### Usage Example

```c
#include "pico.h"
#include "pico_tvg.h"

int main (void) {
    pico_init(1);

    // Draw an SVG icon centered on screen
    pico_output_draw_svg(
        "icon.svg",
        &(Pico_Rel_Rect){'%', {.5,.5,.3,.3},
            PICO_ANCHOR_C, NULL}
    );

    pico_input_event(NULL, PICO_EVENT_QUIT);
    pico_init(0);
}
```

## 5. Color Space Handling

ThorVG uses **ARGB8888** (A in high byte):
```
[A][R][G][B]  = 0xAARRGGBB
```

pico-sdl uses **SDL_PIXELFORMAT_RGBA32** which maps to:
```
[R][G][B][A]  in memory (byte order)
```

Conversion needed per pixel:
```c
// ARGB -> RGBA (byte-level swap)
uint32_t argb = thorvg_buffer[i];
uint8_t a = (argb >> 24) & 0xFF;
uint8_t r = (argb >> 16) & 0xFF;
uint8_t g = (argb >>  8) & 0xFF;
uint8_t b = (argb      ) & 0xFF;
rgba_buffer[i] = (Pico_Color_A){r, g, b, a};
```

This can be optimized with SIMD or batch conversion, but for
pico-sdl's typical canvas sizes (100x100 to ~1000x1000) the
overhead is negligible.

## 6. Build System Integration

### Dependencies
- ThorVG must be installed: `libthorvg-dev` or built from source
- ThorVG repo: https://github.com/thorvg/thorvg

### Conditional Compilation
```makefile
# Optional ThorVG support
ifdef PICO_TVG
    CFLAGS  += -DPICO_TVG $(shell pkg-config --cflags thorvg)
    LDFLAGS += $(shell pkg-config --libs thorvg)
    SRCS    += src/pico_tvg.c
endif
```

This keeps ThorVG optional — pico-sdl works without it.

## 7. Relevant Use Cases for pico-sdl

| Use Case             | Benefit                                    |
|----------------------|--------------------------------------------|
| SVG icons/sprites    | Resolution-independent, small file size    |
| Lottie animations    | Rich animations without frame-by-frame     |
| Scalable UI elements | Buttons, HUD that scale with window resize |
| Issue #46 (Logo)     | Logo could be an SVG rendered by ThorVG    |
| Issue #62 (Thickness)| ThorVG supports stroke-width natively      |

## 8. Implementation Steps

1. **Prototype** — standalone C program that:
   - Initializes ThorVG + SDL
   - Loads an SVG, renders to buffer
   - Converts ARGB->RGBA
   - Displays via `pico_output_draw_buffer`

2. **Wrap in `pico_tvg.c`** — create the API above

3. **Lottie support** — add frame-based animation using
   `pico_set_lottie` (similar to existing `pico_set_video`)

4. **Build integration** — add optional Makefile flags

5. **Tests** — SVG rendering tests (screenshot comparison)

## 9. Open Questions

- Should ThorVG be vendored (bundled) or an external dependency?
  - Vendoring: simpler for students, larger repo
  - External: cleaner, but adds install step
- Should SVG/Lottie layers be cached like images, or re-rendered
  on dimension change?
- Is the C API of ThorVG stable enough, or should we use the
  C++ API with an `extern "C"` wrapper?

---

## Pending Actions
- Decide on Approach A vs B (recommended: A)
- Decide on vendoring vs external dependency
- Prototype the buffer bridge
