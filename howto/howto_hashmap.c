#define HASHMAP_IMPLEMENTATION
#include <stdio.h>
#include <string.h>
#include "hashmap.h"

int compare(const void *str1, const void *str2) {
    return strcmp(str1, str2);
}

int main(void) {
    HashMapArgs args = {
        .capacity = 3,
        .nodeCmp = compare,
    };

    HashMap *map = hashmapNew(&args);
    hashmapSet(map, "robb", "loomstreet");
    hashmapSet(map, "gandalf", "the grey");
    hashmapSet(map, "jaime", "lannister");
    hashmapSet(map, "king", "aragorn");

    printf("robb: %s\n", (char *)hashmapGet(map, "robb"));
    hashmapDel(map, "robb");

    printf("king: %s\n", (char *)hashmapGet(map, "king"));
    printf("gandalf: %s\n", (char *)hashmapGet(map, "gandalf"));
    printf("robb: %s\n", (char *)hashmapGet(map, "robb"));
    printf("jaime: %s\n", (char *)hashmapGet(map, "jaime"));

    hashmapFree(map);

    return 0;
}
