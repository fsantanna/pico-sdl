# Plan: Replace ttl-hash with realm-allocator

## Status: Fix two issues, then compile + tests

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

## Steps

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
- [ ] Fix valgrind.supp: line 574 → 572 (SDL_Init actual line)
- [ ] Revert pico-sdl: `VALGRIND=` → `#VALGRIND=` (re-enable)
- [ ] Re-compile and verify (zero warnings)
- [ ] Run tests (user)
- [ ] Commit / push / PR (user)
