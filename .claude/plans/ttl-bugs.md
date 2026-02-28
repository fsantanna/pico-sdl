# Plan: TTL Bugs

## Context

The TTL-based resource cache (`src/hash.h` + `src/pico.c`) has two bugs:

1. **Cache never expires in non-blocking loops** —
   `pico_input_event_ask()` never calls `ttl_hash_tick()`, so programs
   using non-blocking game loops leak cache entries forever.

2. **Dangling `S.layer` pointer** — `pico_set_layer()` stores a raw
   `Pico_Layer*` from the hash into `S.layer`. A subsequent
   `ttl_hash_tick()` can evict and free that layer, causing
   use-after-free on the next draw operation. Same risk with layers
   saved in `STACK` via `pico_push()`.

## Approach

Enforce a **design contract** via assertions: before any
`ttl_hash_tick()`, the program must not hold references to cached
layers. This prevents dangling pointers by construction.

## Changes

### 1. Add `_pico_tick()` helper — `src/pico.c:117`

```c
static void _pico_tick (void) {
    assert(S.layer == &G.main
        && "must reset layer before input");
    assert(STACK.n == 0
        && "must clear stack before input");
    ttl_hash_tick(G.hash);
}
```

### 2. Replace `ttl_hash_tick(G.hash)` → `_pico_tick()`

4 existing call sites in input functions.

### 3. Add `_pico_tick()` to `pico_input_event_ask()`

Was missing tick entirely — the non-blocking loop bug.

### 4. Comment on `PICO_HASH_TTL` — `src/pico.h:30`

```c
// tick-based: entries expire after N input calls
#define PICO_HASH_TTL  1000
```

### 5. Update `valgrind.supp`

`SDL_Init` shifted from line 544 → 552.

## Files Modified

- `src/pico.c:117-123` — insert `_pico_tick()`
- `src/pico.c:1405` — `pico_input_delay()`
- `src/pico.c:1422` — `pico_input_event()`
- `src/pico.c:1436` — `pico_input_event_ask()`
- `src/pico.c:1443` — `pico_input_event_timeout()`
- `src/pico.c:1461` — `pico_input_loop()`
- `src/pico.h:30` — comment on `PICO_HASH_TTL`
- `valgrind.supp:97` — line 544 → 552

## Progress — Phase 1 (tick bugs)

- [x] Add `_pico_tick()` helper
- [x] Replace 4 existing `ttl_hash_tick` calls
- [x] Add tick to `pico_input_event_ask()`
- [x] Add comment to `PICO_HASH_TTL`
- [x] Update `valgrind.supp`

## Progress — Phase 2 (void pico_layer_*)

- [x] Update `src/pico.h` declarations
- [x] Update `src/pico.c` definitions + draw_buffer
- [x] Update `src/video.h` (NULL fallback + void)
- [x] Update `lua/pico.c` bindings
- [x] Update `tst/layers.c` tests
- [x] Update `valgrind.supp`
- [ ] Run `make tests`
- [ ] Run valgrind check
