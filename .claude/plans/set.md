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

## Pending: multi-arg setters -- Issue #102

The `set` dispatch (`init.lua` line 7) calls `field(v)` with a single
argument.
Setters that take multiple positional args lose their extra arguments.

### Affected functions

| Setter   | Individual call                     | `pico.set{...}` today             | Problem                     |
|----------|-------------------------------------|------------------------------------|-----------------------------|
| `expert` | `pico.set.expert(true, 60)`         | `pico.set{ expert=true }`         | `fps` lost — 1 arg passed   |
| `video`  | `pico.set.video('bg', 3)`           | `pico.set{ video='bg' }`          | `frame` lost — 1 arg passed |
| `view`   | `pico.set.view{grid=true, dim=...}` | `pico.set{ view={grid=true,...} }` | OK — already takes a table  |
| `window` | `pico.set.window{title="X", fs=1}`  | `pico.set{ window={title=...} }`  | OK — already takes a table  |

### Alternative A — Named fields (change C)

Change `l_set_expert` and `l_set_video` in `lua/pico.c` to accept a
table with named fields (like `view`/`window` already do), keeping
backward compat with positional args.

```lua
pico.set.expert {on=true, fps=60}
pico.set { expert = {on=true, fps=60} }
```

- Consistent with `view`/`window` pattern
- Requires C changes in `l_set_expert` and `l_set_video`

### Alternative B — Lua wrappers (no C changes)

Wrap the C functions in `init.lua`, converting table to positional args.

```lua
local raw_expert = M.set.expert
M.set.expert = function(v)
    if type(v) == "table" then
        raw_expert(v.on, v.fps)
    else
        raw_expert(v)
    end
end
```

- No C changes
- Named fields only in table form, positional still works
- Wrapper logic lives in `init.lua`

### Alternative C — Convention: array = unpack, hash = recurse

Detect whether a table is a sequence or has string keys.
Arrays get unpacked, hashes pass as-is.

```lua
if type(v) == "table" then
    if #v > 0 then
        field(table.unpack(v))
    else
        field(v)
    end
end
```

- No C changes, no wrappers
- Fragile: `color = {draw='red'}` has no array part so it passes as
  table instead of recursing — breaks the existing dispatch logic
