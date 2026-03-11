/*
* Hashmap implementation with xxhash and double hashing for collision handling
*
* NOTE: To use this library define the following macro in EXACTLY
* ONE FILE BEFORE inlcuding hashmap.h:
*   #define HASHMAP_IMPLEMENTATION
*   #include "hashmap.h"
*
* DEPENDENCIES: structs/xxhash.h
* REFERENCE: This library is heavily inspired by [tidwall/hashmap.c](https://github.com/tidwall/hashmap.c)
*/
#ifndef HASHMAP_H
#define HASHMAP_H

#define XXHASH_IMPLEMENTATION
#include "xxhash.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/// The lifetime of key and value is owned by the caller
typedef struct {
    void *key;
    void *val;
} Node;

typedef struct {
    // void *(*alloc)(size_t n);
    // void (*free)(void *node);
    size_t (*keySize)(const void *key);
    int (*cmp)(const void *a, const void *b); // returns 0 if equal
    void (*print)(const void *key, const void *val);
    size_t size;
    size_t capacity;
    float load_factor;
    size_t threshold;
    Node **items;
    Node tombstone;
} HashMap;

/// Helper for hashmapNew
typedef struct {
    // void *(*alloc)(size_t n);
    // void (*free)(void *node);
    size_t (*keySize)(const void *key);
    int (*cmp)(const void *a, const void *b); // returns 0 if equal
    void (*print)(const void *key, const void *val);
    size_t capacity;
    float load_factor;
} HashMapArgs;

/// Has to provide HashMapArgs with capacity, nodeKeySize and nodeCmp initializes
/// If load_factor is not provided than it falls back to 7.5
/// Free it with hashmapFree
HashMap *hashmapNew(HashMapArgs *args);
/// Only assigns the pointer. If there is an heap allocated key or value caller
/// is responsible for the lifetime of that memory
void hashmapSet(HashMap *map, const void *key, const void *val);
void *hashmapGet(const HashMap *map, const void *key);
/// If you freed the key or value of any heap allocated object be sure to call
/// hashmapDel as the the lifetime of key, val and node can be completely different.
/// Or, call hashmapSet with the same key with a different val before using it.
/// Otherwise, it will hold dangling pointers.
void hashmapDel(HashMap *map, const void *key);
void hashmapFree(HashMap *map);
/// Has to provide a nodeDisplay function when initializing HashMap via hashmapNew
void hashmapPrint(HashMap *map);

#define malloc_ malloc
#define calloc_ calloc
#define realloc_ realloc
#define free_ free

#ifdef HASHMAP_IMPLEMENTATION

static inline uint64_t getHash_(HashMap *map,
                                const void *key,
                                const size_t capacity,
                                const size_t attempt) {
    size_t size = map->keySize(key);
    uint64_t hash_a = xxh3(key, size, 0);
    const uint64_t hash_b = xxh3(key, size, 1);

    return (hash_a + attempt * (hash_b | 1)) % capacity;
}

static inline Node *nodeNew_(const void *key, const void *val) {
    Node *node = malloc_(sizeof(Node));
    node->key = (void *)key;
    node->val = (void *)val;

    return node;
}

static inline void nodeFree_(Node *node) {
    free_(node);
}

static void hashmapResize_(HashMap *map) {
    size_t old_cap = map->capacity;
    Node **old_items = map->items;

    map->size = 0;
    map->capacity *= 2;
    map->threshold = (size_t)(map->capacity * map->load_factor);
    map->items = calloc_(map->capacity, sizeof(Node *));
    if (!map->items) {
        return;
    }

    for (size_t i = 0; i < old_cap; i++) {
        Node *node = old_items[i];
        if (node && node != &map->tombstone) {
            uint64_t index = getHash_(map, node->key, map->capacity, 0);

            for (size_t j = 1;
                 map->items[index] && map->items[index] != &map->tombstone;
                 j++) {
                index = getHash_(map, node->key, map->capacity, j);
            }

            map->items[index] = node;
            map->size++;
        }
    }

    free_(old_items);
}

HashMap *hashmapNew(HashMapArgs *args) {
    if (!args || !args->capacity || !args->keySize || !args->cmp) {
        return NULL;
    }

    HashMap *map = malloc_(sizeof(HashMap));
    map->size = 0;
    map->capacity = args->capacity;
    if (args->load_factor) {
        map->load_factor = args->load_factor;
    } else {
        map->load_factor = 0.75;
    }
    map->threshold = (size_t)(map->capacity * map->load_factor);
    map->items = calloc_(args->capacity, sizeof(Node *));
    // map->nodeAlloc = args->nodeAlloc;
    // map->nodeFree = args->nodeFree;
    map->cmp = args->cmp;
    map->keySize = args->keySize;
    map->print = args->print;
    map->tombstone.key = NULL;
    map->tombstone.val = NULL;

    return map;
}

void hashmapSet(HashMap *map, const void *key, const void *val) {
    if (!map || !key || !val) {
        return;
    }

    if (map->threshold <= map->size) {
        hashmapResize_(map);
    }

    uint64_t index = getHash_(map, key, map->capacity, 0);
    uint64_t first_deleted = map->capacity;

    for (size_t i = 0; map->items[index]; i++) {
        if (map->items[index] == &map->tombstone &&
            first_deleted == map->capacity) {
            first_deleted = index;
        }

        if (map->items[index] != &map->tombstone &&
            map->cmp(map->items[index]->key, key) == 0) {
            map->items[index]->val = (void *)val;
            return;
        }

        index = getHash_(map, key, map->capacity, i + 1);
    }

    if (first_deleted != map->capacity) {
        index = first_deleted;
    }

    Node *node = nodeNew_(key, val);
    map->items[index] = node;
    map->size++;
}

void *hashmapGet(const HashMap *map, const void *key) {
    if (!map || !key) {
        return NULL;
    }

    uint64_t index = getHash_((void *)map, key, map->capacity, 0);

    for (size_t i = 0; i < map->capacity; i++) {
        Node *node = map->items[index];
        if (!node) {
            return NULL;
        }

        if (node != &map->tombstone && map->cmp(node->key, key) == 0) {
            return node->val;
        }

        index = getHash_((void *)map, key, map->capacity, i + 1);
    }

    return NULL;
}

void hashmapDel(HashMap *map, const void *key) {
    if (!map || !key) {
        return;
    }

    uint64_t index = getHash_(map, key, map->capacity, 0);

    for (size_t i = 0; i < map->capacity; i++) {
        Node *node = map->items[index];
        if (!node) {
            return;
        }

        if (node != &map->tombstone && map->cmp(node->key, key) == 0) {
            nodeFree_(node);
            map->items[index] = &map->tombstone;
            map->size--;
            return;
        }

        index = getHash_(map, key, map->capacity, i + 1);
    }
}

void hashmapFree(HashMap *map) {
    if (!map) {
        return;
    }

    for (size_t i = 0; i < map->capacity; i++) {
        Node *node = map->items[i];
        if (node && node != &map->tombstone) {
            nodeFree_(node);
        }
    }

    free_(map->items);
    free_(map);
}

void hashmapPrint(HashMap *map) {
    if (!map || !map->print) {
        return;
    }

    printf("HashMap (size: %zu, capacity: %zu, load_factor: %.2f)\n",
           map->size,
           map->capacity,
           map->load_factor);
    for (size_t i = 0; i < map->capacity; i++) {
        Node *node = map->items[i];
        printf("[%zu] ", i);
        if (!node) {
            puts("empty");
        } else if (node == &map->tombstone) {
            puts("tomb");
        } else {
            map->print(node->key, node->val);
        }
    }
}

#endif // HASHMAP_IMPLEMENTATION

#undef malloc_
#undef calloc_
#undef realloc_
#undef free_

#ifdef __cplusplus
}
#endif

#endif
