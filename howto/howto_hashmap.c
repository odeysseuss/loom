#define HASHMAP_IMPLEMENTATION
#include <stdio.h>
#include <string.h>
#include "hashmap.h"

size_t keysize(const void *str) {
    return strlen(str);
}

int compare(const void *str1, const void *str2) {
    return strcmp(str1, str2);
}

void display(const void *key, const void *val) {
    printf("%s: %s\n", (char *)key, (char *)val);
}

int main(void) {
    HashMapArgs args = {
        .capacity = 4,
        .keySize = keysize,
        .cmp = compare,
        .print = display,
    };

    HashMap *map = hashmapNew(&args);
    if (!map) {
        return 1;
    }

    hashmapSet(map, "robb", "loomstreet");
    hashmapSet(map, "king", "aragorn");
    hashmapSet(map, "gandalf", "the grey");

    hashmapDel(map, "robb");
    hashmapPrint(map);

    hashmapSet(map, "jaime", "lannister");
    hashmapSet(map, "jaime", "lannister");
    hashmapSet(map, "frodo", "shire");
    hashmapSet(map, "sam", "shire");
    hashmapSet(map, "fingolfin", "silmarillion");
    hashmapPrint(map);

    hashmapFree(map);

    return 0;
}
