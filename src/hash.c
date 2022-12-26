#include <assert.h>
#include <string.h>
#include "hash.h"

typedef struct pico_hash_pair {
    char *key;
    void *value;
    struct pico_hash_pair *next;
} pico_hash_pair;

typedef struct pico_hash {
    pico_hash_pair **buckets;
    size_t num_buckets;
} pico_hash;

pico_hash* pico_hash_create (size_t num_buckets) {
    pico_hash *table = malloc(sizeof(pico_hash));
    if (table == NULL) {
        return NULL;
    }

    table->num_buckets = num_buckets;
    table->buckets = calloc(table->num_buckets, sizeof(pico_hash_pair *));
    if (table->buckets == NULL) {
        free(table);
        return NULL;
    }

    return table;
}

void pico_hash_destroy (pico_hash *table) {
    pico_hash_pair *pair;
    pico_hash_pair *tmp;
    for (size_t i = 0; i < table->num_buckets; i++) {
        pair = table->buckets[i];
        while (pair != NULL) {
            tmp = pair;
            pair = pair->next;
            free(tmp->key);
            free(tmp);
        }
    }
    free(table->buckets);
    free(table);
}

size_t _pico_hash_hash (const char *str, size_t num_buckets) {
    size_t hash = 0;
    for (const char *p = str; *p != '\0'; p++) {
        hash = (hash << 5) + hash + *p;
    }
    return hash % num_buckets;
}

int pico_hash_add (pico_hash *table, const char *key, void *value) {
    size_t index = _pico_hash_hash(key, table->num_buckets);

    // Check if the key already exists in the hash table
    pico_hash_pair *pair = table->buckets[index];
    while (pair != NULL) {
        if (strcmp(pair->key, key) == 0) {
            // Key already exists in the hash table, update the value
            pair->value = value;
            return 1;
        }
        pair = pair->next;
    }

    // Key does not exist in the hash table, add a new key-value pair
    pair = malloc(sizeof(pico_hash_pair));
    if (pair == NULL) {
        return 0;
    }

    pair->key = strdup(key);
    if (pair->key == NULL) {
        free(pair);
        return 0;
    }

    pair->value = value;
    pair->next = table->buckets[index];
    table->buckets[index] = pair;

    return 1;
}

int pico_hash_rem (pico_hash* table, const char *key) {
    size_t index = _pico_hash_hash(key, table->num_buckets);

    // Search for the key-value pair in the linked list at the appropriate index
    pico_hash_pair *pair = table->buckets[index];
    pico_hash_pair *prev = NULL;
    while (pair != NULL) {
        if (strcmp(pair->key, key) == 0) {
            // Key found, remove the key-value pair from the linked list
            if (prev == NULL) {
                // Key-value pair is at the head of the linked list
                table->buckets[index] = pair->next;
            } else {
                // Key-value pair is not at the head of the linked list
                prev->next = pair->next;
            }
            free(pair->key);
            free(pair);
            return 1;
        }
        prev = pair;
        pair = pair->next;
    }

    // Key not found in the hash table
    return 0;
}

void* pico_hash_get (pico_hash* table, const char* key) {
  size_t index = _pico_hash_hash(key, table->num_buckets);

  // Search for the key-value pair in the linked list at the appropriate index
  pico_hash_pair *pair = table->buckets[index];
  while (pair != NULL) {
    if (strcmp(pair->key, key) == 0) {
      // Key found, return the value
      return pair->value;
    }
    pair = pair->next;
  }

  // Key not found in the hash table, return NULL
  return NULL;
}
