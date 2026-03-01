# Plan: Replace ttl-hash with realm-allocator

## Status: Implementing mode parameter for pico_layer_* APIs

## Design Summary

Replace TTL-based resource cache (`hash.h` / `ttl_hash`) with
scope-based resource management (`realm.h` / `realm_t`).

### What changed

| Before (ttl-hash) | After (realm-allocator) |
|-|-|
| `ttl_hash` with `Pico_Key` struct | `realm_t` with plain string keys |
| Single `_pico_hash_clean` callback | Per-type: `_pico_free_layer`, `_pico_free_sound`, `_pico_free_font` |
| TTL eviction + `_tick_and_check` | Scope-based: `realm_enter`/`realm_close` |
| `Pico_Layer.key` (owned by hash) | `Pico_Layer.name` via `strdup` |

### Key decisions

- **TTL dropped** — scope-based cleanup only (enter at `pico_init(1)`,
  close at `pico_init(0)`)
- **No `Pico_Key`** — plain strings; fonts prefixed `/font/<path>/<h>`
- **Insertion mode** — `'!'` exclusive for all `realm_put` calls
- **`_tick_and_check` removed** — deleted function + 5 call sites in
  input functions

### Files modified

| File | Change |
|-|-|
| `src/realm.h` | **NEW** — copied from realm-allocator |
| `src/hash.h` | **DELETED** |
| `src/pico.c` | Core migration (~30 sites): includes, types, G struct, callbacks, helpers, init/close, layers, fonts, sounds, tick removal |
| `src/video.h` | 3 hash calls → realm calls |
| `src/pico.h` | Removed `PICO_HASH_TTL` |
| `Makefile` | Removed `ttl` target |
| `doc/Doxyfile` | `hash.h` → `realm.h` in EXCLUDE |
| `valgrind.supp` | SDL_Init line number update |

## Completed Steps

- [x] Read and understand all affected files
- [x] Copy realm.h into src/
- [x] Update src/pico.c (include, types, G, callbacks, helpers,
      init, layers, fonts, sounds, tick removal)
- [x] Update src/video.h
- [x] Update src/pico.h
- [x] Delete src/hash.h
- [x] Update Makefile
- [x] Update doc/Doxyfile
- [x] Update valgrind.supp
- [x] Compile and verify (zero warnings)
- [x] Remove Pico_Key (use plain string keys)

## Done: Add mode parameter to pico_layer_* APIs

Realm modes: `'!'` exclusive, `'='` shared, `'~'` replaceable.
Add `int mode` as first parameter to all `pico_layer_*` functions.

- [x] Step 1: Update declarations in `src/pico.h` (6 functions)
- [x] Step 2a: Forward declarations in `src/pico.c` (2 statics)
- [x] Step 2b: Internal helpers + public wrappers in `src/pico.c`
- [x] Step 2c: Internal callers of `_pico_layer_image` → pass `'='`
- [x] Step 2d: Internal callers of `_pico_layer_text` → pass `'='`
- [x] Step 2e: `pico_output_draw_buffer` → pass `'='`
- [x] Step 3: Update `src/video.h` (3a/3b/3c)
- [x] Step 4: Update `lua/pico.c` (6 Lua bindings)
- [x] Step 5: valgrind.supp — no change (SDL_Init still line 548)

## Remaining

- [ ] Fix valgrind.supp: line 574 → 572 (SDL_Init actual line)
- [ ] Revert pico-sdl: `VALGRIND=` → `#VALGRIND=` (re-enable)
- [ ] Re-compile and verify (zero warnings)
- [ ] Run tests (user)
- [ ] Commit / push / PR (user)
