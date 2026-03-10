/*
*
* Helper for structs/hashmap.h
* Used as the hash function hashmap.h
*
*/
#ifndef XXHASH_H
#define XXHASH_H

#include <stdint.h>
#include <string.h>

#define XXH_PRIME_1 11400714785074694791ULL
#define XXH_PRIME_2 14029467366897019727ULL
#define XXH_PRIME_3 1609587929392839161ULL
#define XXH_PRIME_4 9650029242287828579ULL
#define XXH_PRIME_5 2870177450012600261ULL

uint64_t xxh3(const void *data, size_t len, uint64_t seed);

#ifdef XXHASH_IMPLEMENTATION

static inline uint64_t read64_(const void *memptr) {
    uint64_t val;
    memcpy(&val, memptr, sizeof(val));
    return val;
}

static inline uint32_t read32_(const void *memptr) {
    uint32_t val;
    memcpy(&val, memptr, sizeof(val));
    return val;
}

static inline uint64_t rotl64_(uint64_t x, int r) {
    return (x << r) | (x >> (64 - r));
}

uint64_t xxh3(const void *data, size_t len, uint64_t seed) {
    const uint8_t *p = (const uint8_t *)data;
    const uint8_t *const end = p + len;
    uint64_t h64;

    if (len >= 32) {
        const uint8_t *const limit = end - 32;
        uint64_t v1 = seed + XXH_PRIME_1 + XXH_PRIME_2;
        uint64_t v2 = seed + XXH_PRIME_2;
        uint64_t v3 = seed + 0;
        uint64_t v4 = seed - XXH_PRIME_1;

        do {
            v1 += read64_(p) * XXH_PRIME_2;
            v1 = rotl64_(v1, 31);
            v1 *= XXH_PRIME_1;

            v2 += read64_(p + 8) * XXH_PRIME_2;
            v2 = rotl64_(v2, 31);
            v2 *= XXH_PRIME_1;

            v3 += read64_(p + 16) * XXH_PRIME_2;
            v3 = rotl64_(v3, 31);
            v3 *= XXH_PRIME_1;

            v4 += read64_(p + 24) * XXH_PRIME_2;
            v4 = rotl64_(v4, 31);
            v4 *= XXH_PRIME_1;

            p += 32;
        } while (p <= limit);

        h64 =
            rotl64_(v1, 1) + rotl64_(v2, 7) + rotl64_(v3, 12) + rotl64_(v4, 18);

        v1 *= XXH_PRIME_2;
        v1 = rotl64_(v1, 31);
        v1 *= XXH_PRIME_1;
        h64 ^= v1;
        h64 = h64 * XXH_PRIME_1 + XXH_PRIME_4;

        v2 *= XXH_PRIME_2;
        v2 = rotl64_(v2, 31);
        v2 *= XXH_PRIME_1;
        h64 ^= v2;
        h64 = h64 * XXH_PRIME_1 + XXH_PRIME_4;

        v3 *= XXH_PRIME_2;
        v3 = rotl64_(v3, 31);
        v3 *= XXH_PRIME_1;
        h64 ^= v3;
        h64 = h64 * XXH_PRIME_1 + XXH_PRIME_4;

        v4 *= XXH_PRIME_2;
        v4 = rotl64_(v4, 31);
        v4 *= XXH_PRIME_1;
        h64 ^= v4;
        h64 = h64 * XXH_PRIME_1 + XXH_PRIME_4;
    } else {
        h64 = seed + XXH_PRIME_5;
    }

    h64 += (uint64_t)len;

    while (p + 8 <= end) {
        uint64_t k1 = read64_(p);
        k1 *= XXH_PRIME_2;
        k1 = rotl64_(k1, 31);
        k1 *= XXH_PRIME_1;
        h64 ^= k1;
        h64 = rotl64_(h64, 27) * XXH_PRIME_1 + XXH_PRIME_4;
        p += 8;
    }

    if (p + 4 <= end) {
        h64 ^= (uint64_t)(read32_(p)) * XXH_PRIME_1;
        h64 = rotl64_(h64, 23) * XXH_PRIME_2 + XXH_PRIME_3;
        p += 4;
    }

    while (p < end) {
        h64 ^= (*p) * XXH_PRIME_5;
        h64 = rotl64_(h64, 11) * XXH_PRIME_1;
        p++;
    }

    h64 ^= h64 >> 33;
    h64 *= XXH_PRIME_2;
    h64 ^= h64 >> 29;
    h64 *= XXH_PRIME_3;
    h64 ^= h64 >> 32;

    return h64;
}

#endif // XXHASH_IMPLEMENTATION

#endif
