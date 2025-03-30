#include "../memory/stats.h"
#include <stdlib.h>
#include <string.h>

DataStats data_stats = {0};

void* track_malloc(size_t data_size) {
    data_stats.malloc_state++;
    return malloc(data_size);
}

void* track_calloc(size_t num, size_t data_size) {
    data_stats.calloc_state++;
    return calloc(num, data_size);
}

char* track_strdup(const char* str) {
    if (str == NULL) return NULL;

    size_t length = strlen(str) + 1;
    char* copy = (char*)track_malloc(length);
    if (copy) memcpy(copy, str, length);

    return copy;
}

char* track_strndup(const char* str, size_t n) {
    if (str == NULL) return NULL;

    size_t length = strnlen(str, n);
    char* copy = (char*)track_malloc(length + 1);
    if (copy) {
        memcpy(copy, str, length);
        copy[length] = '\0';
    }

    return copy;
}

void* track_realloc(void* pointer, size_t data_size) {
    data_stats.realloc_state++;
    return realloc(pointer, data_size);
}

void track_free(void* pointer) {
    if (!pointer) return;
    data_stats.free_count++;
    pointer = NULL;
}

void get_memstat(DataStats* memstat) {
    *memstat = data_stats;
}