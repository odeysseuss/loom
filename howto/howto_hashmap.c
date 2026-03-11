#define HASHMAP_IMPLEMENTATION
#include <stdio.h>
#include <string.h>
#include "hashmap.h"

typedef struct {
    char *name;
    char *movie;
    float rating;
} Actor;

size_t keysize(const void *key) {
    return strlen(key);
}

int compare(const void *a, const void *b) {
    return strcmp(a, b);
}

void display(const void *key, const void *val) {
    const Actor *b2 = (const Actor *)val;
    printf("%s: { .name: %s, .show: %s, .rating: %.1f }\n",
           (char *)key,
           (char *)b2->name,
           (char *)b2->movie,
           (float)b2->rating);
}

/// only for gcc and clang
void displayWithCC(const void *key, const void *val) {
    const Actor *b2 = (const Actor *)val;
    printf("key: %s\n", (char *)key);
    __builtin_dump_struct(b2, &printf);
}

int main(void) {
    Actor a1 = {"Aragorn", "Lord of the Rings", 10.0};
    Actor a2 = {"Tyler", "Fight CLub", 9.8};
    Actor a3 = {"Rorschach", "The Watchman", 8.2};

    HashMap *map = hashmapNew(&(HashMapArgs){
        .capacity = 4,
        .keySize = keysize,
        .cmp = compare,
        .print = displayWithCC,
    });
    if (!map) {
        return 1;
    }

    hashmapSet(map, "Viggo", &a1);
    hashmapSet(map, "Brad", &a2);
    hashmapSet(map, "Earle", &a3);
    hashmapPrint(map);
    // duplicate keys are overridden but they can trigger resize
    hashmapSet(map, "Earle", &a3);
    hashmapPrint(map);

    hashmapDel(map, "Earle");
    /// should be nill
    printf("Earle: %p\n", hashmapGet(map, "Earle"));

    hashmapFree(map);

    return 0;
}
