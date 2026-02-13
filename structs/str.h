/*
* SDS style string library
*/

#ifndef STR_H
#define STR_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef char *String;

String strNew(const char *s);
String strNewLen(const void *s, int16_t len);
void strFree(String s);

// #ifdef STRING_IMPLEMENTATION

// header for String type
// used as a binary perfix that's stored before the actual String type
typedef struct {
    int16_t len_;
    int16_t alloc_;
} StrHdr_;

// get pointer to header from String
static inline StrHdr_ *getStrHdr_(const String s) {
    return ((StrHdr_ *)s - 1);
}

// get available memory space from String
static inline int16_t getStrAvail_(const String s) {
    return (getStrHdr_(s)->alloc_ - getStrHdr_(s)->len_);
}

static inline int16_t getStrLen_(const String s) {
    return (getStrHdr_(s)->len_);
}

static inline int16_t getStrAlloc_(const String s) {
    return (getStrHdr_(s)->alloc_);
}

String strNewLen(const void *s, int16_t len) {
    StrHdr_ *hdr = (StrHdr_ *)malloc(sizeof(StrHdr_) + len + 1);
    if (!hdr) {
        return NULL;
    }

    hdr->len_ = len;
    hdr->alloc_ = len;

    String str = (String)(hdr + 1);
    if (s && len > 0) {
        memcpy(str, s, len);
    }
    str[len] = '\0';

    return str;
}

String strNew(const char *s) {
    if (!s) {
        return NULL;
    }

    return strNewLen(s, strlen(s));
}

void strFree(String s) {
    if (!s) {
        return;
    }

    StrHdr_ *hdr = getStrHdr_(s);
    free(hdr);
}
// #endif

#ifdef __cplusplus
}
#endif

#endif
