# Plan: `pico.set` -- Issue #70

- Issue: https://github.com/fsantanna/pico-sdl/issues/70

## Goal

Make `pico.set` callable as an all-at-once setter while keeping
the existing `pico.set.<field>()` API intact.

## Current State

- `pico.set` is a Lua table containing individual setter
  functions:
    - `pico.set.alpha(a)`
    - `pico.set.color.clear(clr)`, `pico.set.color.draw(clr)`
    - `pico.set.style(s)`
    - `pico.set.crop(r)`
    - `pico.set.font(path)`
    - `pico.set.layer(name)`
    - `pico.set.expert(on)`
- State lives in the `S` struct in `src/pico.c`

## Proposed Lua API

### Set all (make `pico.set` callable)

```lua
pico.set {
    alpha = 0xCC,
    color = { clear="white", draw="black" },
    style = "fill",
}
```

Existing per-field calls remain unchanged.

## Implementation

### Step 1 -- Lua: `pico.set` all (`lua/pico/init.lua`)

- Generic recursive implementation in Lua:
    - `apply_set(target, values)` traverses the input table
    - for each key, looks up the corresponding field in `pico.set`
    - if field is a function, calls it with the value
    - if field is a table, recurses into it
- Attach a metatable with `__call = apply_set` to `M.set`
- Works with any setter without hardcoding field names

### Step 2 -- Tests

- Add test for `pico.set` all (alpha, color, style)
- Verify individual setters still work

## Status

- [x] Step 1 -- Lua `pico.set` all
- [x] Step 2 -- Tests
