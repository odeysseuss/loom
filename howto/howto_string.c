#define STRING_IMPLEMENTATION
#include "str.h"
#include <stdio.h>

int main(void) {
    String s = strNew("Gandalf the grey");
    printf("%s\n", s);
    strFree(s);
}
