/*
* Hashmap implementation with xxhash and double hashing for collision hasndling
*
* NOTE: To use this library define the following macro in EXACTLY
* ONE FILE BEFORE inlcuding hashmap.h:
*   #define HASHMAP_IMPLEMENTATION
*   #include "hashmap.h"
*
* DEPENDENCIES: structs/xxhash.h
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

typedef struct {
    void *key;
    void *val;
} Node;

typedef struct {
    // void *(*nodeAlloc)(size_t n);
    // void (*nodeFree)(void *node);
    int (*nodeCmp)(const void *a, const void *b); // returns 0 if equal
    void (*nodeDisplay)(const void *a, const void *b);
    size_t size;
    size_t capacity;
    size_t key_size;
    // size_t val_size;
    float load_factor;
    size_t threshold;
    Node **items;
    Node tombstone;
} HashMap;

typedef struct {
    // void *(*nodeAlloc)(size_t n);
    // void (*nodeFree)(void *node);
    int (*nodeCmp)(const void *a, const void *b);
    void (*nodeDisplay)(const void *a, const void *b);
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
void hashmapPrint(HashMap *map);

#define malloc_ malloc
#define calloc_ calloc
#define realloc_ realloc
#define free_ free

#ifdef HASHMAP_IMPLEMENTATION

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
        if (node && node != &map->tombstone) {
            uint64_t index =
                getHash_(node->key, map->key_size, map->capacity, 0);

            for (size_t j = 1;
                 map->items[index] && map->items[index] != &map->tombstone;
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
    if (!args || !args->capacity || !args->nodeCmp) {
        return NULL;
    }

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
    // map->nodeAlloc = args->nodeAlloc;
    // map->nodeFree = args->nodeFree;
    map->nodeCmp = args->nodeCmp;
    map->nodeDisplay = args->nodeDisplay;
    map->tombstone.key = NULL;
    map->tombstone.val = NULL;

    return map;
}

void hashmapSet(HashMap *map, const void *key, const void *val) {
    if (map->threshold <= map->size) {
        hashmapResize_(map);
    }

    uint64_t index = getHash_(key, map->key_size, map->capacity, 0);
    uint64_t first_deleted = map->capacity;

    for (size_t i = 1; map->items[index]; i++) {
        if (map->items[index] == &map->tombstone &&
            first_deleted == map->capacity) {
            first_deleted = index;
        }

        if (map->items[index] != &map->tombstone &&
            map->nodeCmp(map->items[index]->key, key) == 0) {
            map->items[index]->val = (void *)val;
            return;
        }

        index = getHash_(key, map->key_size, map->capacity, i);
    }

    if (first_deleted != map->capacity) {
        index = first_deleted;
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

        if (node != &map->tombstone && map->nodeCmp(node->key, key) == 0) {
            return node->val;
        }

        index = getHash_(key, map->key_size, map->capacity, i + 1);
    }

    return NULL;
}

void hashmapDel(HashMap *map, const void *key) {
    uint64_t index = getHash_(key, map->key_size, map->capacity, 0);

    for (size_t i = 0; i < map->capacity; i++) {
        Node *node = map->items[index];
        if (!node) {
            return;
        }

        if (node != &map->tombstone && map->nodeCmp(node->key, key) == 0) {
            nodeFree_(node);
            map->items[index] = &map->tombstone;
            map->size--;
            return;
        }

        index = getHash_(key, map->key_size, map->capacity, i + 1);
    }
}

void hashmapFree(HashMap *map) {
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
    if (!map->nodeDisplay) {
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
            map->nodeDisplay(node->key, node->val);
        }
    }
}

#endif // HASHMAP_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif
