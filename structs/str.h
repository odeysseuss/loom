#ifndef STR_H
#define STR_H

#ifdef __cplusplus
extern "C" {
#endif

int add(int a, int b);

#ifdef STRING_IMPLEMENTATION
int add(int a, int b) {
    return a + b;
}
#endif

#ifdef __cplusplus
}
#endif

#endif
