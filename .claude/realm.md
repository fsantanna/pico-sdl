# realm-allocator

## 1. Name

**`realm-allocator`** — a resource allocator that groups named resources by
realm, enabling bulk-free of a group when its lifetime ends.

It is a *resource allocator*, not a raw memory allocator: you allocate named
resources (SDL textures, sounds, fonts) and free them by realm — the hash is
just the lookup mechanism.

This is region/arena-style lifetime management applied to keyed resources
rather than raw bytes.

## 2. Fork Strategy

`realm-allocator` is a fork of the current `src/hash.h` (`ttl-hash`).

**Why fork instead of modify in place:** the fork gives a clean, reviewable
diff between the old design (TTL-only) and the new design (realms). A simple
`diff hash.h realm.h` shows exactly what changed.

Steps:
1. Copy `src/hash.h` → `src/realm.h`
2. Rename all symbols (see section 3)
3. Add realm support (see section 4)
4. Update `src/pico.c` and `src/pico.h` to use `realm.h`
5. Delete `src/hash.h`

## 3. Symbol Renames (pure mechanical diff)

| Old (`ttl_hash`)       | New (`realm`)          |
|------------------------|------------------------|
| `TTL_HASH_H`           | `REALM_H`              |
| `TTL_HASH_C`           | `REALM_C`              |
| `cb_clean_t`           | `Realm_Clean`          |
| `ttl_hash_entry`       | `Realm_Entry`          |
| `ttl_hash`             | `Realm`                |
| `ttl_hash_open`        | `realm_open`           |
| `ttl_hash_close`       | `realm_close`          |
| `ttl_hash_put`         | `realm_put`            |
| `ttl_hash_get`         | `realm_get`            |
| `ttl_hash_rem`         | `realm_rem`            |
| `ttl_hash_tick`        | `realm_tick`           |
| `ttl_hash_djb2`        | `realm_djb2` (static)  |
| `ttl_hash_find`        | `realm_find` (static)  |
| `ttl_hash_remove_entry`| `realm_remove` (static)|
| field `n_buk`          | `n_buk` (unchanged)    |
| field `n_ttl`          | `n_ttl` (unchanged)    |
| field `clean`          | `clean` (unchanged)    |
| field `buckets`        | `buckets` (unchanged)  |
| field `n` (key size)   | `n` (unchanged)        |
| field `key`            | `key` (unchanged)      |
| field `value`          | `value` (unchanged)    |
| field `ttl`            | `ttl` (unchanged)      |
| field `next`           | `next` (unchanged)     |

In `src/pico.c`, the internal helpers also rename:

| Old                       | New                        |
|---------------------------|----------------------------|
| `_ttl_hash_get_layer`     | `_realm_get_layer`         |
| `_ttl_hash_put_layer`     | `_realm_put_layer`         |
| `_pico_hash_clean`        | `_pico_realm_clean`        |
| `G.hash`                  | `G.realm`                  |

In `src/pico.h`:

| Old                | New                |
|--------------------|--------------------|
| `PICO_HASH_BUK`    | `PICO_REALM_BUK`   |
| `PICO_HASH_TTL`    | `PICO_REALM_TTL`   |

## 4. New Feature: Realm Tags and `realm_leave`

### Concept

Each entry optionally carries a **realm tag** — a short string identifying
which group it belongs to. Entries in the same realm are freed together by
calling `realm_leave`.

Two eviction mechanisms, chosen per entry:

| Mechanism | Condition              | Use when                              |
|-----------|------------------------|---------------------------------------|
| TTL       | `realm == NULL`        | ad-hoc cache, automatic expiry        |
| Realm     | `realm != NULL`        | explicit lifetime, freed on demand    |

TTL countdown still runs on `realm_tick` for TTL-only entries.
Realm-tagged entries are immune to TTL eviction; they live until
`realm_leave` is called (or `realm_close`).

### Struct Change

Add one field to `Realm_Entry`:

```c
typedef struct Realm_Entry {
    int    n;
    void*  key;
    void*  value;
    int    ttl;
    char*  realm;          // NULL = TTL-only; non-NULL = realm-managed
    struct Realm_Entry* next;
} Realm_Entry;
```

### API Addition

```c
// Free all entries whose realm tag equals `tag`. Calls the clean callback
// for each. Safe to call with tag==NULL (no-op).
void realm_leave (Realm* r, const char* tag);
```

### `realm_put` signature change

Add `realm` parameter (NULL = TTL-only, preserving current behavior):

```c
// Old:
const void* ttl_hash_put (ttl_hash* ht, int n, const void* key, void* value);

// New:
const void* realm_put (Realm* r, int n, const void* key, void* value,
                       const char* realm);
```

`realm_put` allocates and copies the realm string (like it does for the key).
On update (key already exists), the realm tag is also replaced.

### `realm_tick` change

Skip TTL countdown for realm-tagged entries:

```c
// Old behavior: decrement ttl for every entry
// New behavior: skip entries where realm != NULL
if (e->realm == NULL) {
    e->ttl--;
    if (e->ttl <= 0) { realm_remove(...); }
}
```

### `realm_leave` implementation (sketch)

```c
void realm_leave (Realm* r, const char* tag) {
    if (tag == NULL) return;
    for (int i = 0; i < r->n_buk; i++) {
        Realm_Entry** pp = &r->buckets[i];
        while (*pp != NULL) {
            Realm_Entry* e = *pp;
            if (e->realm != NULL && strcmp(e->realm, tag) == 0) {
                realm_remove(r, pp);   // calls clean, frees key+entry
            } else {
                pp = &e->next;
            }
        }
    }
}
```

`realm_remove` must also `free(e->realm)` (added alongside the existing
`free(e->key)` and `free(e)`).

## 5. pico-sdl Migration

### Current call sites in `src/pico.c`

All `ttl_hash_put` calls pass `NULL` as the new realm argument (backwards
compatible, TTL-only behavior unchanged):

```c
// fonts
realm_put(G.realm, n, key, ttf, NULL);

// sounds
realm_put(G.realm, n, res, mix, NULL);

// layers (via _realm_put_layer)
realm_put(G.realm, n, key, data, NULL);
```

This keeps behavior identical to the current implementation. Realm tags can
be introduced later per-feature without further structural changes.

### `src/pico.h` constants

```c
#define PICO_REALM_BUK  128
#define PICO_REALM_TTL  1000     // expire after N pico_input_* calls
```

### Initialization / teardown (no logic change)

```c
// pico_init(1)
G.realm = realm_open(PICO_REALM_BUK, PICO_REALM_TTL, _pico_realm_clean);

// pico_init(0)
realm_close(G.realm);

// pico_input_* tick
realm_tick(G.realm);
```

### `valgrind.supp`

After renaming, `src/pico.c` line numbers shift. The `sdl-init` suppression
entry (`src:pico.c:N`) must be updated to the new line of the `SDL_Init`
call in `pico_init`, per the CLAUDE.md instruction.

## 6. Files Changed

| File              | Change                                              |
|-------------------|-----------------------------------------------------|
| `src/hash.h`      | **Deleted** (replaced by `src/realm.h`)             |
| `src/realm.h`     | **New** — fork of hash.h with renames + realm       |
| `src/pico.h`      | Rename `PICO_HASH_*` → `PICO_REALM_*`              |
| `src/pico.c`      | Rename all call sites; update internal helpers      |
| `valgrind.supp`   | Update `src:pico.c:N` line number                  |

## 7. Out of Scope (future)

- Assigning explicit realm tags to fonts, sounds, or layers in pico-sdl
- Exposing `realm_leave` to the Lua API
- Nested / hierarchical realms
- Multiple `Realm` instances (one per resource category)
