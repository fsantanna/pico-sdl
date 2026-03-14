# Plan: Realm — resource management

## Context

Replaced TTL-based cache (`hash.h`) with scope-based resource
management (`realm.h`). Three phases:

1. **TTL bugs** — fixed cache expiration in non-blocking loops +
   dangling layer pointer via `_pico_tick()` assertions
2. **TTL → realm** — migrated from `ttl_hash` to `realm_t` with
   per-type free callbacks, plain string keys, scope-based cleanup
3. **Realm modes** — hid realm modes (`!`/`=`/`~`) behind sane
   defaults, exposed `_mode` variants for power users

## Key decisions

| Decision                   | Value                                |
|----------------------------|--------------------------------------|
| TTL dropped                | scope-based only (init/close)        |
| Keys                       | plain strings (no `Pico_Key` struct) |
| `pico_layer_*` default     | `'!'` exclusive                      |
| `_mode` variants           | only `pico_layer_*` (6 functions)    |
| `draw_text` / `get_text`   | explicit key param, use `'~'`        |
| `draw_image` / `get_image` | no API change (keep `'='` internal)  |

## Status

### Phase 1 — TTL bugs (done)

- [x] `_pico_tick()` helper with assertions
- [x] Tick in all input functions including `event_ask`
- [x] `PICO_HASH_TTL` comment

### Phase 2 — TTL → realm

- [x] Core migration (~30 sites)
- [x] `_layer_new` / `_view_new` helpers (mem.hc refactor)
- [ ] Alloc callbacks (8 functions: buffer, empty, image, sub,
      text, video, font, sound)
- [ ] Compile and verify
- [ ] Update valgrind.supp
- [ ] Run tests

### Phase 3 — Realm modes (done, pending tests)

- [x] Split `pico_layer_*` into default + `_mode` (6 functions)
- [x] `draw_text` / `get_text` with explicit key param
- [x] Lua bindings with mode detection
- [x] C and Lua test call sites updated
- [ ] Testing (make tests, Lua tests, valgrind)
