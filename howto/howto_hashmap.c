#define HASHMAP_IMPLEMENTATION
#define STR_IMPLEMENTATION
#include "hashmap.h"
#include "str.h"
#include <stdio.h>

typedef struct {
    char *name;
    char *movie;
    float rating;
} Actor;

Actor *actorInit(char *name, char *movie, float rating) {
    Actor *actor = malloc(sizeof(Actor));
    actor->name = name;
    actor->movie = movie;
    actor->rating = rating;

    return actor;
}

void actorFree(void *actor) {
    free((Actor *)actor);
}

size_t keysize(const void *key) {
    return strLen((String)key);
}

int compare(const void *a, const void *b) {
    return strCmp((String)a, (String)b);
}

void display(const void *key, const void *val) {
    const Actor *b2 = (const Actor *)val;
    printf("key: %s\n", (String)key);
    __builtin_dump_struct(b2, &printf);
}

int main(void) {
    // object allocation is always maintained by user
    Actor *a1 = actorInit("Aragorn", "Lord of the Rings", 10.0);
    Actor *a2 = actorInit("Tyler", "Fight CLub", 9.8);
    Actor *a3 = actorInit("Rorschach", "The Watchman", 8.2);

    HashMap *map = hashmapNew(&(HashMapArgs){
        .capacity = 4,
        .keySize = keysize,
        .keyCmp = compare,
        // .keyFree = strFree, // if keyFree is not provided than key ownership belongs to the caller
        // by providing valFree, the ownership of value moves to the hashmap
        .valFree = actorFree,
        .print = display,
    });
    if (!map) {
        return 1;
    }

    // as keyFree is not provided s1, s2 and s3 must be freed explicitly
    String s1 = strNew("Viggo");
    String s2 = strNew("Brad");
    String s3 = strNew("Earle");

    hashmapSet(map, s1, a1);
    hashmapSet(map, s2, a2);
    hashmapSet(map, s3, a3);
    hashmapPrint(map);
    // duplicate keys are overridden but they can trigger resize
    hashmapSet(map, s3, a3);
    hashmapPrint(map);

    // strCmp takes requires both to be of type String, so, this won't work
    // for that you must use strCmpCStr as keyCmp
    // hashmapDel(map, "Earle");
    hashmapDel(map, s3);
    strFree(s3); // key should be freed explicitly

    strFree(s1);
    strFree(s2);
    hashmapFree(map);

    return 0;
}
