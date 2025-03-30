#ifndef DB_STATS_H
#define DB_STATS_H
#include <sys/_types/_size_t.h>

typedef struct {
    size_t malloc_state;
    size_t calloc_state;
    size_t realloc_state;
    size_t free_count;
} DataStats;

void* track_malloc(size_t);
char* track_strdup(const char*);
char* track_strndup(const char*, size_t);
void* track_calloc(size_t, size_t);
void* track_realloc(void*, size_t);
void track_free(void*);

void get_memstat(DataStats*);

#endif //DB_STATS_H