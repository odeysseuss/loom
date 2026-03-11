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
    printf("first index of a: %zu\n", strFindLen(s3, "a", 1));
    printf("last index of a: %zu\n", strFindLastLen(s3, "a", 1));

    String s4 = strEmpty();
    s4 = strCatCStr(s4, "Ned");
    s4 = strCatFmt(s4, "%s", " Stark");
    printf("%s\n", s4);
    printf("capacity: %zu\n", getStrAlloc_(s4));
    s4 = strTrim(s4);
    printf("after trim: %zu\n", getStrAlloc_(s4));
    printf("after clear: %s\n", strClear(s4));

    String s5 = strNew("aragorn, glorfindel, fingolfin, boromir");
    size_t count = 0;
    String *tokens = strSplitLen(s5, ", ", 2, &count);
    for (size_t i = 0; i < count; i++) {
        printf("Token-%zu: %s\n", i, tokens[i]);
    }
    String s6 = strJoinLen(tokens, count, ", ", 2);
    printf("%s\n", s6);
    printf("%s\n", strReplaceLen(s5, "o", 1, "0", 1));

    strSplitResFree(tokens, count);
    strFree(s);
    strFree(s2);
    strFree(s3);
    strFree(s4);
    strFree(s5);
    strFree(s6);
}
