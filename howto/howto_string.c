#define STRING_IMPLEMENTATION
#include "str.h"
#include <stdio.h>

int main(void) {
    String s = strNew("Gandalf the grey");
    String s2 = strNew("Gandalf the grey");
    if (strCmp(s, s2) == 0) {
        printf("both are: %s\n", s);
    }
    printf("s2 len: %zu\n", strLen(s2));
    strFree(s);
    strFree(s2);
}
