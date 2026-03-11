/*
* SDS style binary safe string library
* REFERENCE: This library is designed similarly as [antirez/sds](https://github.com/antirez/sds)
*
* USAGE:
*   #define STRING_IMPLEMENTATION
*   #include "str.h"
*/

#ifndef STR_H
#define STR_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define malloc_ malloc
#define calloc_ calloc
#define realloc_ realloc
#define free_ free

typedef char *String;

/// Constructors
String strNewLen(const void *s, size_t len);
String strNew(const char *cstr);
String strEmpty(void);
String strDup(const String s);
String strSlice(const String s, size_t start, size_t end);
/// Get string length
size_t strLen(const String s);
/// Comparison of strings
int strCmp(const String s1, const String s2);
/// Grow or trim strings
String strGrow(const String s, size_t addlen);
String strReserve(const String s, size_t addlen);
String strTrim(String s);
/// Concatinating strings
String strCatLen(String dest, const void *src, size_t len);
String strCatCStr(String dest, const char *src);
String strCat(String dest, const String src);
String strCatFmt(String s1, const char *fmt, ...);
/// Finding substrings
size_t strFindLen(const String s, const void *t, size_t len);
size_t strFindLastLen(const String s, const void *t, size_t len);
size_t strFirst(const String s, const char *cstr);
size_t strFindLast(const String s, const char *cstr);
/// Spitting and joining strings
String *strSplitLen(const String s, const void *sep, size_t seplen, int *count);
String *strSplit(const String s, const char *sep, int *count);
void strSplitResFree(String *toks, int count);
String strJoinLen(String *toks, int count, const void *sep, size_t seplen);
String strJoin(String *toks, int count, const char *sep);
/// Replace substrings
String strReplaceLen(const String s,
                     const void *from,
                     size_t from_len,
                     const void *to,
                     size_t to_len,
                     size_t len);
String strReplace(const String s, const char *from, const char *to, size_t len);
/// String casing
String strToLower(String s);
String strToUpper(String s);
/// Clear without freeing strings
String strClear(String s);
/// Destructors
void strFree(String s);

// #ifdef STRING_IMPLEMENTATION

// header for String type
// used as a binary perfix that's stored before the actual String type
typedef struct {
    size_t len_;
    size_t alloc_;
} StrHdr_;

// get pointer to header from String
static inline StrHdr_ *getStrHdr_(const String s) {
    return ((StrHdr_ *)s - 1);
}

// get available memory space from String
static inline size_t getStrAvail_(const String s) {
    return (getStrHdr_(s)->alloc_ - getStrHdr_(s)->len_);
}

static inline size_t getStrLen_(const String s) {
    return (getStrHdr_(s)->len_);
}

static inline size_t getStrAlloc_(const String s) {
    return (getStrHdr_(s)->alloc_);
}

String strNewLen(const void *s, const size_t len) {
    if (!s) {
        return NULL;
    }

    StrHdr_ *hdr = (StrHdr_ *)malloc_(sizeof(StrHdr_) + len + 1);
    hdr->len_ = len;
    hdr->alloc_ = len;

    String str = (String)(hdr + 1);
    if (s && len > 0) {
        memcpy(str, s, len);
    }
    str[len] = '\0';

    return str;
}

String strNew(const char *cstr) {
    return strNewLen(cstr, strlen(cstr));
}

String strEmpty(void) {
    return strNewLen("", 0);
}

size_t strLen(const String s) {
    if (!s) {
        return -1;
    }

    return getStrLen_(s);
}

int strCmp(const String s1, const String s2) {
    if (!s1 || !s2) {
        return -1;
    }

    size_t s1_len = getStrLen_(s1);
    size_t s2_len = getStrLen_(s2);
    size_t min_len = s1_len < s2_len ? s1_len : s2_len;

    int cmp = memcmp(s1, s2, min_len);
    if (cmp != 0) {
        return cmp;
    }

    return (s1_len > s2_len) - (s1_len < s2_len);
}

String strDup(const String s) {
    return strNewLen(s, getStrLen_(s));
}

String strSlice(const String s, size_t start, size_t end) {
    if (!s) {
        return NULL;
    }

    size_t new_len = end - start;
    return strNewLen(s + start, new_len);
}

String strGrow(const String s, size_t addlen) {
    if (!s || addlen == 0) {
        return NULL;
    }

    StrHdr_ *hdr = getStrHdr_(s);
    size_t curr_len = hdr->len_;
    size_t new_alloc = hdr->alloc_ + addlen;
    StrHdr_ *new_hdr = realloc_(hdr, sizeof(StrHdr_) + new_alloc + 1);
    new_hdr->alloc_ = new_alloc;
    new_hdr->len_ = curr_len;

    return (String)(new_hdr + 1);
}

String strReserve(const String s, size_t capacity) {
    if (!s) {
        return NULL;
    }

    StrHdr_ *hdr = getStrHdr_(s);
    size_t curr_len = hdr->len_;
    StrHdr_ *new_hdr = realloc_(hdr, sizeof(StrHdr_) + capacity + 1);
    new_hdr->alloc_ = capacity;
    new_hdr->len_ = curr_len;

    return (String)(new_hdr + 1);
}

String strTrim(String s) {
    if (!s) {
        return NULL;
    }

    StrHdr_ *hdr = getStrHdr_(s);
    if (getStrAvail_(s) == 0) {
        return s;
    }

    size_t new_alloc = hdr->len_;
    StrHdr_ *new_hdr = realloc_(hdr, sizeof(StrHdr_) + new_alloc + 1);
    new_hdr->alloc_ = new_alloc;
    new_hdr->len_ = new_alloc;

    return (String)(new_hdr + 1);
}

String strCatLen(String dest, const void *src, size_t len) {
    if (!dest || !src || len == 0) {
        return NULL;
    }

    size_t curr_len = getStrLen_(dest);
    if (getStrAvail_(dest) < len) {
        dest = strGrow(dest, len);
    }

    memcpy(dest + curr_len, src, len);
    StrHdr_ *hdr = getStrHdr_(dest);
    hdr->len_ = curr_len + len;
    dest[hdr->len_] = '\0';

    return dest;
}

String strCatCStr(String dest, const char *src) {
    return strCatLen(dest, src, strlen(src));
}

String strCat(String dest, const String src) {
    return strCatLen(dest, src, getStrLen_(src));
}

String strCatFmt(String dest, const char *fmt, ...) {
    if (!dest || !fmt) {
        return NULL;
    }

    va_list ap;
    va_start(ap, fmt);
    int needed = vsnprintf(NULL, 0, fmt, ap);
    if (needed < 0) {
        return NULL;
    }
    va_end(ap);

    if (getStrAvail_(dest) < (size_t)needed) {
        dest = strGrow(dest, needed);
    }

    StrHdr_ *hdr = getStrHdr_(dest);
    size_t len = getStrLen_(dest);
    va_start(ap, fmt);
    int written = vsnprintf(dest + len, needed + 1, fmt, ap);
    if (written < 0) {
        return NULL;
    }
    va_end(ap);

    hdr->len_ = len + written;
    return dest;
}

size_t strFindLen(const String s, const void *t, size_t len);
size_t strFindLastLen(const String s, const void *t, size_t len);
size_t strFind(const String s, const char *cstr);
size_t strFindLast(const String s, const char *cstr);

String strClear(String s) {
    if (!s) {
        return NULL;
    }

    StrHdr_ *hdr = getStrHdr_(s);
    hdr->len_ = 0;
    s[hdr->len_] = '\0';
}

String *strSplitLen(const String s, const void *sep, size_t seplen, int *count);
String *strSplit(const String s, const char *sep, int *count);
void strSplitResFree(String *toks, int count);
String strJoinLen(String *toks, int count, const void *sep, size_t seplen);
String strJoin(String *toks, int count, const char *sep);
String strReplaceLen(const String s,
                     const void *from,
                     size_t from_len,
                     const void *to,
                     size_t to_len,
                     size_t len);
String strReplace(const String s, const char *from, const char *to, size_t len);
String strToLower(String s);
String strToUpper(String s);

void strFree(String s) {
    if (!s) {
        return;
    }

    StrHdr_ *hdr = getStrHdr_(s);
    free(hdr);
}

// #endif // STRING_IMPLEMENTATION

#undef malloc_
#undef calloc_
#undef realloc_
#undef free_

#ifdef __cplusplus
}
#endif

#endif
