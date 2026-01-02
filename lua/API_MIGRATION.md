# Pico-SDL Lua API Migration Guide

This document describes the changes from the old Lua API to the new API.

## Major Changes

### 1. Drawing Functions Now Have `_raw` and `_pct` Variants

All drawing functions now come in two variants:
- `_raw`: Uses pixel coordinates (integers)
- `_pct`: Uses percentage coordinates (floats from 0.0 to 1.0) with anchors

**Old API:**
```lua
pico.output.draw.rect({x=32, y=18, w=10, h=5})
pico.output.draw.image(pos, "image.png")
```

**New API:**
```lua
-- Raw (pixel coordinates)
pico.output.draw.rect_raw({x=32, y=18, w=10, h=5})

-- Percentage (with anchor support)
pico.output.draw.rect_pct({
    x=0.5, y=0.5,         -- 50% of world dimensions
    w=0.15, h=0.14,        -- 15% width, 14% height
    anchor=pico.anchor.C   -- Centered anchor
})

pico.output.draw.image_pct({
    x=0.5, y=0.5,
    w=0, h=0,              -- 0 means use original dimensions
    anchor=pico.anchor.C
}, "image.png")
```

### 2. View Management Replaced Individual Setters

**Old API:**
```lua
pico.set.dim.window({x=800, y=600})
pico.set.dim.world({x=100, y=100})
pico.set.scroll({x=10, y=10})
pico.set.zoom({x=1.5, y=1.5})
```

**New API:**
```lua
-- Set view using pico.set.view_raw
pico.set.view_raw(
    false,                    -- fullscreen
    {w=800, h=600},           -- window dimensions
    nil,                      -- window_target (nil = keep current)
    {w=100, h=100},           -- world dimensions
    {x=10, y=10, w=100, h=100}, -- world_source (for panning)
    nil                       -- world_clip (nil = keep current)
)

-- Get current view
local view = pico.get.view()
print(view.window.x, view.window.y)
print(view.world.x, view.world.y)
```

### 3. Removed Helper Functions

The following helper functions no longer exist:
- `pico.pos()` - Use direct table creation instead
- `pico.rect()` - Use direct table creation instead
- `pico.dim()` - Use direct table creation instead
- `pico.set.cursor()` - No replacement (write/writeln removed)
- `pico.output.write()` - Use `draw.text_raw/pct` instead
- `pico.output.writeln()` - Use `draw.text_raw/pct` instead

### 4. Anchor System Changes

**Old API:**
```lua
pico.set.anchor.pos('center', 'middle')
pico.set.anchor.pos{x='center', y='middle'}
```

**New API:**
```lua
-- Anchors are now numeric values
pico.set.anchor.pos(PICO_CENTER, PICO_MIDDLE)
pico.set.anchor.pos{x=pico.anchor.CENTER, y=pico.anchor.MIDDLE}

-- Pre-defined anchor constants:
-- pico.anchor.LEFT, .CENTER, .RIGHT (x-axis)
-- pico.anchor.TOP, .MIDDLE, .BOTTOM (y-axis)
-- pico.anchor.C (center), .NW (northwest), .E (east), .SE (southeast)

-- Use anchors in _pct functions:
pico.output.draw.rect_pct({
    x=0.5, y=0.5, w=0.2, h=0.2,
    anchor=pico.anchor.C
})
```

### 5. Collision Detection Functions

**Old API:**
```lua
local hit = pico.vs.pos_rect(pos, rect)
local overlap = pico.vs.rect_rect(r1, r2)
```

**New API:**
```lua
-- Raw (pixel coordinates)
local hit = pico.collision.pos_rect_raw({x=10, y=20}, {x=0, y=0, w=100, h=100})
local overlap = pico.collision.rect_rect_raw(r1, r2)

-- Percentage coordinates
local hit = pico.collision.pos_rect_pct(
    {x=0.5, y=0.5, anchor=pico.anchor.C},
    {x=0.25, y=0.25, w=0.5, h=0.5, anchor=pico.anchor.C}
)
```

### 6. New APIs

- `pico.set.alpha(value)` - Set alpha transparency (0-255)
- `pico.get.view()` - Get all view parameters
- `pico.get.color.clear()` - Get clear color
- `pico.get.color.draw()` - Get draw color
- `pico.get.crop()` - Get crop rectangle
- `pico.get.flip()` - Get flip state
- `pico.get.font()` - Get current font
- `pico.get.title()` - Get window title

## Complete Function Mapping

### Drawing Functions
| Old API | New API |
|---------|---------|
| `draw.buffer(...)` | `draw.buffer_raw(...)` or `draw.buffer_pct(...)` |
| `draw.image(...)` | `draw.image_raw(...)` or `draw.image_pct(...)` |
| `draw.line(...)` | `draw.line_raw(...)` or `draw.line_pct(...)` |
| `draw.pixel(...)` | `draw.pixel_raw(...)` or `draw.pixel_pct(...)` |
| `draw.rect(...)` | `draw.rect_raw(...)` or `draw.rect_pct(...)` |
| `draw.oval(...)` | `draw.oval_raw(...)` or `draw.oval_pct(...)` |
| `draw.poly(...)` | `draw.poly_raw(...)` or `draw.poly_pct(...)` |
| `draw.tri(...)` | `draw.tri_raw(...)` or `draw.tri_pct(...)` |
| `draw.text(...)` | `draw.text_raw(...)` or `draw.text_pct(...)` |

### State Management
| Old API | New API |
|---------|---------|
| `get.dim.window()` | `get.view().window` |
| `get.dim.world()` | `get.view().world` |
| `set.dim.window(...)` | `set.view_raw(nil, dim, ...)` |
| `set.dim.world(...)` | `set.view_raw(nil, nil, nil, dim, ...)` |
| `set.scroll(...)` | `set.view_raw(nil, nil, nil, nil, source_rect, ...)` |
| `set.zoom(...)` | `set.view_pct(nil, nil, nil, pct, ...)` |
| `set.scale(...)` | No direct replacement - use view |

## Example: Simple Program

```lua
local pico = require 'pico'

pico.init(true)
pico.set.title "Hello Pico!"

-- Set colors
pico.set.color.clear(0x00, 0x00, 0x00)  -- Black background
pico.set.color.draw(0xFF, 0xFF, 0xFF)   -- White drawing color

-- Clear screen
pico.output.clear()

-- Draw centered rectangle using _pct
pico.output.draw.rect_pct({
    x=0.5, y=0.5,      -- Center of screen
    w=0.3, h=0.3,      -- 30% of screen size
    anchor=pico.anchor.C  -- Anchored at center
})

-- Draw text
pico.output.draw.text_pct({
    x=0.5, y=0.5,
    w=0, h=0,
    anchor=pico.anchor.C
}, "Hello Pico!")

-- Wait for key
pico.input.event('key.dn')

pico.init(false)
```

## Tips for Migration

1. **Use `_pct` for responsive layouts**: Percentage-based coordinates automatically adapt to different screen sizes.

2. **Use `_raw` for pixel-perfect positioning**: When you need exact pixel control, use the raw variants.

3. **Replace dimension getters**: Instead of calling multiple `get.dim.*` functions, call `get.view()` once and extract the values you need.

4. **Simplify view management**: Instead of calling multiple setters, call `set.view_raw` or `set.view_pct` once with all parameters.

5. **Anchor positioning**: The new anchor system is more powerful - you can specify different anchors for each object without changing global state.
