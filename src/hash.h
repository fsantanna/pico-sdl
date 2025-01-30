/// @file hash.h
/// @brief Modules: @ref Hash.

#ifndef PICO_HASH_H
#define PICO_HASH_H

#include <stdlib.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct pico_hash;

typedef struct pico_hash pico_hash;

pico_hash* pico_hash_create (size_t num_buckets);
void pico_hash_destroy (pico_hash* table);
int pico_hash_add (pico_hash* table, const char* key, void* value);
int pico_hash_rem (pico_hash* table, const char* key);
void* pico_hash_get (pico_hash* table, const char* key);

#ifdef __cplusplus
}
#endif

#endif // PICO_HASH_H
