#define STR_IMPLEMENTATION
#include "str.h"
#include <stdio.h>

int main(void) {
    String s = strNew("Gandalf the Grey");
    String s2 = strDup(s);
    if (strCmp(s, s2) == 0) {
        printf("strNew: %s == strDup: %s\n", s, s2);
    }
    printf("strLen: %zu\n", strLen(s));
    String s3 = strSlice(s, 0, 7);
    printf("strSlice: %s\n", s3);
    printf("strFindLen: %zu\n", strFindLen(s3, "a", 1));
    printf("strFindLastLen: %zu\n", strFindLastLen(s3, "a", 1));

    String s4 = strEmpty();
    printf("strEmpty: %s\n", s4);
    s4 = strCatCStr(s4, "Ned"); // Must be reassigned before use
    printf("strCatCStr: %s\n", s4);
    s4 = strCatFmt(s4, " %s", "Stark"); // Must be reassigned before use
    printf("strCatFmt: %s\n", s4);
    printf("curr_alloc: %zu\n", getStrAlloc_(s4));
    s4 = strGrow(s4, 1000); // Must be reassigned before use
    printf("strGrow alloc: %zu\n", getStrAlloc_(s4));
    s4 = strTrim(s4); // Must be reassigned before use
    printf("strTrim alloc: %zu\n", getStrAlloc_(s4));
    printf("strClear: %s\n", strClear(s4));

    String s5 = strNew("aragorn, glorfindel, fingolfin, boromir");
    size_t count = 0;
    String *tokens = strSplitLen(s5, ", ", 2, &count);
    printf("strSplitResLen: \n");
    for (size_t i = 0; i < count; i++) {
        printf("[%zu]: %s\n", i, tokens[i]);
    }
    printf("strReplaceLen (o-0): %s\n", strReplaceLen(s5, "o", 1, "0", 1));
    String s6 = strJoinLen(tokens, count, ", ", 2);
    printf("strJoinLen: %s\n", s6);
    printf("strToUppper: %s\n", strToUpper(s6));

    strSplitResFree(tokens, count);
    strFree(s);
    strFree(s2);
    strFree(s3);
    strFree(s4);
    strFree(s5);
    strFree(s6);
}
