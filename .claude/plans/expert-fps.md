# Expert FPS Mode

## Context

Expert mode currently only toggles buffered rendering.
This plan adds FPS-based frame timing: `pico_set_expert(on, fps)`
returns the frame period in ms, and `pico_input_event()` (no timeout)
auto-uses that period with drift compensation across calls.

## Progress

- [x] C: struct S + init
- [x] C: pico_set_expert signature + implementation
- [x] C: pico_get_expert signature + implementation
- [x] C: pico_input_event fps logic
- [x] C: pico.h declarations + doc
- [x] Lua: l_set_expert
- [x] Lua: l_input_event routing
- [x] Update all existing callers
- [x] C test: tst/expert_fps.c
- [x] Lua test: lua/tst/expert_fps.lua
- [x] valgrind.supp line update
- [ ] Run tests
