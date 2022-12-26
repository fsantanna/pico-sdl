#include <assert.h>
#include <stdio.h>
#include "../hash.h"

int main(int argc, char **argv) {
    pico_table* table = pico_table_create(16);
    assert(table != NULL);
    pico_table_add(table, "key1", "value1");
    pico_table_add(table, "key2", "value2");
    pico_table_rem(table, "key1");
    void* v1 = pico_table_get(table, "key1");
    void* v2 = pico_table_get(table, "key2");
    assert(v1 == NULL);
    assert(v2 != NULL);
    printf("v2 = %s\n", (char*) v2);
    pico_table_destroy(table);
    return 0;
}
