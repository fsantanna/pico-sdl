# Plan: bug-timeout

## Description

Bug fix for `pico_input_event_timeout`: the `type` filter parameter
is not working correctly.

## Analysis

### Root cause (C — `src/pico.c:1433`)

`pico_input_event_timeout` calls `SDL_WaitEventTimeout` only **once**.
If a non-matching event arrives before the timeout expires, the function
returns 0 immediately — as if the timeout expired — instead of continuing
to wait for a matching event.

Compare with:
- `pico_input_event` (line 1413): loops until a matching event arrives
- `pico_input_delay` (line 1396): loops, subtracting elapsed time

The fix should loop like `pico_input_delay`, consuming non-matching
events and subtracting elapsed time from the remaining timeout.

### Secondary bug (Lua — `lua/pico.c:1121`)

```c
ise = pico_input_event_timeout(&e, PICO_EVENT_ANY, ms);
```

The Lua binding hardcodes `PICO_EVENT_ANY` instead of passing `evt`
(the parsed filter type). So even after fixing the C function, Lua
callers with a filter + timeout would still get unfiltered results.

## Steps

- [x] Analyze the bug
- [x] Create test `tst/todo_input_timeout.c`
- [x] Fix `pico_input_event_timeout` in `src/pico.c` (user did this)
- [x] Create test `lua/tst/todo/input_timeout.lua`
- [x] Fix Lua binding in `lua/pico.c`
    - line 1121: `PICO_EVENT_ANY` → `evt`
    - line 1596: add `mouse.motion` → `PICO_EVENT_MOUSE_MOTION`
    - line 1596: add `mouse.button.up` → `PICO_EVENT_MOUSE_BUTTON_UP`
- [ ] Manual testing
- [ ] Commit / push / PR
