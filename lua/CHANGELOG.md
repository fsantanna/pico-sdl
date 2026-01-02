# Lua Bindings Changelog

## Major Update - New API

This update brings the Lua bindings in sync with the latest pico-SDL C API, introducing significant breaking changes.

### Summary of Changes

1. **Complete rewrite of Lua bindings** (lua/pico.c)
   - All drawing functions now have `_raw` and `_pct` variants
   - Added new view management functions
   - Added missing getter functions
   - Removed deprecated helper functions

2. **Updated examples** in lua/tst/:
   - main.lua - Comprehensive test updated to new API
   - image.lua - Image drawing examples
   - pct.lua - Percentage-based positioning
   - pixels.lua - Pixel drawing
   - collide.lua - Collision detection
   - hello.lua - New simple example (NEW)

3. **Documentation**:
   - API_MIGRATION.md - Complete migration guide
   - CHANGELOG.md - This file

### API Changes

#### New Drawing API

All drawing functions now require explicit `_raw` or `_pct` suffix:

```lua
-- OLD (no longer works):
pico.output.draw.rect({x=10, y=10, w=20, h=20})

-- NEW - Raw coordinates:
pico.output.draw.rect_raw({x=10, y=10, w=20, h=20})

-- NEW - Percentage coordinates with anchor:
pico.output.draw.rect_pct({
    x=0.5, y=0.5, w=0.3, h=0.3,
    anchor=pico.anchor.C
})
```

#### View Management

Replaced individual dimension setters with unified view functions:

```lua
-- OLD (no longer works):
pico.set.dim.window({x=800, y=600})
pico.set.dim.world({x=100, h=100})
local win_dim = pico.get.dim.window()

-- NEW:
pico.set.view_raw(nil, {w=800, h=600}, nil, {w=100, h=100}, nil, nil)
local view = pico.get.view()
print(view.window.x, view.window.y)
```

#### Removed Functions

- `pico.pos()` - Use direct table creation
- `pico.rect()` - Use direct table creation
- `pico.dim()` - Use direct table creation
- `pico.set.dim.window()` - Use `pico.set.view_raw()`
- `pico.set.dim.world()` - Use `pico.set.view_raw()`
- `pico.set.scroll()` - Use `pico.set.view_raw()` with world_source
- `pico.set.zoom()` - Use `pico.set.view_pct()` or `pico.set.view_raw()`
- `pico.set.scale()` - Use `pico.set.view_*()` functions
- `pico.set.cursor()` - No replacement
- `pico.output.write()` - Use `draw.text_raw/pct`
- `pico.output.writeln()` - Use `draw.text_raw/pct`

#### New Functions

- `pico.set.alpha(value)` - Set transparency
- `pico.set.view_raw(...)` - Unified view management (raw)
- `pico.set.view_pct(...)` - Unified view management (percentage)
- `pico.get.view()` - Get all view parameters at once
- `pico.get.color.clear()` - Get clear color
- `pico.get.color.draw()` - Get draw color
- `pico.get.crop()` - Get crop rectangle
- `pico.get.flip()` - Get flip state
- `pico.get.font()` - Get current font
- `pico.get.title()` - Get window title

#### New Collision API

```lua
-- OLD (no longer works):
pico.vs.pos_rect(pos, rect)
pico.vs.rect_rect(r1, r2)

-- NEW:
pico.collision.pos_rect_raw(pos, rect)
pico.collision.pos_rect_pct(pos, rect)
pico.collision.rect_rect_raw(r1, r2)
pico.collision.rect_rect_pct(r1, r2)
```

### Migration Guide

See `API_MIGRATION.md` for complete migration instructions and examples.

### Breaking Changes

**This is a major breaking change.** All existing Lua programs using the old API will need to be updated. The changes are necessary to align with the new C API and provide better support for:

- Percentage-based responsive layouts
- Flexible anchor positioning
- Unified view management
- Better separation between pixel and percentage coordinates

### Compatibility

- Lua 5.4
- pico-SDL main branch (latest)

### Files Modified

- `lua/pico.c` - Complete rewrite (~1200 lines)
- `lua/tst/main.lua` - Updated to new API
- `lua/tst/image.lua` - Updated to new API
- `lua/tst/pct.lua` - Updated to new API
- `lua/tst/pixels.lua` - Updated to new API
- `lua/tst/collide.lua` - Updated to new API
- `lua/tst/hello.lua` - NEW: Simple example

### Files Added

- `lua/API_MIGRATION.md` - Migration guide
- `lua/CHANGELOG.md` - This file
- `lua/tst/hello.lua` - Simple hello world example

### TODO / Notes

- Some test files in lua/tst/ still need updating:
  - control.lua, size.lua, anchor.lua, hide.lua, buffer.lua
  - move.lua, blend.lua, layout.lua, shot.lua, font.lua
  - scale.lua, events.lua, dim.lua, mouse.lua, rotate.lua
  - ticks.lua, style.lua, x.lua

- The `up` pointer in `Pico_Rect_Pct` and `Pico_Pos_Pct` is not yet exposed in Lua bindings (set to NULL for now)

- Consider adding compatibility layer for gradual migration in future updates

### Testing

Updated examples have been tested with the new API structure. Run examples:

```bash
cd lua/tst
lua hello.lua
lua main.lua
lua image.lua
lua pct.lua
lua pixels.lua
lua collide.lua
```
