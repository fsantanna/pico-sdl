#ifndef TTL_HASH_H
#define TTL_HASH_H

#include <stdlib.h>
#include <string.h>

typedef void (*cb_clean_t) (int n, const void* key, void* value);

typedef struct ttl_hash_entry {
    int n;
    void* key;
    void* value;
    int ttl;
    struct ttl_hash_entry* next;
} ttl_hash_entry;

typedef struct {
    int n_buk;
    int n_ttl;
    cb_clean_t clean;
    ttl_hash_entry** buckets;
} ttl_hash;

ttl_hash* ttl_hash_open  (int n_buk, int n_ttl, cb_clean_t f);
void      ttl_hash_close (ttl_hash* ht);
int       ttl_hash_put   (ttl_hash* ht, int n, const void* key, void* value);
void*     ttl_hash_get   (ttl_hash* ht, int n, const void* key);
int       ttl_hash_rem   (ttl_hash* ht, int n, const void* key);
void      ttl_hash_tick  (ttl_hash* ht);

#endif

#ifdef TTL_HASH_C

/* DJB2 hash function */
static unsigned long ttl_hash_djb2 (int n, const void* key) {
    unsigned long hash = 5381;
    const unsigned char* p = (const unsigned char*)key;
    for (int i = 0; i < n; i++) {
        hash = ((hash << 5) + hash) + p[i];
    }
    return hash;
}

/* Find entry in bucket chain, return pointer to the link pointing to it */
static ttl_hash_entry** ttl_hash_find (ttl_hash* ht, int n, const void* key) {
    unsigned long hash = ttl_hash_djb2(n, key);
    int idx = hash % ht->n_buk;
    ttl_hash_entry** pp = &ht->buckets[idx];
    while (*pp != NULL) {
        ttl_hash_entry* e = *pp;
        if (e->n == n && memcmp(e->key, key, n) == 0) {
            return pp;
        }
        pp = &e->next;
    }
    return pp;
}

/* Remove entry and call cleanup callback */
static void ttl_hash_remove_entry (ttl_hash* ht, ttl_hash_entry** pp) {
    ttl_hash_entry* e = *pp;
    *pp = e->next;
    if (ht->clean != NULL) {
        ht->clean(e->n, e->key, e->value);
    }
    free(e->key);
    free(e);
}

ttl_hash* ttl_hash_open (int n_buk, int n_ttl, cb_clean_t f) {
    ttl_hash* ht = malloc(sizeof(ttl_hash));
    if (ht == NULL) {
        return NULL;
    }
    ht->buckets = calloc(n_buk, sizeof(ttl_hash_entry*));
    if (ht->buckets == NULL) {
        free(ht);
        return NULL;
    }
    ht->n_buk = n_buk;
    ht->n_ttl = n_ttl;
    ht->clean = f;
    return ht;
}

void ttl_hash_close (ttl_hash* ht) {
    for (int i = 0; i < ht->n_buk; i++) {
        while (ht->buckets[i] != NULL) {
            ttl_hash_remove_entry(ht, &ht->buckets[i]);
        }
    }
    free(ht->buckets);
    free(ht);
}

int ttl_hash_put (ttl_hash* ht, int n, const void* key, void* value) {
    ttl_hash_entry** pp = ttl_hash_find(ht, n, key);

    /* Key exists: replace value */
    if (*pp != NULL) {
        ttl_hash_entry* e = *pp;
        if (ht->clean != NULL) {
            ht->clean(e->n, e->key, e->value);
        }
        e->value = value;
        e->ttl = ht->n_ttl;
        return 0;
    }

    /* Key does not exist: create new entry */
    ttl_hash_entry* e = (ttl_hash_entry*)malloc(sizeof(ttl_hash_entry));
    if (e == NULL) {
        return -1;
    }
    e->key = malloc(n);
    if (e->key == NULL) {
        free(e);
        return -1;
    }
    memcpy(e->key, key, n);
    e->n = n;
    e->value = value;
    e->ttl = ht->n_ttl;
    e->next = NULL;
    *pp = e;
    return 0;
}

void* ttl_hash_get (ttl_hash* ht, int n, const void* key) {
    ttl_hash_entry** pp = ttl_hash_find(ht, n, key);
    if (*pp == NULL) {
        return NULL;
    }
    ttl_hash_entry* e = *pp;
    e->ttl = ht->n_ttl;
    return e->value;
}

int ttl_hash_rem (ttl_hash* ht, int n, const void* key) {
    ttl_hash_entry** pp = ttl_hash_find(ht, n, key);
    if (*pp == NULL) {
        return -1;
    }
    ttl_hash_remove_entry(ht, pp);
    return 0;
}

void ttl_hash_tick (ttl_hash* ht) {
    for (int i = 0; i < ht->n_buk; i++) {
        ttl_hash_entry** pp = &ht->buckets[i];
        while (*pp != NULL) {
            ttl_hash_entry* e = *pp;
            e->ttl--;
            if (e->ttl <= 0) {
                ttl_hash_remove_entry(ht, pp);
            } else {
                pp = &e->next;
            }
        }
    }
}

#endif
