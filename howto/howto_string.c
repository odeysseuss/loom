#define STRING_IMPLEMENTATION
#include "str.h"
#include <stdio.h>

int main(void) {
    String s = strNew("Gandalf the Grey");
    String s2 = strDup(s);
    if (strCmp(s, s2) == 0) {
        printf("%s == %s\n", s, s2);
    }
    String s3 = strSlice(s, 0, 7);
    printf("sliced: %s\n", s3);
    String s4 = strEmpty();

    s4 = strCatCStr(s4, "Ned");
    s4 = strCatFmt(s4, "%s", " Stark");
    printf("%s\n", s4);
    printf("alloc: %zu\n", getStrAlloc_(s4));
    s4 = strReserve(s4, 1000);
    printf("after reserve: %zu\n", getStrAlloc_(s4));
    s4 = strTrim(s4);
    printf("after trim: %zu\n", getStrAlloc_(s4));
    printf("after clear: %s\n", strClear(s4));

    strFree(s);
    strFree(s2);
    strFree(s3);
    strFree(s4);
}
