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

## Steps

### 1. Document in `src/pico.h`

- `pico_set_expert` doc comment | describe three fps inputs
  and `ms` return semantics

### 2. Fix `src/pico.c`

- `pico_set_expert` (line 635-646) | map
  `fps=0 → ms=-1`, `fps=-1 → ms=0`, `fps>0 → 1000/fps`;
  always update `ms` when `on`
- `pico_input_event` (line 1108-1114) | collapse
  `fps==0`/`fps==-1` branches; pass `S.expert.ms` as
  timeout; keep the `ms > 0` timing loop

### 3. Update Lua doc

- `lua/doc/api.md` (`pico.set.expert` entry) | clarify that
  the return is an SDL-timeout value (`-1` = block forever,
  `0` = immediate, `N` = wait up to N ms)

### 4. Update tests

- `tst/expert_fps.c` | tests 2, 4, 6 | `ms == -1` for
  fps=0 / no-arg / false
- `lua/tst/expert_fps.lua` | tests 2, 4, 6 | same

## Verification

- `make tests`
- `cd lua/ && make tests`
