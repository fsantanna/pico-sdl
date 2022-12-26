#include <stdlib.h>
#include <stddef.h>

struct pico_table;
typedef struct pico_table pico_table;

pico_table* pico_table_create (size_t num_buckets);

// Destroys a given hash table
// table: pointer to the hash table to destroy
void pico_table_destroy (pico_table* table);

// Adds a value to the hash table with the given key
// table: pointer to the hash table
// key: the key to use for the value
// value: the value to add to the hash table
// Returns 1 if the value was added successfully, or 0 if an error occurred
int pico_table_add (pico_table* table, const char* key, void* value);

// Removes a value from the hash table with the given key
// table: pointer to the hash table
// key: the key of the value to remove
// Returns 1 if the value was removed successfully, or 0 if an error occurred
int pico_table_rem (pico_table* table, const char* key);

// Returns the value associated with the given key in the hash table, or NULL if the key does not exist
// table: pointer to the hash table
// key: the key of the value to retrieve
void* pico_table_get (pico_table* table, const char* key);
