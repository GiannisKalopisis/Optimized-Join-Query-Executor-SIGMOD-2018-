#ifndef HASH_FUNCTIONS_H
#define HASH_FUNCTIONS_H

#include <stdint.h>

#define H1BITS 7
#define H1BUCKETS ( 1 << H1BITS ) // 2 ^ H1BITS
#define H2BITS 6
#define H2BUCKETS ( 1 << H2BITS )

#define HASH_FUNCTION_1(value) (  ( value  << (64 - H1BITS) ) >> (64 - H1BITS) )

//#define HASH_FUNCTION_2(value) (  ( value  << (64 - H2BITS) ) >> (64 - H2BITS) )


// Hash Function 2 inspired by this blog post
// "https://nullprogram.com/blog/2018/07/31/"
static inline uint64_t HASH_FUNCTION_2 ( uint64_t value ) {

    value += 0x9e3779b97f4a7c15;
    value = (value ^ (value >> 30)) * 0xbf58476d1ce4e5b9;
    value = (value ^ (value >> 27)) * 0x94d049bb133111eb;
    value = value ^ (value >> 31);
    return (value >> (64 - H2BITS));
}

// Some other good hash functions

/*

value ^= value >> 16;
value *= UINT32_C(0x85ebca6b);
value ^= value >> 13;
value *= UINT32_C(0xc2b2ae35);
value ^= value >> 16;
return ( value >> (64 - H2BITS) );

 */

/*

value = ((value >> 16) ^ value) * UINT32_C(0x45d9f3b);
value = ((value >> 16) ^ value) * UINT32_C(0x45d9f3b);
value = (value >> 16) ^ value;
return ( value >> (64 - H2BITS) );

 */

/*
value ^= value >> 16;
value *= UINT32_C(0x7feb352d);
value ^= value >> 15;
value *= UINT32_C(0x846ca68b);
value ^= value >> 16;
return ( value >> (64 - H2BITS) );
 */

/*
value ^= value >> 32;
value *= UINT64_C(0xd6e8feb86659fd93);
value ^= value >> 32;
value *= UINT64_C(0xd6e8feb86659fd93);
value ^= value >> 32;
return ( value >> (64 - H2BITS) );
 */

#endif
