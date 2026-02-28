# realm-allocator

## 1. Name

**`realm-allocator`** — a resource allocator that groups named resources by
realm, enabling bulk-free of a group when its lifetime ends.

It is a *resource allocator*, not a raw memory allocator: you allocate named
resources (SDL textures, sounds, fonts) and free them by realm — the hash is
just the lookup mechanism on top.

This is region/arena-style lifetime management applied to keyed resources
rather than raw bytes. The defining operation is `realm_leave(tag)`, which
bulk-frees everything tagged with that realm string.

---

## 2. Fork Strategy

`realm-allocator` is a **fork** of `src/hash.h` (the current `ttl-hash`
single-header library).

**Why fork instead of modify in place:** a fork gives a clean diff between the
original design (TTL-only) and the new design (realms + TTL). After the work
is done, `diff hash.h realm.h` shows exactly what changed conceptually.

**Order of operations:**

1. Copy `src/hash.h` → `src/realm.h` verbatim
2. Rename all symbols in `realm.h` (section 3 — mechanical, no logic change)
3. Add realm support to `realm.h` (section 4 — logic change)
4. Update `src/pico.c` to use `realm.h` (section 5)
5. Update `src/video.h` to use `realm.h` (section 6)
6. Update `src/pico.h` constants (section 7)
7. Update `valgrind.supp` line number (section 8)
8. Delete `src/hash.h`

---

## 3. Symbol Renames in `realm.h` (mechanical, step 2)

Do these as pure text substitutions. No logic changes in this step.

### Header guard and implementation gate

```
TTL_HASH_H  →  REALM_H
TTL_HASH_C  →  REALM_C
```

### Types

| Old                 | New           |
|---------------------|---------------|
| `cb_clean_t`        | `Realm_Clean` |
| `ttl_hash_entry`    | `Realm_Entry` |
| `ttl_hash`          | `Realm`       |

### Public functions

| Old              | New          |
|------------------|--------------|
| `ttl_hash_open`  | `realm_open` |
| `ttl_hash_close` | `realm_close`|
| `ttl_hash_put`   | `realm_put`  |
| `ttl_hash_get`   | `realm_get`  |
| `ttl_hash_rem`   | `realm_rem`  |
| `ttl_hash_tick`  | `realm_tick` |

### Internal (static) functions

| Old                    | New            |
|------------------------|----------------|
| `ttl_hash_djb2`        | `realm_djb2`   |
| `ttl_hash_find`        | `realm_find`   |
| `ttl_hash_remove_entry`| `realm_remove` |

### Struct field names — **unchanged**

`n_buk`, `n_ttl`, `clean`, `buckets`, `n`, `key`, `value`, `ttl`, `next`
are all kept as-is to minimize noise in the diff.

---

## 4. Realm Support Added to `realm.h` (logic change, step 3)

After the mechanical rename, make the following logic changes.

### 4.1 New field in `Realm_Entry`

Add `char* realm` as the last field before `next`:

```c
// Before (after rename):
typedef struct Realm_Entry {
    int    n;
    void*  key;
    void*  value;
    int    ttl;
    struct Realm_Entry* next;
} Realm_Entry;

// After:
typedef struct Realm_Entry {
    int    n;
    void*  key;
    void*  value;
    int    ttl;
    char*  realm;          // NULL = TTL-only; non-NULL = realm-managed
    struct Realm_Entry* next;
} Realm_Entry;
```

### 4.2 New public function declaration

Add to the header section (alongside the other declarations):

```c
void realm_leave (Realm* r, const char* tag);
```

### 4.3 `realm_remove` — free the realm string

```c
// Before:
static void realm_remove (Realm* r, Realm_Entry** pp) {
    Realm_Entry* e = *pp;
    *pp = e->next;
    if (r->clean != NULL) {
        r->clean(e->n, e->key, e->value);
    }
    free(e->key);
    free(e);
}

// After:
static void realm_remove (Realm* r, Realm_Entry** pp) {
    Realm_Entry* e = *pp;
    *pp = e->next;
    if (r->clean != NULL) {
        r->clean(e->n, e->key, e->value);
    }
    free(e->key);
    free(e->realm);   // new
    free(e);
}
```

### 4.4 `realm_put` — add `realm` parameter, copy the string

```c
// Before:
const void* realm_put (Realm* r, int n, const void* key, void* value) {
    Realm_Entry** pp = realm_find(r, n, key);

    /* Key exists: replace value */
    if (*pp != NULL) {
        Realm_Entry* e = *pp;
        if (r->clean != NULL) {
            r->clean(e->n, e->key, e->value);
        }
        e->value = value;
        e->ttl = r->n_ttl;
        return e->key;
    }

    /* Key does not exist: create new entry */
    Realm_Entry* e = (Realm_Entry*)malloc(sizeof(Realm_Entry));
    if (e == NULL) {
        return NULL;
    }
    e->key = malloc(n);
    if (e->key == NULL) {
        free(e);
        return NULL;
    }
    memcpy(e->key, key, n);
    e->n = n;
    e->value = value;
    e->ttl = r->n_ttl;
    e->next = NULL;
    *pp = e;
    return e->key;
}

// After:
const void* realm_put (Realm* r, int n, const void* key, void* value,
                       const char* realm) {
    Realm_Entry** pp = realm_find(r, n, key);

    /* Key exists: replace value and realm tag */
    if (*pp != NULL) {
        Realm_Entry* e = *pp;
        if (r->clean != NULL) {
            r->clean(e->n, e->key, e->value);
        }
        e->value = value;
        e->ttl = r->n_ttl;
        free(e->realm);
        e->realm = realm ? strdup(realm) : NULL;
        return e->key;
    }

    /* Key does not exist: create new entry */
    Realm_Entry* e = (Realm_Entry*)malloc(sizeof(Realm_Entry));
    if (e == NULL) {
        return NULL;
    }
    e->key = malloc(n);
    if (e->key == NULL) {
        free(e);
        return NULL;
    }
    memcpy(e->key, key, n);
    e->n = n;
    e->value = value;
    e->ttl = r->n_ttl;
    e->realm = realm ? strdup(realm) : NULL;
    e->next = NULL;
    *pp = e;
    return e->key;
}
```

### 4.5 `realm_tick` — skip realm-managed entries

Realm-tagged entries are immune to TTL eviction. Only entries with
`realm == NULL` count down and expire.

```c
// Before:
void realm_tick (Realm* r) {
    for (int i = 0; i < r->n_buk; i++) {
        Realm_Entry** pp = &r->buckets[i];
        while (*pp != NULL) {
            Realm_Entry* e = *pp;
            e->ttl--;
            if (e->ttl <= 0) {
                realm_remove(r, pp);
            } else {
                pp = &e->next;
            }
        }
    }
}

// After:
void realm_tick (Realm* r) {
    for (int i = 0; i < r->n_buk; i++) {
        Realm_Entry** pp = &r->buckets[i];
        while (*pp != NULL) {
            Realm_Entry* e = *pp;
            if (e->realm != NULL) {
                pp = &e->next;      // realm-managed: skip TTL countdown
            } else {
                e->ttl--;
                if (e->ttl <= 0) {
                    realm_remove(r, pp);
                } else {
                    pp = &e->next;
                }
            }
        }
    }
}
```

### 4.6 `realm_leave` — new function (add after `realm_tick`)

```c
void realm_leave (Realm* r, const char* tag) {
    if (tag == NULL) return;
    for (int i = 0; i < r->n_buk; i++) {
        Realm_Entry** pp = &r->buckets[i];
        while (*pp != NULL) {
            Realm_Entry* e = *pp;
            if (e->realm != NULL && strcmp(e->realm, tag) == 0) {
                realm_remove(r, pp);    // calls clean, frees key+realm+entry
            } else {
                pp = &e->next;
            }
        }
    }
}
```

### 4.7 Complete `realm.h` (final state)

```c
#ifndef REALM_H
#define REALM_H

#include <stdlib.h>
#include <string.h>

typedef void (*Realm_Clean) (int n, const void* key, void* value);

typedef struct Realm_Entry {
    int    n;
    void*  key;
    void*  value;
    int    ttl;
    char*  realm;          // NULL = TTL-only; non-NULL = realm-managed
    struct Realm_Entry* next;
} Realm_Entry;

typedef struct {
    int           n_buk;
    int           n_ttl;
    Realm_Clean   clean;
    Realm_Entry** buckets;
} Realm;

Realm*      realm_open  (int n_buk, int n_ttl, Realm_Clean f);
void        realm_close (Realm* r);
const void* realm_put   (Realm* r, int n, const void* key, void* value,
                         const char* realm);
void*       realm_get   (Realm* r, int n, const void* key);
int         realm_rem   (Realm* r, int n, const void* key);
void        realm_tick  (Realm* r);
void        realm_leave (Realm* r, const char* tag);

#endif

#ifdef REALM_C

/* DJB2 hash function */
static unsigned long realm_djb2 (int n, const void* key) {
    unsigned long hash = 5381;
    const unsigned char* p = (const unsigned char*)key;
    for (int i = 0; i < n; i++) {
        hash = ((hash << 5) + hash) + p[i];
    }
    return hash;
}

/* Find entry in bucket chain, return pointer to the link pointing to it */
static Realm_Entry** realm_find (Realm* r, int n, const void* key) {
    unsigned long hash = realm_djb2(n, key);
    int idx = hash % r->n_buk;
    Realm_Entry** pp = &r->buckets[idx];
    while (*pp != NULL) {
        Realm_Entry* e = *pp;
        if (e->n == n && memcmp(e->key, key, n) == 0) {
            return pp;
        }
        pp = &e->next;
    }
    return pp;
}

/* Remove entry and call cleanup callback */
static void realm_remove (Realm* r, Realm_Entry** pp) {
    Realm_Entry* e = *pp;
    *pp = e->next;
    if (r->clean != NULL) {
        r->clean(e->n, e->key, e->value);
    }
    free(e->key);
    free(e->realm);
    free(e);
}

Realm* realm_open (int n_buk, int n_ttl, Realm_Clean f) {
    Realm* r = malloc(sizeof(Realm));
    if (r == NULL) {
        return NULL;
    }
    r->buckets = calloc(n_buk, sizeof(Realm_Entry*));
    if (r->buckets == NULL) {
        free(r);
        return NULL;
    }
    r->n_buk = n_buk;
    r->n_ttl = n_ttl;
    r->clean = f;
    return r;
}

void realm_close (Realm* r) {
    for (int i = 0; i < r->n_buk; i++) {
        while (r->buckets[i] != NULL) {
            realm_remove(r, &r->buckets[i]);
        }
    }
    free(r->buckets);
    free(r);
}

const void* realm_put (Realm* r, int n, const void* key, void* value,
                       const char* realm) {
    Realm_Entry** pp = realm_find(r, n, key);

    /* Key exists: replace value and realm tag */
    if (*pp != NULL) {
        Realm_Entry* e = *pp;
        if (r->clean != NULL) {
            r->clean(e->n, e->key, e->value);
        }
        e->value = value;
        e->ttl = r->n_ttl;
        free(e->realm);
        e->realm = realm ? strdup(realm) : NULL;
        return e->key;
    }

    /* Key does not exist: create new entry */
    Realm_Entry* e = (Realm_Entry*)malloc(sizeof(Realm_Entry));
    if (e == NULL) {
        return NULL;
    }
    e->key = malloc(n);
    if (e->key == NULL) {
        free(e);
        return NULL;
    }
    memcpy(e->key, key, n);
    e->n = n;
    e->value = value;
    e->ttl = r->n_ttl;
    e->realm = realm ? strdup(realm) : NULL;
    e->next = NULL;
    *pp = e;
    return e->key;
}

void* realm_get (Realm* r, int n, const void* key) {
    Realm_Entry** pp = realm_find(r, n, key);
    if (*pp == NULL) {
        return NULL;
    }
    Realm_Entry* e = *pp;
    e->ttl = r->n_ttl;
    return e->value;
}

int realm_rem (Realm* r, int n, const void* key) {
    Realm_Entry** pp = realm_find(r, n, key);
    if (*pp == NULL) {
        return -1;
    }
    realm_remove(r, pp);
    return 0;
}

void realm_tick (Realm* r) {
    for (int i = 0; i < r->n_buk; i++) {
        Realm_Entry** pp = &r->buckets[i];
        while (*pp != NULL) {
            Realm_Entry* e = *pp;
            if (e->realm != NULL) {
                pp = &e->next;      // realm-managed: skip TTL countdown
            } else {
                e->ttl--;
                if (e->ttl <= 0) {
                    realm_remove(r, pp);
                } else {
                    pp = &e->next;
                }
            }
        }
    }
}

void realm_leave (Realm* r, const char* tag) {
    if (tag == NULL) return;
    for (int i = 0; i < r->n_buk; i++) {
        Realm_Entry** pp = &r->buckets[i];
        while (*pp != NULL) {
            Realm_Entry* e = *pp;
            if (e->realm != NULL && strcmp(e->realm, tag) == 0) {
                realm_remove(r, pp);
            } else {
                pp = &e->next;
            }
        }
    }
}

#endif
```

---

## 5. Changes to `src/pico.c`

All call sites pass `NULL` as the `realm` argument — preserving current
TTL-only behavior exactly. No logic change in pico-sdl at this stage.

### 5.1 Preprocessor (top of file)

```c
// Before:
#define TTL_HASH_C
#include "hash.h"

// After:
#define REALM_C
#include "realm.h"
```

### 5.2 Global state struct `G`

```c
// Before:
    ttl_hash*     hash;

// After:
    Realm*        realm;
```

### 5.3 `_tick_and_check`

```c
// Before:
    ttl_hash_tick(G.hash);

// After:
    realm_tick(G.realm);
```

### 5.4 `_font_get`

```c
// Before:
    TTF_Font* ttf = (TTF_Font*)ttl_hash_get(G.hash, n, key);
    if (ttf != NULL) {
        return ttf;
    }
    ttf = _font_open(path, h);
    ttl_hash_put(G.hash, n, key, ttf);
    return ttf;

// After:
    TTF_Font* ttf = (TTF_Font*)realm_get(G.realm, n, key);
    if (ttf != NULL) {
        return ttf;
    }
    ttf = _font_open(path, h);
    realm_put(G.realm, n, key, ttf, NULL);
    return ttf;
```

### 5.5 `_ttl_hash_get_layer` → renamed to `_realm_get_layer`

```c
// Before:
static Pico_Layer* _ttl_hash_get_layer (const char* name) {
    int n = sizeof(Pico_Key) + strlen(name) + 1;
    Pico_Key* key = alloca(n);
    key->type = PICO_KEY_LAYER;
    strcpy(key->key, name);
    return (Pico_Layer*)ttl_hash_get(G.hash, n, key);
}

// After:
static Pico_Layer* _realm_get_layer (const char* name) {
    int n = sizeof(Pico_Key) + strlen(name) + 1;
    Pico_Key* key = alloca(n);
    key->type = PICO_KEY_LAYER;
    strcpy(key->key, name);
    return (Pico_Layer*)realm_get(G.realm, n, key);
}
```

### 5.6 `_ttl_hash_put_layer` → renamed to `_realm_put_layer`

```c
// Before:
static const Pico_Key* _ttl_hash_put_layer (
    const char* name, Pico_Layer* data
) {
    int n = sizeof(Pico_Key) + strlen(name) + 1;
    Pico_Key* key = alloca(n);
    key->type = PICO_KEY_LAYER;
    strcpy(key->key, name);
    return ttl_hash_put(G.hash, n, key, data);
}

// After:
static const Pico_Key* _realm_put_layer (
    const char* name, Pico_Layer* data
) {
    int n = sizeof(Pico_Key) + strlen(name) + 1;
    Pico_Key* key = alloca(n);
    key->type = PICO_KEY_LAYER;
    strcpy(key->key, name);
    return realm_put(G.realm, n, key, data, NULL);
}
```

### 5.7 `_pico_hash_clean` → renamed to `_pico_realm_clean`

Function name only — body is unchanged (including the call to
`_pico_hash_clean_video`, which becomes `_pico_realm_clean_video` in 6.1):

```c
// Before:
static void _pico_hash_clean (int n, const void* key, void* value) {
    ...
        _pico_hash_clean_video((Pico_Layer_Video*)data);
    ...
}

// After:
static void _pico_realm_clean (int n, const void* key, void* value) {
    ...
        _pico_realm_clean_video((Pico_Layer_Video*)data);
    ...
}
```

### 5.8 `pico_init(1)` — open and assert

```c
// Before:
    .hash  = ttl_hash_open(PICO_HASH_BUK, PICO_HASH_TTL, _pico_hash_clean),
...
    assert(G.hash != NULL);

// After:
    .realm = realm_open(PICO_REALM_BUK, PICO_REALM_TTL, _pico_realm_clean),
...
    assert(G.realm != NULL);
```

### 5.9 `pico_init(0)` — close

```c
// Before:
    if (G.hash != NULL) {
        ttl_hash_close(G.hash);
    }

// After:
    if (G.realm != NULL) {
        realm_close(G.realm);
    }
```

### 5.10 `_pico_output_sound_cache`

```c
// Before:
    mix = (Mix_Chunk*)ttl_hash_get(G.hash, n, res);
    if (mix == NULL) {
        mix = Mix_LoadWAV(path);
        ttl_hash_put(G.hash, n, res, mix);
    }

// After:
    mix = (Mix_Chunk*)realm_get(G.realm, n, res);
    if (mix == NULL) {
        mix = Mix_LoadWAV(path);
        realm_put(G.realm, n, res, mix, NULL);
    }
```

### 5.11 All call sites of `_ttl_hash_get_layer` and `_ttl_hash_put_layer`

Every occurrence of `_ttl_hash_get_layer(` becomes `_realm_get_layer(` and
every occurrence of `_ttl_hash_put_layer(` becomes `_realm_put_layer(`.
These appear in: `pico_layer_set`, `pico_layer_buffer`, `pico_layer_new`,
`pico_layer_sub`, `_pico_layer_image`, `_pico_layer_text`, and
`pico_layer_remove`. Do a global rename — do not search by line number.

---

## 6. Changes to `src/video.h`

### 6.1 `_pico_hash_clean_video` → `_pico_realm_clean_video`

Two locations: the forward declaration and the definition:

```c
// Before (forward decl):
static void _pico_hash_clean_video (Pico_Layer_Video*);

// After:
static void _pico_realm_clean_video (Pico_Layer_Video*);

// Before (definition):
/* Free video-specific resources (called from hash cleanup) */
static void _pico_hash_clean_video (Pico_Layer_Video* vs) {

// After:
/* Free video-specific resources (called from realm cleanup) */
static void _pico_realm_clean_video (Pico_Layer_Video* vs) {
```

### 6.2 `_pico_layer_video` — get and put calls

```c
// Before:
    Pico_Layer_Video* vs =
        (Pico_Layer_Video*)ttl_hash_get(
            G.hash, n, key);

// After:
    Pico_Layer_Video* vs =
        (Pico_Layer_Video*)realm_get(
            G.realm, n, key);
```

```c
// Before:
        .key  = ttl_hash_put(G.hash, n, key, vs),

// After:
        .key  = realm_put(G.realm, n, key, vs, NULL),
```

### 6.3 `pico_set_video` — get call

```c
// Before:
    Pico_Layer* layer =
        (Pico_Layer*)ttl_hash_get(G.hash, n, key);

// After:
    Pico_Layer* layer =
        (Pico_Layer*)realm_get(G.realm, n, key);
```

---

## 7. Changes to `src/pico.h`

```c
// Before:
#define PICO_HASH_BUK  128
#define PICO_HASH_TTL  1000     // expire after N pico_input_* calls

// After:
#define PICO_REALM_BUK  128
#define PICO_REALM_TTL  1000    // expire after N pico_input_* calls
```

---

## 8. `valgrind.supp` Update

The file `valgrind.supp` contains a suppression named `sdl-init` with a line:

```
src:pico.c:N
```

where `N` is the line number of the `SDL_Init` call inside `pico_init` in
`src/pico.c`. After any edits to `pico.c`, that line number may shift.

**Procedure** (per CLAUDE.md instructions — must be done automatically):

1. After all edits to `src/pico.c` are complete, find the new line number:
   ```bash
   grep -n "SDL_Init" src/pico.c
   ```
2. Update `valgrind.supp`: replace the old `src:pico.c:N` with the new line
   number.

The `sdl-init` entry in `valgrind.supp` currently looks like:

```
{
   sdl-init
   Memcheck:Leak
   ...
   src:pico.c:N
}
```

Only the `N` changes; everything else stays the same.

---

## 9. Behavioral Specification

### Two eviction mechanisms, per entry

| `realm` field | Eviction trigger              |
|---------------|-------------------------------|
| `NULL`        | TTL countdown via `realm_tick`|
| `"tag"`       | Explicit `realm_leave("tag")` |

Realm-tagged entries are **completely immune** to `realm_tick`. They live
indefinitely until `realm_leave` is called (or `realm_close`).

TTL-only entries work exactly as before: TTL is reset on every `realm_get`,
and counted down on every `realm_tick`. When TTL reaches 0 the `clean`
callback fires and the entry is freed.

### `realm_get` on a realm-tagged entry

Resets `ttl` (harmless since TTL countdown is skipped for realm entries).
This is intentional — it avoids a branch and keeps `realm_get` uniform.

### `realm_put` on an existing key

Replaces value **and** realm tag. The old resource is freed via `clean`. The
old realm string is freed. The new realm string (if any) is `strdup`'d.

### `realm_leave(NULL)`

No-op. Safe to call.

### `realm_close`

Frees **all** entries regardless of realm tag or TTL. Calls `clean` for each.

### `realm_rem`

Removes a single entry by key, regardless of realm tag. Calls `clean`.
Returns 0 on success, -1 if key not found.

---

## 10. Files Changed (complete list)

| File              | Change                                                |
|-------------------|-------------------------------------------------------|
| `src/hash.h`      | **Deleted** (replaced by `src/realm.h`)               |
| `src/realm.h`     | **New** — fork of hash.h, renames + realm support     |
| `src/pico.c`      | 18 substitutions (see section 5)                      |
| `src/video.h`     | 4 substitutions (see section 6)                       |
| `src/pico.h`      | 2 constant renames (see section 7)                    |
| `valgrind.supp`   | Update `src:pico.c:N` line number (see section 8)     |

---

## 11. Out of Scope (future work)

- Assigning actual realm tags to pico-sdl resources (fonts, sounds, layers)
- Exposing `realm_leave` to the Lua API
- Nested or hierarchical realms
- Multiple `Realm` instances (one per resource category)
- Lua-side realm management (`pico.realm.enter`, `pico.realm.leave`)
