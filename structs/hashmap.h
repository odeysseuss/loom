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
*   - Add node{Alloc, Free, Cmp}
*   - Resizable hashmap
*   - Benchmark
*/
#ifndef HASHMAP_H
#define HASHMAP_H

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
    void *value;
} Node;

typedef struct {
    void *(*nodeAlloc)(size_t n);
    void (*nodeFree)(void *node);
    int (*nodeCmp)(const void *a, const void *b); // returns 0 if equal
    size_t size;
    size_t capacity;
    size_t key_size;
    Node **items;
} HashMap;

typedef struct {
    void *(*nodeAlloc)(size_t n);
    void (*nodeFree)(void *node);
    int (*nodeCmp)(const void *a, const void *b);
    size_t capacity;
    size_t key_size;
} HashMapArgs;

HashMap *hashmapNew(HashMapArgs *args);
void hashmapSet(HashMap *map, const void *key, const void *value);
void *hashmapGet(const HashMap *map, const void *key);
void hashmapDel(HashMap *map, const void *key);
void hashmapFree(HashMap *map);

#define malloc_ malloc
#define calloc_ calloc
#define realloc_ realloc
#define free_ free

#ifdef HASHMAP_IMPLEMENTATION

static Node deleted_item_ = {NULL, NULL};

static uint64_t getHash_(const void *s,
                         const size_t size,
                         const size_t capacity,
                         const size_t attempt) {
    const uint64_t hash_a = xxh3(s, size, 0);
    const uint64_t hash_b = xxh3(s, size, 1);

    return (hash_a + (attempt * (hash_b + 1))) % capacity;
}

static Node *nodeNew_(const void *key, const void *value) {
    Node *node = malloc_(sizeof(Node));
    node->key = key;
    node->value = value;

    return node;
}

static void nodeFree_(Node *node) {
    free_(node);
}

HashMap *hashmapNew(HashMapArgs *args) {
    HashMap *map = malloc_(sizeof(HashMap));
    map->size = 0;
    map->capacity = args->capacity;
    map->key_size = args->key_size;
    map->items = calloc_(args->capacity, sizeof(Node *));
    map->nodeAlloc = args->nodeAlloc;
    map->nodeFree = args->nodeFree;
    map->nodeCmp = args->nodeCmp;

    return map;
}

void hashmapSet(HashMap *map, const void *key, const void *value) {
    Node *node = nodeNew_(key, value);
    uint64_t index = getHash_(key, map->key_size, map->capacity, 0);
    Node *curr_node = map->items[index];

    for (size_t i = 1; curr_node != NULL && curr_node != &deleted_item_; i++) {
        index = getHash_(key, map->key_size, map->capacity, i);
        curr_node = map->items[index];

        if (map->nodeCmp(node->key, key) == 0) {
            nodeFree_(curr_node);
            map->items[index] = node;
            return;
        }
    }

    map->items[index] = node;
    map->size++;
}

void *hashmapGet(const HashMap *map, const void *key) {
    uint64_t index = getHash_(key, map->key_size, map->capacity, 0);
    Node *node = map->items[index];

    for (size_t i = 0; node != NULL; i++) {
        if (node != &deleted_item_) {
            if (map->nodeCmp(node->key, key) == 0) {
                return node->value;
            }
        }

        index = getHash_(key, map->key_size, map->capacity, i);
        node = map->items[index];
    }

    return NULL;
}

void hashmapDel(HashMap *map, const void *key) {
    uint64_t index = getHash_(key, map->key_size, map->capacity, 0);
    Node *node = map->items[index];

    for (size_t i = 0; node != NULL; i++) {
        if (node != &deleted_item_) {
            if (map->nodeCmp(node->key, key) == 0) {
                nodeFree_(node);
                map->items[index] = &deleted_item_;
            }
        }

        index = getHash_(key, map->key_size, map->capacity, i);
        node = map->items[index];
    }

    map->size--;
}

void hashmapFree(HashMap *map) {
    for (size_t i = 0; i < map->capacity; i++) {
        Node *node = map->items[i];
        if (node != NULL) {
            nodeFree_(node);
        }
    }

    free_(map->items);
    free_(map);
}

#endif // HASHMAP_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif
