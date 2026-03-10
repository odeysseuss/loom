#define HASHMAP_IMPLEMENTATION
#include <stdio.h>
#include <string.h>
#include "hashmap.h"

int compare(const void *str1, const void *str2) {
    return strcmp(str1, str2);
}

void display(const void *key, const void *val) {
    printf("%s: %s\n", (char *)key, (char *)val);
}

int main(void) {
    HashMapArgs args = {
        .capacity = 4,
        .nodeCmp = compare,
        .nodeDisplay = display,
    };

    HashMap *map = hashmapNew(&args);
    hashmapSet(map, "robb", "loomstreet");
    hashmapSet(map, "gandalf", "the grey");
    hashmapSet(map, "jaime", "lannister");
    hashmapSet(map, "king", "aragorn");

    hashmapDel(map, "robb");
    hashmapPrint(map);

    hashmapSet(map, "frodo", "shire");
    hashmapSet(map, "sam", "shire");
    hashmapPrint(map);

    hashmapFree(map);

    return 0;
}
