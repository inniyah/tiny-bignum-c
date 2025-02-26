// This is free and unencumbered software released into the public domain.
// 
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
// 
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
// 
// For more information, please refer to <https://unlicense.org>

#ifndef __BIGNUM_H__
#define __BIGNUM_H__
/*

Big number library - arithmetic on multiple-precision unsigned integers.

This library is an implementation of arithmetic on arbitrarily large integers.

The difference between this and other implementations, is that the data structure
has optimal memory utilization (i.e. a 1024 bit integer takes up 128 bytes RAM),
and all memory is allocated statically: no dynamic allocation for better or worse.

Primary goals are correctness, clarity of code and clean, portable implementation.
Secondary goal is a memory footprint small enough to make it suitable for use in
embedded applications.


The current state is correct functionality and adequate performance.
There may well be room for performance-optimizations and improvements.

*/

#include <stdint.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

/* This macro defines the word size in bytes of the array that constitues the big-number data structure. */
#ifndef WORD_SIZE
  #define WORD_SIZE 4
#endif

/* Size of big-numbers in bytes */
#ifndef BN_ARRAY_SIZE
  #define BN_ARRAY_SIZE (256 / WORD_SIZE)
#endif


/* Here comes the compile-time specialization for how large the underlying array size should be. */
/* The choices are 1, 2 and 4 bytes in size with uint32, uint64 for WORD_SIZE==4, as temporary. */
#ifndef WORD_SIZE
  #error Must define WORD_SIZE to be 1, 2, 4
#elif (WORD_SIZE == 1)
  /* Data type of array in structure */
  #define DTYPE                    uint8_t
  /* bitmask for getting MSB */
  #define DTYPE_MSB                ((DTYPE_TMP)(0x80))
  /* Data-type larger than DTYPE, for holding intermediate results of calculations */
  #define DTYPE_TMP                uint32_t
  /* sprintf format string */
  #define SPRINTF_FORMAT_STR       "%.02x"
  #define SSCANF_FORMAT_STR        "%2hhx"
  /* Max value of integer type */
  #define MAX_VAL                  ((DTYPE_TMP)0xFF)
#elif (WORD_SIZE == 2)
  #define DTYPE                    uint16_t
  #define DTYPE_TMP                uint32_t
  #define DTYPE_MSB                ((DTYPE_TMP)(0x8000))
  #define SPRINTF_FORMAT_STR       "%.04x"
  #define SSCANF_FORMAT_STR        "%4hx"
  #define MAX_VAL                  ((DTYPE_TMP)0xFFFF)
#elif (WORD_SIZE == 4)
  #define DTYPE                    uint32_t
  #define DTYPE_TMP                uint64_t
  #define DTYPE_MSB                ((DTYPE_TMP)(0x80000000))
  #define SPRINTF_FORMAT_STR       "%.08x"
  #define SSCANF_FORMAT_STR        "%8x"
  #define MAX_VAL                  ((DTYPE_TMP)0xFFFFFFFF)
#endif
#ifndef DTYPE
  #error DTYPE must be defined to uint8_t, uint16_t uint32_t or whatever
#endif


/* Custom assert macro - easy to disable */
#define require(p, msg) assert(p && msg)

/* Data-holding structure: array of DTYPEs */
struct bn {
  DTYPE array[BN_ARRAY_SIZE];
};

/* Tokens returned by bignum_cmp() for value comparison */
enum { SMALLER = -1, EQUAL = 0, LARGER = 1 };

/* Initialization functions: */
void bignum_init(struct bn* n);
void bignum_from_int(struct bn* n, DTYPE_TMP i);
int  bignum_to_int(struct bn* n);
void bignum_from_string(struct bn* n, const char* str, int nbytes);
void bignum_to_string(const struct bn* n, char* str, int maxsize);

/* Basic arithmetic operations: */
void bignum_add(const struct bn* a, const struct bn* b, struct bn* c); /* c = a + b */
void bignum_sub(const struct bn* a, const struct bn* b, struct bn* c); /* c = a - b */
void bignum_mul(const struct bn* a, const struct bn* b, struct bn* c); /* c = a * b */
void bignum_div(const struct bn* a, const struct bn* b, struct bn* c); /* c = a / b */
void bignum_mod(const struct bn* a, const struct bn* b, struct bn* c); /* c = a % b */
void bignum_divmod(const struct bn* a, const struct bn* b, struct bn* c, struct bn* d); /* c = a/b, d = a%b */

/* Bitwise operations: */
void bignum_and(const struct bn* a, const struct bn* b, struct bn* c); /* c = a & b */
void bignum_or(const struct bn* a, const struct bn* b, struct bn* c);  /* c = a | b */
void bignum_xor(const struct bn* a, const struct bn* b, struct bn* c); /* c = a ^ b */
void bignum_lshift(const struct bn* a, struct bn* b, int nbits); /* b = a << nbits */
void bignum_rshift(const struct bn* a, struct bn* b, int nbits); /* b = a >> nbits */

/* Special operators and comparison */
int  bignum_cmp(const struct bn* a, const struct bn* b);   /* Compare: returns LARGER, EQUAL or SMALLER */
int  bignum_is_zero(const struct bn* n);                   /* For comparison with zero */
void bignum_inc(struct bn* n);                             /* Increment: add one to n */
void bignum_dec(struct bn* n);                             /* Decrement: subtract one from n */
void bignum_pow(const struct bn* a, const struct bn* b, struct bn* c); /* Calculate a^b -- e.g. 2^10 => 1024 */
void bignum_isqrt(const struct bn* a, struct bn* b);             /* Integer square root -- e.g. isqrt(5) => 2*/
void bignum_assign(struct bn* dst, const struct bn* src);  /* Copy src into dst -- dst := src */

/* Faster power and module sequence of operations, for RSA: O(log n) */
void bignum_pow_mod(const struct bn* a, const struct bn* b, const struct bn* n, struct bn* res);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __BIGNUM_H__ */
