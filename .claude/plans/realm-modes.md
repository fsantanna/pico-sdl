# Plan: Realm Modes — Hide with Sane Defaults

## Context

The realm allocator (`realm.hc`) manages resources (layers, fonts, sounds)
with three modes:
- `'!'` — **exclusive**: asserts if key already exists
- `'='` — **shared**: returns existing value if found, creates only if new
- `'~'` — **replace**: frees old value, allocates new one in place

Currently, all C `pico_layer_*` functions and Lua `pico.layer.*` functions
require an explicit `mode` parameter.
This is low-level detail that most users shouldn't need to think about.

## Goal

Hide realm modes behind sane defaults:

- **C**: Provide two variants for each function:
    - `pico_layer_image(name, path)` — uses default mode
    - `pico_layer_image_mode(mode, name, path)` — explicit mode
- **Lua**: Mode is an optional first argument (1-char string):
    - `pico.layer.image(name, path)` — uses default mode
    - `pico.layer.image('!', name, path)` — explicit mode

## Status

- Awaiting additional context from user to determine sane defaults
- Interview pending
