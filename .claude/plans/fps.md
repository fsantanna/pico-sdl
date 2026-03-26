# Plan: Expert FPS Modes + Delta Time

## Prereqs

- [x] rename `pico_get_ticks` → `pico_get_now` (C/Lua/docs/tests)

## FPS Modes

Change `pico_set_expert` FPS parameter to support three modes:

| C value | Lua value       | Meaning                |
|---------|-----------------|------------------------|
| `0`     | `nil` / `false` | disable (wait forever) |
| `-1`    | `true`          | as fast as possible    |
| `N>0`   | `N>0`           | fixed FPS              |

## Delta Time (decided: C5a)

All input functions return `int dt` (ms elapsed).
Event info is obtained from the out-parameter / second return value.

### C side

- `int pico_input_event(Pico_Event* e, int filter)` → returns dt
- `int pico_input_delay(int ms)` → returns dt
- Timeout / no event: `e->type == PICO_EVENT_NONE`

### Lua side

- `local evt, dt = pico.input.event([filter], [timeout])`
    - evt is nil on timeout
- `local dt = pico.input.delay(ms)`

## Tasks

### FPS modes
- [x] `src/pico.c` | `pico_set_expert` | accept fps=-1, set ms=0
- [x] `src/pico.c` | `pico_input_event` | fps==-1 → timeout=0
- [x] `src/pico.h` | doxygen | update @param fps docs
- [x] `lua/pico.c` | `l_set_expert` | accept nil/false/true/number
      for fps arg
- [x] `lua/doc/api.md` | update pico.set.expert signature

### Delta time
- [x] `src/pico.c` | `pico_input_event_timeout` | return `int` dt
- [x] `src/pico.c` | `pico_input_event` | return `int` dt
- [x] `src/pico.c` | `pico_input_delay` | return `int` dt
- [x] `src/pico.h` | update declarations for new return types
- [x] `lua/pico.c` | `l_input_event` | return `evt, dt` (nil+dt
      on timeout)
- [x] `lua/pico.c` | `l_input_delay` | return dt

### Docs & tests
- [x] `lua/doc/api.md` | update input signatures and docs
- [x] `lua/doc/guide.md` | update Section 7 and 9 examples
- [x] Update callers of old return type across `tst/`, `doc/exs/`
- [x] Update tests: `tst/expert.c`, `tst/expert_fps.c`
- [x] Update tests: `lua/tst/expert.lua`, `lua/tst/expert_fps.lua`
