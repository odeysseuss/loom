/*
* Hashmap implementation with xxhash and double hashing for collision hasndling
*
* NOTE: To use this library define the following macro in EXACTLY
* ONE FILE BEFORE inlcuding hashmap.h:
*   #define HASHMAP_IMPLEMENTATION
*   #include "hashmap.h"
*
* DEPENDENCIES: structs/xxhash.h
* TODO:
*   - Allow dynamic alloc and free
*   - Resizable hashmap
*   - Benchmark
*/
#ifndef HASHMAP_H
#define HASHMAP_H

#include <netdb.h>
#include <stdio.h>
#define XXHASH_IMPLEMENTATION
#include "xxhash.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *key;
    void *val;
} Node;

typedef struct {
    void *(*nodeAlloc)(size_t n);
    void (*nodeFree)(void *node);
    int (*nodeCmp)(const void *a, const void *b); // returns 0 if equal
    size_t size;
    size_t capacity;
    size_t key_size;
    // size_t val_size;
    float load_factor;
    size_t threshold;
    Node **items;
} HashMap;

typedef struct {
    void *(*nodeAlloc)(size_t n);
    void (*nodeFree)(void *node);
    int (*nodeCmp)(const void *a, const void *b);
    size_t capacity;
    size_t key_size;
    // size_t val_size;
    float load_factor;
} HashMapArgs;

HashMap *hashmapNew(HashMapArgs *args);
void hashmapSet(HashMap *map, const void *key, const void *val);
void *hashmapGet(const HashMap *map, const void *key);
void hashmapDel(HashMap *map, const void *key);
void hashmapFree(HashMap *map);

#define malloc_ malloc
#define calloc_ calloc
#define realloc_ realloc
#define free_ free

// #ifdef HASHMAP_IMPLEMENTATION

static Node deleted_item_ = {NULL, NULL};

static inline uint64_t getHash_(const void *s,
                                const size_t size,
                                const size_t capacity,
                                const size_t attempt) {
    const uint64_t hash_a = xxh3(s, size, 0);
    const uint64_t hash_b = xxh3(s, size, 1);

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

    map->size = 0; // update through rehashing
    map->capacity *= 2;
    map->threshold = (size_t)(map->capacity * map->load_factor);
    map->items = calloc_(map->capacity, sizeof(Node *));
    if (!map->items) {
        return;
    }

    for (size_t i = 0; i < old_cap; i++) {
        Node *node = old_items[i];
        if (node && node != &deleted_item_) {
            uint64_t index =
                getHash_(node->key, map->key_size, map->capacity, 0);

            for (size_t j = 1;
                 map->items[index] && map->items[index] != &deleted_item_;
                 j++) {
                index = getHash_(node->key, map->key_size, map->capacity, j);
            }

            map->items[index] = node;
            map->size++;
        }
    }

    free_(old_items);
}

HashMap *hashmapNew(HashMapArgs *args) {
    HashMap *map = malloc_(sizeof(HashMap));
    map->size = 0;
    map->capacity = args->capacity;
    map->key_size = args->key_size;
    if (args->load_factor) {
        map->load_factor = args->load_factor;
    } else {
        map->load_factor = 0.75;
    }
    map->threshold = (size_t)(map->capacity * map->load_factor);
    map->items = calloc_(args->capacity, sizeof(Node *));
    map->nodeAlloc = args->nodeAlloc;
    map->nodeFree = args->nodeFree;
    map->nodeCmp = args->nodeCmp;

    return map;
}

void hashmapSet(HashMap *map, const void *key, const void *val) {
    if (map->threshold <= map->size) {
        hashmapResize_(map);
    }

    uint64_t index = getHash_(key, map->key_size, map->capacity, 0);
    Node *curr_node = map->items[index];

    for (size_t i = 1; curr_node && curr_node != &deleted_item_; i++) {
        if (map->nodeCmp(curr_node->key, key) == 0) {
            curr_node->val = (void *)val;
            return;
        }

        index = getHash_(key, map->key_size, map->capacity, i);
        curr_node = map->items[index];
    }

    Node *node = nodeNew_(key, val);
    map->items[index] = node;
    map->size++;
}

void *hashmapGet(const HashMap *map, const void *key) {
    uint64_t index = getHash_(key, map->key_size, map->capacity, 0);

    for (size_t i = 0; i < map->capacity; i++) {
        Node *node = map->items[index];
        if (!node) {
            return NULL;
        }

        if (node != &deleted_item_ && map->nodeCmp(node->key, key) == 0) {
            return node->val;
        }

        index = getHash_(key, map->key_size, map->capacity, i);
    }

    return NULL;
}

void hashmapDel(HashMap *map, const void *key) {
    uint64_t index = getHash_(key, map->key_size, map->capacity, 0);

    for (size_t i = 0; i <= map->capacity; i++) {
        Node *node = map->items[index];
        if (!node) {
            return;
        }

        if (node != &deleted_item_ && map->nodeCmp(node->key, key) == 0) {
            nodeFree_(node);
            map->items[index] = &deleted_item_;
            map->size--;
            return;
        }

        index = getHash_(key, map->key_size, map->capacity, i);
    }
}

void hashmapFree(HashMap *map) {
    for (size_t i = 0; i < map->capacity; i++) {
        Node *node = map->items[i];
        if (node != NULL && node != &deleted_item_) {
            nodeFree_(node);
        }
    }

    free_(map->items);
    free_(map);
}

// #endif // HASHMAP_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif
