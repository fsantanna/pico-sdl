# ttl-hash → realm-allocator

## 1. Motivation: Drop TTL Entirely

The TTL mechanism in `ttl-hash` is **automatic** lifetime management: every
`n_ttl` ticks without a `realm_get`, an entry expires. This requires:

- A `ttl` counter field on every entry
- A `realm_tick` function called on every input event
- The caller to tune `n_ttl` (currently hardcoded to 1000)
- Trust that "not accessed recently" means "no longer needed"

These are all indirect, implicit, and fragile. The core insight is:

> The caller always knows exactly when a resource is no longer needed.
> TTL is a workaround for not having an explicit lifetime API.

**`realm-allocator` removes TTL entirely** and replaces it with three explicit
lifetime modes. No ticks, no counters, no tuning.

---

## 2. The 3-Mode Lifetime Matrix

Each entry is created with one of three modes:

| Mode | Name      | Eviction trigger                         |
|------|-----------|------------------------------------------|
| `'n'`| None      | Only `realm_rem` or `realm_close`        |
| `'r'`| Realm     | `realm_leave(tag)` or `realm_close`      |
| `'x'`| eXclusive | First `realm_get` (ownership transfer)   |

### `'n'` — None (permanent)

Entry lives until explicitly removed (`realm_rem`) or the allocator is closed
(`realm_close`). The `tag` parameter is ignored (pass `NULL`).

Use for: global singletons, persistent resources that outlive any specific
scene or operation.

### `'r'` — Realm (grouped lifetime)

Entry is tagged with a realm string and freed when `realm_leave(tag)` is
called. Also freed on `realm_close`. This is the core feature of the library:
bulk-free of a named group.

Use for: resources with a known collective lifetime (level assets, UI screen
resources, temporary scratch buffers with a named scope).

### `'x'` — eXclusive (consume-once)

Entry is removed from the hash on the **first** `realm_get`. The clean
callback is **not** called on removal — ownership is transferred to the
caller. The caller is then responsible for the resource.

If the entry is never gotten (e.g., cleared by `realm_rem` or `realm_close`),
the clean callback **is** called as usual.

Use for: one-shot resources that are produced by one part of the code and
consumed once by another (handoff pattern, pipeline stages).

---

## 3. What Disappears

Compared to `ttl-hash`:

| Removed           | Reason                                      |
|-------------------|---------------------------------------------|
| `int ttl` field   | No TTL countdown                            |
| `int n_ttl` field | No TTL configuration                        |
| `realm_tick()`    | No per-tick processing                      |
| TTL reset in `realm_get` | Not needed without TTL              |
| `n_ttl` param in `realm_open` | Not needed                   |

---

## 4. Data Structures

```c
typedef void (*Realm_Clean) (int n, const void* key, void* value);

typedef struct Realm_Entry {
    int    n;               // key size in bytes
    void*  key;             // heap-allocated copy of key
    void*  value;           // caller-owned resource pointer
    char   mode;            // 'n', 'r', or 'x'
    char*  tag;             // heap-allocated realm string; NULL unless mode='r'
    struct Realm_Entry* next;
} Realm_Entry;

typedef struct {
    int           n_buk;   // number of hash buckets
    Realm_Clean   clean;   // callback to free value (NULL = no cleanup)
    Realm_Entry** buckets;
} Realm;
```

---

## 5. API Sketch

```c
// --- Lifecycle ---

// Create allocator with n_buk buckets.
// f is called to free the value when an entry is evicted (may be NULL).
Realm* realm_open  (int n_buk, Realm_Clean f);

// Free all entries (calling clean for each) and destroy the allocator.
void   realm_close (Realm* r);


// --- Entry operations ---

// Insert or replace entry with key of size n bytes.
//   mode='n': permanent; tag is ignored (pass NULL)
//   mode='r': realm-managed; tag is the realm name (required, copied)
//   mode='x': consume-once; tag is ignored (pass NULL)
// If key already exists, the old value is cleaned and replaced.
// Returns pointer to the internal copy of key, or NULL on alloc failure.
const void* realm_put (Realm* r, int n, const void* key, void* value,
                       char mode, const char* tag);

// Look up entry by key.
//   mode='n','r': returns value; no side effects.
//   mode='x': returns value AND removes entry WITHOUT calling clean
//             (ownership transferred to caller).
// Returns NULL if key not found.
void* realm_get (Realm* r, int n, const void* key);

// Remove entry by key. Calls clean regardless of mode.
// Returns 0 on success, -1 if key not found.
int realm_rem (Realm* r, int n, const void* key);


// --- Realm operations ---

// Free all entries whose mode='r' and tag equals `tag`.
// Calls clean for each. Safe to call with tag=NULL (no-op).
void realm_leave (Realm* r, const char* tag);
```

---

## 6. Behavior Table

Full behavior matrix for each function × mode:

| Function       | `'n'`                        | `'r'`                              | `'x'`                              |
|----------------|------------------------------|------------------------------------|------------------------------------|
| `realm_put`    | store permanently            | store under realm `tag`            | store as consume-once              |
| `realm_get`    | return value, no side effect | return value, no side effect       | return value, remove entry (no clean) |
| `realm_rem`    | remove, call clean           | remove, call clean                 | remove, call clean                 |
| `realm_leave`  | no effect                    | remove all with matching tag, call clean | no effect                    |
| `realm_close`  | remove, call clean           | remove, call clean                 | remove, call clean                 |

---

## 7. `realm_put` on Existing Key

When `realm_put` is called and the key already exists:

1. `clean(n, key, old_value)` is called (old resource freed)
2. `free(old_tag)` (old realm string freed)
3. New `value`, `mode`, and `tag` are stored in the existing entry

The key itself is **not** reallocated — the existing internal copy is reused.
This is the same behavior as `ttl_hash_put`.

---

## 8. Internal `realm_remove` (static helper)

```c
// Called internally for all eviction paths EXCEPT realm_get on mode='x'.
static void realm_remove (Realm* r, Realm_Entry** pp) {
    Realm_Entry* e = *pp;
    *pp = e->next;
    if (r->clean != NULL) {
        r->clean(e->n, e->key, e->value);
    }
    free(e->key);
    free(e->tag);
    free(e);
}

// Called only by realm_get on mode='x': no clean, ownership transferred.
static void realm_take (Realm_Entry** pp) {
    Realm_Entry* e = *pp;
    *pp = e->next;
    // clean NOT called — value returned to caller
    free(e->key);
    free(e->tag);  // NULL for 'x' entries, free(NULL) is safe
    free(e);
}
```

---

## 9. `realm_get` Implementation Sketch

```c
void* realm_get (Realm* r, int n, const void* key) {
    Realm_Entry** pp = realm_find(r, n, key);
    if (*pp == NULL) {
        return NULL;
    }
    Realm_Entry* e = *pp;
    void* value = e->value;
    if (e->mode == 'x') {
        realm_take(pp);     // consume: remove without clean
    }
    return value;
}
```

---

## 10. Invariants

- `tag` is `NULL` for modes `'n'` and `'x'`; non-NULL for mode `'r'`
- `realm_leave(NULL)` is always a no-op (safe)
- `free(NULL)` is always safe (C standard); no NULL guards needed for `free(e->tag)`
- `realm_close` frees **all** entries regardless of mode, always calling clean
- `realm_rem` always calls clean regardless of mode (explicit removal = caller
  decided to discard the resource, not take it)
- The hash function (`realm_djb2`) and bucket logic are unchanged from `ttl-hash`
