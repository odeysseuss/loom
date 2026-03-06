/*
* Arena allocator
*
* NOTE: To use this library define the following macro in exactly one file
* before inlcuding arena.h:
*   #define ARENA_IMPLEMENTATION
*   #include "arena.h"
*/

#ifndef ARENA_H
#define ARENA_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Used for custom general purpose allocators
/// To use a custom allocators simply define them to the specific allocator's functions
/// Warning: The function signatures has to be the same as stdlib's allocator
/// Recommended Allocator: Microsoft's mimalloc
#define malloc_ malloc
#define calloc_ calloc
#define realloc_ realloc
#define free_ free

typedef struct {
    void *memory;
    size_t capacity;
    size_t offset;
} ArenaAlloc;

ArenaAlloc *arenaInit(size_t arena_size);
void *arenaAlloc(ArenaAlloc *arena, size_t size);
void arenaClear(ArenaAlloc *arena);
void arenaFree(ArenaAlloc *arena);

#ifdef ARENA_IMPLEMENTATION

ArenaAlloc *arenaInit(size_t arena_size) {
    if (arena_size == 0) {
        return NULL;
    }

    ArenaAlloc *arena = malloc_(sizeof(ArenaAlloc));
    arena->memory = calloc_(1, arena_size);
    arena->capacity = arena_size;
    arena->offset = 0;

    return arena;
}

void *arenaAlloc(ArenaAlloc *arena, size_t size) {
    if (!arena || arena->capacity < arena->offset) {
        return NULL;
    };

    void *chunk = (unsigned char *)arena->memory + arena->offset;
    arena->offset += size;

    return chunk;
}

void arenaClear(ArenaAlloc *arena) {
    if (!arena) {
        return;
    }

    arena->offset = 0;
}

void arenaFree(ArenaAlloc *arena) {
    if (!arena) {
        return;
    }

    free_(arena->memory);
    free_(arena);
}

#endif // ARENA_IMPLEMENTATION

#undef malloc_
#undef calloc_
#undef realloc_
#undef free_

#ifdef __cplusplus
}
#endif

#endif
