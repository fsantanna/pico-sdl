# Plan: `pico.set` -- Issue #70

- Issue: https://github.com/fsantanna/pico-sdl/issues/70

## Goal

Make `pico.set` callable as an all-at-once setter while keeping
the existing `pico.set.<field>()` API intact.
Add `pico.push`/`pico.pop` for save/restore state semantics.

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

### Push / Pop

```lua
pico.push {
    alpha = 0x80,
    color = { draw="red" },
}
-- draw operations with overridden state
pico.pop()
```

`pico.push` saves the current state onto a stack, then applies
the fields. `pico.pop` restores the previous state.

## Implementation

### Step 1 -- C: push/pop (`src/pico.c`, `src/pico.h`)

- Define a fixed-size stack of `S` snapshots (e.g. 8 deep)
- `void pico_push(void)` -- copy current `S` onto stack
- `void pico_pop(void)`  -- restore `S` from stack top
- Declare both in `pico.h`

### Step 2 -- Lua: `pico.set` all (`lua/pico.c`)

- Implement `l_set_all(L)`:
    - parse table argument
    - for each recognized field, call the corresponding C setter
    - supported fields: `alpha`, `color` (`clear`/`draw`),
      `style`, `crop`, `font`
- Attach a metatable with `__call = l_set_all` to the
  `pico.set` table so both syntaxes work

### Step 3 -- Lua: push/pop (`lua/pico.c`)

- `l_push(L)` -- call `pico_push()`, then delegate to
  `l_set_all(L)` for the table argument
- `l_pop(L)` -- call `pico_pop()`
- Register `pico.push` and `pico.pop` in `luaopen_pico_native`

### Step 4 -- Tests

- Add test for `pico.set` all
- Add test for `pico.push`/`pico.pop` round-trip

## Status

- [x] Step 1 -- C push/pop
- [x] Step 2 -- Lua `pico.set` all
- [x] Step 3 -- Lua push/pop
- [x] Step 4 -- Tests
