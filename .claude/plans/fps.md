# Plan: Expert FPS Modes + Delta Time

## FPS Modes

Change `pico_set_expert` FPS parameter to support three modes:

| C value | Lua value       | Meaning               |
|---------|-----------------|------------------------|
| `0`     | `nil` / `false` | disable (wait forever) |
| `-1`    | `true`          | as fast as possible    |
| `N>0`   | `N>0`           | fixed FPS              |

## Delta Time

Add `pico_get_dt()` to get ms elapsed since last `pico_input_event`
call.

**Open question**: return `dt` from `pico_input_event` instead?
- Option C2: positive = event occurred, negative = no event
  (timeout), `abs(dt)` = elapsed ms
- Option C3: keep current return (`PICO_EVENT`), add separate
  `pico_get_dt()` / `pico.get.dt()`
- Decision pending

## Tasks

- [ ] `src/pico.c` | `pico_set_expert` | accept fps=-1, set ms=0
- [ ] `src/pico.c` | `pico_input_event` | fps==-1 → timeout=0
- [ ] `src/pico.h` | doxygen | update @param fps docs
- [ ] `src/pico.c` | add dt tracking (store ticks at each
      `pico_input_event` exit)
- [ ] `src/pico.h` | declare `pico_get_dt` (or change return type
      of `pico_input_event` — pending decision)
- [ ] `lua/pico.c` | `l_set_expert` | accept nil/false/true/number
      for fps arg
- [ ] `lua/pico.c` | expose `pico.get.dt()` (or adapt event return)
- [ ] `lua/doc/api.md` | update signature and docs
- [ ] `lua/doc/guide.md` | update Section 9 examples
- [ ] Verify existing tests: `tst/expert.c`, `tst/expert_fps.c`,
      `lua/tst/expert.lua`, `lua/tst/expert_fps.lua`
