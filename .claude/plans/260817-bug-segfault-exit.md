# Plan: lua-exit-teardown — 26-08-17

Fix segfault/hang on abnormal exit of pico-lua scripts.

# Problem

- Reported: Ctrl-C on `lua/doc/anims.lua` → segfault on teardown
- Reproduced: 4/6 segfaults (real display), rare hangs (headless)

# Diagnosis

- lua5.4 installs its own SIGINT handler around the script
    - Ctrl-C → `interrupted!` error inside `pico.input.event`
- SDL skips its SIGINT→SDL_QUIT handler
    - only installs over `SIG_DFL`, lua got there first
- Error aborts script → `pico.init(false)` never runs
- `exit()` unloads libs while PulseMainloop thread still runs
    - opened by `Mix_OpenAudio`, never closed
    - jumps through freed callback → SIGSEGV (or hang)
- Backtrace: SEGV in thread "PulseMainloop", `pa_pdispatch_run`
- C programs immune: SDL owns SIGINT → clean `quit` event

# Scope

- Not Ctrl-C only — any exit skipping `pico.init(false)`:
    - uncaught Lua error in script
    - `os.exit()` mid-script
    - missing `pico.init(false)` at end
    - SIGTERM/kill
- Impact low (process dying anyway), symptom ugly:
    - segfault + core dump, sometimes hang

# Fix

- Option 3 (chosen): safety net in `lua/pico.c`
    - `atexit` handler → `pico_init(0)` if still init
    - ~5 lines, covers all paths, no semantic change
- Option 1 (won't do, polish): `signal(SIGINT, SIG_DFL)`
  before `pico_init(1)` → Ctrl-C becomes `quit` event
- Option 2 (won't do): custom 2-stage SIGINT handler

# Steps

- [ ] add `atexit` guard in `lua/pico.c` `l_init(true)`
- [ ] verify: SIGINT loop on `anims.lua` → no segfault/hang
- [ ] decide: fold into `v0.7` branch or post-release
