# Plan: Replace ttl-hash with realm-allocator

## Status: Use realm alloc callbacks (pass mode as-is)

## Design Summary

Replace TTL-based resource cache (`hash.h` / `ttl_hash`) with
scope-based resource management (`realm.h` / `realm_t`).

### What changed

| Before (ttl-hash) | After (realm-allocator) |
|-|-|
| `ttl_hash` with `Pico_Key` struct | `realm_t` with plain string keys |
| Single `_pico_hash_clean` callback | Per-type: `_free_layer`, `_free_sound`, `_free_font` |
| TTL eviction + `_tick_and_check` | Scope-based: `realm_enter`/`realm_close` |
| `Pico_Layer.key` (owned by hash) | `Pico_Layer.name` via `strdup` |

### Key decisions

- **TTL dropped** — scope-based cleanup only (enter at `pico_init(1)`,
  close at `pico_init(0)`)
- **No `Pico_Key`** — plain strings; fonts prefixed `/font/<path>/<h>`
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
- [x] Add mode parameter to pico_layer_* APIs
- [x] Fix valgrind.supp line + re-enable valgrind

## Phase: Use realm alloc callbacks

### Problem

Current code manually emulates `'='` mode:
1. Check `realm_get` — if exists, return early
2. Create resource inline
3. Call `realm_put` with `(mode=='=') ? '!' : mode`

This is wrong. Mode should pass through to `realm_put` as-is.
Realm natively handles `'='` via the `alloc` callback:
- Key exists → return existing value (no alloc)
- Key missing → call `alloc(n, key, ctx)` to create

### Approach

- Create per-type `alloc` callbacks
- Move resource creation logic into each `alloc`
- Pass `mode` directly to `realm_put` (no ternary)
- Remove manual `realm_get` + early-return blocks
- `realm.h` stays unchanged

### Alloc callbacks needed

| Callback | File | Used by | ctx type |
|-|-|-|-|
| `_alloc_layer_buffer` | `pico.c` | `_pico_layer_buffer` | `_Ctx_Buffer*` `{dim, pixels}` |
| `_alloc_layer_empty` | `pico.c` | `pico_layer_empty` | `Pico_Abs_Dim*` |
| `_alloc_layer_image` | `pico.c` | `_pico_layer_image` | `const char*` (path) |
| `_alloc_layer_sub` | `pico.c` | `pico_layer_sub` | `_Ctx_Sub*` `{par, crop}` |
| `_alloc_layer_text` | `pico.c` | `_pico_layer_text` | `_Ctx_Text*` `{height, text}` |
| `_alloc_layer_video` | `video.h` | `_pico_layer_video` | `const char*` (path) |
| `_alloc_font` | `pico.c` | `_font_get` | `_Ctx_Font*` `{path, h}` |
| `_alloc_sound` | `pico.c` | `_pico_output_sound_cache` | `const char*` (path) |

### Context structs

```c
typedef struct { Pico_Abs_Dim dim; const Pico_Color_A* px; } _Ctx_Buffer;
typedef struct { Pico_Layer* par; Pico_Rel_Rect crop; } _Ctx_Sub;
typedef struct { int height; const char* text; } _Ctx_Text;
typedef struct { const char* path; int h; } _Ctx_Font;
```

Simple types (image, video, sound) just cast `const char*` path
as `ctx`.
Empty uses `Pico_Abs_Dim*` directly (no struct needed).
Text reads `S.font` and `S.color.draw` from globals (safe —
alloc runs synchronously).

### Per-function changes

#### 1. `_pico_layer_buffer` (pico.c:1021)
- Add `_Ctx_Buffer` struct
- Add `_alloc_layer_buffer(n, key, ctx)`:
  create surface from pixels, texture, malloc Pico_Layer, set blend
- Simplify function body:
  `return realm_put(G.realm, mode, n, name, _free_layer, _alloc_layer_buffer, &ctx);`

#### 2. `pico_layer_empty` (pico.c:1080)
- Add `_alloc_layer_empty(n, key, ctx)`:
  `_tex_create(dim)`, malloc Pico_Layer, set blend
- Body: `realm_put(G.realm, mode, n, name, _free_layer, _alloc_layer_empty, &dim);`

#### 3. `_pico_layer_image` (pico.c:1116)
- Add `_alloc_layer_image(n, key, ctx)`:
  `IMG_LoadTexture`, query dim, malloc Pico_Layer, set blend
- Body: `return realm_put(G.realm, mode, n, str, _free_layer, _alloc_layer_image, (void*)path);`

#### 4. `pico_layer_sub` (pico.c:1165)
- Add `_Ctx_Sub` struct
- Add `_alloc_layer_sub(n, key, ctx)`:
  malloc Pico_Layer, set type=SUB, copy crop, compute abs,
  set `view.src.up = &par->view.src`
- Body: parent lookup stays outside, then
  `realm_put(G.realm, mode, n, name, _free_layer, _alloc_layer_sub, &ctx);`

#### 5. `_pico_layer_text` (pico.c:1220)
- Add `_Ctx_Text` struct
- Add `_alloc_layer_text(n, key, ctx)`:
  `_tex_text(height, text, &dim)`, malloc Pico_Layer, set blend
- Key generation (`/text/...` string) stays outside
- Body: `return realm_put(G.realm, mode, n, str, _free_layer, _alloc_layer_text, &ctx);`

#### 6. `_pico_layer_video` (video.h:119)
- Add `_alloc_layer_video(n, key, ctx)`:
  fopen, parse Y4M, create YUV texture, calloc Pico_Layer_Video,
  set blend, alloc planes
- Body: `return realm_put(G.realm, mode, n, key, _free_layer, _alloc_layer_video, (void*)path);`

#### 7. `_font_get` (pico.c:143)
- Add `_Ctx_Font` struct
- Add `_alloc_font(n, key, ctx)`:
  `_font_open(path, h)`
- Body: `return realm_put(G.realm, '=', n, key, _free_font, _alloc_font, &ctx);`

#### 8. `_pico_output_sound_cache` (pico.c:1873)
- Add `_alloc_sound(n, key, ctx)`:
  `Mix_LoadWAV(path)`
- Cached path:
  `mix = realm_put(G.realm, '=', n, path, _free_sound, _alloc_sound, (void*)path);`

### Steps

- [ ] Step 1: Add context structs + alloc callbacks (pico.c)
- [ ] Step 2: Refactor 5 layer functions in pico.c
      (buffer, empty, image, sub, text)
- [ ] Step 3: Refactor _font_get + _pico_output_sound_cache
- [ ] Step 4: Refactor _pico_layer_video in video.h
- [ ] Step 5: Compile and verify (zero warnings)
- [ ] Step 6: Update valgrind.supp if SDL_Init line changed
- [ ] Step 7: Run tests (user)
- [ ] Step 8: Commit / push / PR (user)
