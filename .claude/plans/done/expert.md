# Expert Mode API Redesign

## Context

`pico_set_expert(on, fps)` currently has confusing return
semantics: when `fps==0`, the stored `ms` is left untouched
(stale from previous call) — a bug. Three modes exist but the
return value does not cleanly reflect them.

## Current behavior

`pico_input_event` has three branches on `S.expert.fps`:

| `fps` | input.event timeout  | meaning               |
|-------|----------------------|-----------------------|
| `0`   | `-1` (block forever) | wait-for-event        |
| `-1`  | `0` (immediate)      | as-fast-as-possible   |
| `N>0` | `1000/N` ms          | fixed FPS             |

Lua mapping (`lua/pico.c:l_set_expert`):

| Lua arg               | C fps |
|-----------------------|-------|
| `nil` / `false` / `0` | `0`   |
| `true`                | `-1`  |
| integer `N`           | `N`   |

Bug: `pico_set_expert` skips `ms` update when `fps==0`, so
`pico.set.expert(true, 0)` after `pico.set.expert(true, 40)`
returns stale `ms=25` instead of documented `ms=0`.

## New behavior

Unify `ms` as the SDL timeout value passed to
`pico_input_event_timeout`:

| Lua arg               | `fps` (C) | `ms` (return) | timeout         | meaning             |
|-----------------------|-----------|---------------|-----------------|---------------------|
| `nil` / `false` / `0` | `0`       | `-1`          | `-1` (block)    | wait forever        |
| `true`                | `-1`      | `0`           | `0` (immediate) | as fast as possible |
| `N > 0`               | `N`       | `1000/N`      | `1000/N`        | fixed FPS           |

Benefits:

- `ms` is the single mental model (SDL timeout directly).
- `pico_input_event` collapses its three branches into one —
  pass `S.expert.ms` as timeout (keeping the timing loop for
  `ms > 0`).
- No stale-state bug: `ms` always reflects the new fps.

## Status: complete

## Steps

### 1. Document in `src/pico.h` [x]

- `pico_set_expert` doc (line 525-526) | return now documented
  as SDL timeout (`-1` block, `0` immediate, `N>0` period)

### 2. Fix `src/pico.c` [x]

- `pico_set_expert` (line 596-613) | maps
  `fps=0 → ms=-1`, `fps=-1 → ms=0`, `fps>0 → 1000/fps`;
  always updates `ms` and `t0` (no `if (on)` guard) so
  `pico_input_event` never reads a stale `ms` after disable
- `pico_input_event` (line 1129-1132) | collapsed two
  branches into `ms==0 || ms==-1` that delegates to
  `pico_input_event_timeout(evt, type, S.expert.ms)`; the
  `ms > 0` timing loop below is unchanged

### 3. Update Lua doc [x]

- `lua/doc/api.md` line 102 | return documented as SDL
  timeout (`-1` block, `0` immediate, `N>0` period)

### 4. Update tests [x]

- `tst/expert_fps.c` | only test 2 (fps=0) changed to expect
  `ms == -1` (tests 4 and 6 check getter/NULL, unaffected)
- `lua/tst/expert_fps.lua` | tests 2 (fps=0), 4 (no-arg),
  6 (false) changed to expect `ms == -1`

## Verification

Both suites pass:
```bash
make tests
cd lua/ && make tests
```
