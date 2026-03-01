#ifndef REALM_H
#define REALM_H

#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef void  (*realm_free_t)  (int n, const void* key, void* value);
typedef void* (*realm_alloc_t) (int n, const void* key, void* ctx);

#define REALM_EXCLUSIVE   '!'
#define REALM_SHARED      '='
#define REALM_REPLACEABLE '~'

typedef struct realm_entry {
    int                 n;
    void*               key;
    void*               value;
    int                 depth;
    realm_free_t        free;
    struct realm_entry* next;
} realm_entry;

typedef struct {
    int            n;
    realm_entry**  entries;
    int            depth;
} realm_t;

realm_t* realm_open  (int n);
void     realm_close (realm_t* r);
void     realm_enter (realm_t* r);
void     realm_leave (realm_t* r);
void*    realm_put   (realm_t* r, int mode, int n, const void* key,
                      realm_free_t free, realm_alloc_t alloc, void* ctx);
void*    realm_get   (realm_t* r, int n, const void* key);

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

/* Find entry in bucket chain */
static realm_entry** realm_find (realm_t* r, int n, const void* key) {
    unsigned long hash = realm_djb2(n, key);
    int idx = hash % r->n;
    realm_entry** pp = &r->entries[idx];
    while (*pp != NULL) {
        realm_entry* e = *pp;
        if (e->n == n && memcmp(e->key, key, n) == 0) {
            return pp;
        }
        pp = &e->next;
    }
    return pp;
}

/* Remove entry and call per-entry free callback */
static void realm_remove_entry (realm_entry** pp) {
    realm_entry* e = *pp;
    *pp = e->next;
    if (e->free != NULL) {
        e->free(e->n, e->key, e->value);
    }
    free(e->key);
    free(e);
}

realm_t* realm_open (int n) {
    realm_t* r = malloc(sizeof(realm_t));
    if (r == NULL) {
        return NULL;
    }
    r->entries = calloc(n, sizeof(realm_entry*));
    if (r->entries == NULL) {
        free(r);
        return NULL;
    }
    r->n = n;
    r->depth = 0;
    return r;
}

void realm_close (realm_t* r) {
    for (int i = 0; i < r->n; i++) {
        while (r->entries[i] != NULL) {
            realm_remove_entry(&r->entries[i]);
        }
    }
    free(r->entries);
    free(r);
}

void realm_enter (realm_t* r) {
    r->depth++;
}

void realm_leave (realm_t* r) {
    assert(r->depth > 0);
    r->depth--;
    for (int i = 0; i < r->n; i++) {
        realm_entry** pp = &r->entries[i];
        while (*pp != NULL) {
            if ((*pp)->depth == r->depth) {
                realm_remove_entry(pp);
            } else {
                pp = &(*pp)->next;
            }
        }
    }
}

void* realm_put (realm_t* r, int mode, int n, const void* key,
                 realm_free_t free_, realm_alloc_t alloc, void* ctx) {
    assert(r->depth > 0);
    if (mode == '=') {
        assert(alloc != NULL);
    }
    realm_entry** pp = realm_find(r, n, key);

    /* Key exists */
    if (*pp != NULL) {
        realm_entry* e = *pp;
        switch (mode) {
            case '!':
                assert(0 && "realm: exclusive key exists");
                return NULL;
            case '=':
                return e->value;
            case '~': {
                void* nv = (alloc != NULL) ? alloc(n, key, ctx) : ctx;
                if (e->free != NULL) {
                    e->free(e->n, e->key, e->value);
                }
                e->depth = r->depth - 1;
                e->value = nv;
                e->free = free_;
                return nv;
            }
            default: assert(0 && "bug found");
        }
    }

    /* Key does not exist: create */
    else {
        void* nv = (alloc != NULL) ? alloc(n, key, ctx) : ctx;
        realm_entry* e = malloc(sizeof(realm_entry));
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
        e->value = nv;
        e->depth = r->depth - 1;
        e->free = free_;
        e->next = NULL;
        *pp = e;
        return nv;
    }
}

void* realm_get (realm_t* r, int n, const void* key) {
    realm_entry** pp = realm_find(r, n, key);
    if (*pp == NULL) {
        return NULL;
    } else {
        return (*pp)->value;
    }
}

#endif
