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

#include "bignum.h"

#include <stdio.h>
#include <string.h>

#include "test_harness.h"

FIXTURE(bignum) {
	//~ struct { int id; } data;
};

FIXTURE_SETUP(bignum) {}
FIXTURE_TEARDOWN(bignum) {}

/*
 * Testing bignum_from_string and bignum_from_int
 *
 * Also testing bignum_cmp
 */

TEST_F(bignum, load_cmp) {
  char sabuf[8192];
  char sbbuf[8192];
  char scbuf[8192];
  char sdbuf[8192];
  char iabuf[8192];
  char ibbuf[8192];
  char icbuf[8192];
  char idbuf[8192];

  struct bn sa, sb, sc, sd, se;
  struct bn ia, ib, ic, id;

  bignum_init(&sa);
  bignum_init(&sb);
  bignum_init(&sc);
  bignum_init(&sd);
  bignum_init(&ia);
  bignum_init(&ib);
  bignum_init(&ic);
  bignum_init(&id);

  printf("\nLoading numbers from strings and from int.\n");

  bignum_from_string(&sa, "000000FF", 8);
  bignum_from_string(&sb, "0000FF00", 8);
  bignum_from_string(&sc, "00FF0000", 8);
  bignum_from_string(&sd, "FF000000", 8);

  bignum_from_int(&ia, 0x000000FF);
  bignum_from_int(&ib, 0x0000FF00);
  bignum_from_int(&ic, 0x00FF0000);
  bignum_from_int(&id, 0xFF000000);

  printf("Verifying comparison function.\n");

  EXPECT_EQ(bignum_cmp(&ia, &ib), SMALLER);
  EXPECT_EQ(bignum_cmp(&ia, &ic), SMALLER);
  EXPECT_EQ(bignum_cmp(&ia, &id), SMALLER);

  EXPECT_EQ(bignum_cmp(&ib, &ia), LARGER);
  EXPECT_EQ(bignum_cmp(&ic, &ia), LARGER);
  EXPECT_EQ(bignum_cmp(&id, &ia), LARGER);

  EXPECT_EQ(bignum_cmp(&sa, &sb), SMALLER);
  EXPECT_EQ(bignum_cmp(&sa, &sc), SMALLER);
  EXPECT_EQ(bignum_cmp(&sa, &sd), SMALLER);

  EXPECT_EQ(bignum_cmp(&sb, &sa), LARGER);
  EXPECT_EQ(bignum_cmp(&sc, &sa), LARGER);
  EXPECT_EQ(bignum_cmp(&sd, &sa), LARGER);

  EXPECT_EQ(bignum_cmp(&ia, &sa), EQUAL);
  EXPECT_EQ(bignum_cmp(&ib, &sb), EQUAL);
  EXPECT_EQ(bignum_cmp(&ic, &sc), EQUAL);
  EXPECT_EQ(bignum_cmp(&id, &sd), EQUAL);

  printf("Verifying to_string function.\n");

  bignum_to_string(&sa, sabuf, sizeof(sabuf));
  bignum_to_string(&sb, sbbuf, sizeof(sbbuf));
  bignum_to_string(&sc, scbuf, sizeof(scbuf));
  bignum_to_string(&sd, sdbuf, sizeof(sdbuf));

  bignum_to_string(&ia, iabuf, sizeof(iabuf));
  bignum_to_string(&ib, ibbuf, sizeof(ibbuf));
  bignum_to_string(&ic, icbuf, sizeof(icbuf));
  bignum_to_string(&id, idbuf, sizeof(idbuf));

  EXPECT_EQ(strcmp(sabuf, iabuf), 0);
  EXPECT_EQ(strcmp(sbbuf, ibbuf), 0);
  EXPECT_EQ(strcmp(scbuf, icbuf), 0);
  EXPECT_EQ(strcmp(sdbuf, idbuf), 0);

  printf("Verifying increment + decrement functions.\n");

  bignum_init(&sd);
  int i;
  for (i = 0; i < 255; ++i) {
    bignum_inc(&sd);
    EXPECT_FALSE(bignum_is_zero(&sd));
  }

  EXPECT_EQ(bignum_cmp(&sd, &ia), EQUAL);

  for (i = 0; i < 255; ++i) {
    EXPECT_FALSE(bignum_is_zero(&sd));
    bignum_dec(&sd);
  }
  EXPECT_TRUE(bignum_is_zero(&sd));

  bignum_init(&sa);
  bignum_init(&sb);
  bignum_init(&sc);
  bignum_init(&sd);

  char hex_1000[]    = "000003E8";
  char hex_1000000[] = "000F4240";

  /* Load 0x0308 into A and B from string */
  bignum_from_string(&sa, hex_1000, 8);
  bignum_from_string(&sb, hex_1000, 8);

  /* Load 0x0308 into C from integer */
  bignum_from_int(&sc, 0x3e8);

  /* Check for agreement - does bignum_from_string match bignum_from_int? */
  EXPECT_EQ(bignum_cmp(&sa, &sc), EQUAL);
  EXPECT_EQ(bignum_cmp(&sb, &sc), EQUAL);

  /* Load comparison value: */
  bignum_from_string(&sd, hex_1000000, 8);
  bignum_from_int(&se, 0xf4240);

  /* Perform calculation:  C = A * B => C = 0x308 * 0x308 */
  bignum_mul(&sa, &sb, &sc);

  /* Check if results agree: */
  EXPECT_EQ(bignum_cmp(&sc, &sd), EQUAL);
  EXPECT_EQ(bignum_cmp(&sc, &se), EQUAL);

  /* Perform calculation other way around: C = B * A */
  bignum_mul(&sb, &sa, &sc);

  /* Check if results agree: */
  EXPECT_EQ(bignum_cmp(&sc, &sd), EQUAL);
  EXPECT_EQ(bignum_cmp(&sc, &se), EQUAL);
}

/*
 * Golden tests - input and expected outputs:
 */

/* For table-defined list of tests */
struct test {
  char op;           /* operator: plus, minus, multiply, divide */
  uint64_t a,b,c;    /* operands a, b and c - c contains expected result such that [c] = [a] [op] [b] */
};

/* Golden tests - input and expected outputs: */
static struct test oracle[] =
{
  {'+', 80, 20, 100 },
  {'+', 18, 22, 40 },
  {'+', 12, 8, 20 },
  {'+', 100080, 20, 100100 },
  {'+', 18, 559022, 559040 },
  {'+', 2000000000, 2000000000, 4000000000 },
  {'+', 0x00FFFF, 1, 0x010000 },
  {'+', 0x00FFFF00, 0x00000100, 0x01000000 },
  {'-', 1000001, 1000000, 1 },
  {'-', 42, 0, 42 },
  {'-', 101, 100, 1 },
  {'-', 242, 42, 200 },
  {'-', 1042, 0, 1042 },
  {'-', 101010101, 101010100, 1 },
  {'-', 0x010000, 1, 0x00FFFF },
//./build/test_random 1 0000000000f505c2 00000000000fffe0 0000000000e505e2
  {'-', 0xf505c2, 0x0fffe0, 0xe505e2 },
//./build/test_random 1 00000000009f735a 000000000065ffb5 00000000003973a5
  {'-', 0x9f735a, 0x65ffb5, 0x3973a5 },
//./build/test_random 1 0000000000cf7810 000000000004ff34 0000000000ca78dc
  {'-', 0xcf7810, 0x04ff34, 0xca78dc },
//./build/test_random 1 0000000000bbc55f 00000000004eff76 00000000006cc5e9
  {'-', 0xbbc55f, 0x4eff76, 0x6cc5e9 },
  {'-', 0x100000, 1, 0x0fffff },
  {'-', 0x010000, 1, 0x00ffff },
//./build/test_random 1 0000000000b5beb4 000000000001ffc4 0000000000b3bef0
  {'-', 0xb5beb4, 0x01ffc4, 0xb3bef0 },
//./build/test_random 1 0000000000707655 000000000050ffa8 00000000001f76ad
  {'-', 0x707655, 0x50ffa8, 0x1f76ad },
//./build/test_random 1 0000000000f0a990 00000000001cffd1 0000000000d3a9bf
  {'-', 0xf0a990, 0x1cffd1, 0xd3a9bf },
  {'*', 0x010203, 0x1020, 0x10407060 },
  {'*', 42, 0,   0 },
  {'*', 42, 1,   42 },
  {'*', 42, 2,   84 },
  {'*', 42, 10,  420 },
  {'*', 42, 100, 4200 },
  {'*', 420, 1000, 420000 },
  {'*', 200, 8,  1600 },
  {'*', 2, 256, 512 },
  {'*', 500, 2, 1000 },
  {'*', 500000, 2, 1000000 },
  {'*', 500, 500, 250000 },
  {'*', 1000000000, 2, 2000000000 },
  {'*', 2, 1000000000, 2000000000 },
  {'*', 1000000000, 4, 4000000000 },
  {'/', 0xFFFFFFFF, 0xFFFFFFFF, 1 },
  {'/', 0xFFFFFFFF, 0x10000, 0xFFFF },
  {'/', 0xFFFFFFFF, 0x1000, 0xFFFFF },
  {'/', 0xFFFFFFFF, 0x100, 0xFFFFFF },
  {'/', 1000000, 1000, 1000 },
  {'/', 1000000, 10000, 100 },
  {'/', 1000000, 100000, 10 },
  {'/', 1000000, 1000000, 1 },
  {'/', 1000000, 10000000, 0 },
  {'/', 28, 7, 4 },
  {'/', 27, 7, 3 },
  {'/', 26, 7, 3 },
  {'/', 25, 7, 3 },
  {'/', 24, 7, 3 },
  {'/', 23, 7, 3 },
  {'/', 22, 7, 3 },
  {'/', 21, 7, 3 },
  {'/', 20, 7, 2 },
  {'/', 0, 12, 0 },
  {'/', 10, 1, 10 },
  {'/', 0xFFFFFFFF, 1, 0xFFFFFFFF },
  {'/', 0xFFFFFFFF, 0x10000, 0xFFFF },
//./build/test_random 3 0000000000b36627 00000000000dff95 000000000000000c
  {'/', 0xb36627, 0x0dff95, 0x0c },
//./build/test_random 3 0000000000e5a18e 000000000009ff82 0000000000000016
  {'/', 0xe5a18e, 0x09ff82, 0x16 },
//./build/test_random 3 000000000045edd0 000000000004ff1a 000000000000000d
  {'/', 0x45edd0, 0x04ff1a, 0x0d },
  {'%', 8, 3, 2 },
  {'%', 1024, 1000, 24 },
  {'%', 0xFFFFFF, 1234, 985 },
  {'%', 0xFFFFFFFF, 0xEF, 0x6D },
  {'%', 12345678, 16384, 8526 },
//mikl@21972:~/c_bignum2$ ./build/test_random 8 0000000000e7a344 000000000071ffe8 000000000003a374
  {'%', 0xe7a344, 0x71ffe8, 0x03a374 },
//./build/test_random 8 0000000000a3a9a1 000000000002ff44 000000000001d149
  {'%', 0xa3a9a1, 0x2ff44, 0x1d149 },
//./build/test_random 8 0000000000c128b2 000000000060ff61 0000000000602951
  {'%', 0xc128b2, 0x60ff61, 0x602951 },
//./build/test_random 8 0000000000dc2254 0000000000517fea 0000000000392280
  {'%', 0xDC2254, 0x517FEA, 0x392280 },
//./build/test_random 8 0000000000769c99 00000000002cffda 00000000001c9ce5
  {'%', 0x769c99, 0x2cffda, 0x1c9ce5 },
//./build/test_random 8 0000000000c19076 000000000031ffd4 00000000002b90fa
  {'%', 0xc19076, 0x31ffd4, 0x2b90fa },
  {'&', 0xFFFFFFFF, 0x005500AA, 0x005500AA },
  {'&', 7, 3, 3 },
  {'&', 0xFFFFFFFF, 0, 0 },
  {'&', 0, 0xFFFFFFFF, 0 },
  {'&', 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF },
  {'|', 0xFFFFFFFF, 0, 0xFFFFFFFF },
  {'|', 0, 0xFFFFFFFF, 0xFFFFFFFF },
  {'|', 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF },
  {'|', 0x55555555, 0xAAAAAAAA, 0xFFFFFFFF },
  {'|', 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF },
  {'|', 4, 3, 7 },
  {'^', 7, 4, 3 },
  {'^', 0xFFFF, 0x5555, 0xAAAA },
  {'^', 0x5555, 0xAAAA, 0xFFFF },
  {'^', 0xAAAA, 0x5555, 0xFFFF },
  {'^', 0x0000, 0xFFFF, 0xFFFF },
  {'^', 0x5555, 0xFFFF, 0xAAAA },
  {'^', 0xAAAA, 0xFFFF, 0x5555 },
  {'p', 2, 0, 1 },
  {'p', 2, 1, 2 },
  {'p', 2, 2, 4 },
  {'p', 2, 3, 8 },
  {'p', 2, 10, 1024 },
  {'p', 2, 20, 1048576 },
  {'p', 2, 30, 1073741824 },
  {'<', 1, 0,        1},
  {'<', 1, 1,        2},
  {'<', 1, 2,        4},
  {'<', 1, 3,        8},
  {'<', 1, 4,       16},
  {'<', 1, 5,       32},
  {'<', 1, 6,       64},
  {'<', 1, 7,      128},
  {'<', 1, 8,      256},
  {'<', 1, 9,      512},
  {'<', 1, 10,    1024},
  {'<', 1, 11,    2048},
  {'<', 1, 12,    4096},
  {'<', 1, 13,    8192},
  {'<', 1, 14,   16384},
  {'<', 1, 15,   32768},
  {'<', 1, 16,   65536},
  {'<', 1, 17,  131072},
  {'<', 1, 18,  262144},
  {'<', 1, 19,  524288},
  {'<', 1, 20, 1048576},
  {'<', 0xdd, 0x18, 0xdd000000 },
  {'<', 0x68, 0x02, 0x01a0 },
  {'>', 0xf6, 1, 0x7b },
  {'>', 0x1a, 1, 0x0d },
  {'>', 0xb0, 1, 0x58 },
  {'>', 0xba, 1, 0x5d },
  {'>', 0x10, 3, 0x02 },
  {'>', 0xe8, 4, 0x0e },
  {'>', 0x37, 4, 0x03 },
  {'>', 0xa0, 7, 0x01 },
  {'>',       1,  0, 1},
  {'>',       2,  1, 1},
  {'>',       4,  2, 1},
  {'>',       8,  3, 1},
  {'>',      16,  4, 1},
  {'>',      32,  5, 1},
  {'>',      64,  6, 1},
  {'>',     128,  7, 1},
  {'>',     256,  8, 1},
  {'>',     512,  9, 1},
  {'>',    1024, 10, 1},
  {'>',    2048, 11, 1},
  {'>',    4096, 12, 1},
  {'>',    8192, 13, 1},
  {'>',   16384, 14, 1},
  {'>',   32768, 15, 1},
  {'>',   65536, 16, 1},
  {'>',  131072, 17, 1},
  {'>',  262144, 18, 1},
  {'>',  524288, 19, 1},
  {'>', 1048576, 20, 1},
};
static const int ntests = sizeof(oracle) / sizeof(*oracle);


TEST_F(bignum, golden) {
  struct bn sa, sb, sc, sd;
  uint32_t ia, ib, ic;
  char op;
  char buf[8192];

  for (int i = 0; i < ntests; ++i) {
    /* Copy operator + operands from oracle */
    op = oracle[i].op;
    ia = oracle[i].a;
    ib = oracle[i].b;
    ic = oracle[i].c;

    /* Initialize big-num structures: */
    bignum_init(&sd); /* init result holder */
    bignum_from_int(&sa, ia);
    bignum_from_int(&sb, ib);
    bignum_from_int(&sc, ic);

    /* Perform calculation: */
    switch (op) {
      case '+': bignum_add(&sa, &sb, &sd);   break;
      case '-': bignum_sub(&sa, &sb, &sd);   break;
      case '*': bignum_mul(&sa, &sb, &sd);   break;
      case '/': bignum_div(&sa, &sb, &sd);   break;
      case '%': bignum_mod(&sa, &sb, &sd);   break;
      case '&': bignum_and(&sa, &sb, &sd);   break;
      case '|': bignum_or (&sa, &sb, &sd);   break;
      case '^': bignum_xor(&sa, &sb, &sd);   break;
      case 'p': bignum_pow(&sa, &sb, &sd);   break;
      case '<': bignum_lshift(&sa, &sd, ib); break;
      case '>': bignum_rshift(&sa, &sd, ib); break;

      /* Crash program if operator is unsupported. */
      default:  require(0, "default switch-case hit");
    }

    /* Verify validity: */
    EXPECT_EQ(bignum_cmp(&sc, &sd), EQUAL);

    /* Print status: */
    int test_passed = (bignum_cmp(&sc, &sd) == EQUAL);

    if (op == 'p') {
      printf("  %s pow(%u, %u) = %u \n", (test_passed ? "[ OK ]" : "[FAIL]"), ia, ib, ic);
    } else if ((op == '<') || (op == '>')) {
      printf("  %s %u %c%c %u = %u \n", (test_passed ? "[ OK ]" : "[FAIL]"), ia, op, op, ib, ic);
    } else {
      printf("  %s %u %c %u = %u \n", (test_passed ? "[ OK ]" : "[FAIL]"), ia, op, ib, ic);
    }

    if (!test_passed) {
      bignum_to_string(&sa, buf, sizeof(buf));
      printf("    a = %s \n", buf);
      bignum_to_string(&sb, buf, sizeof(buf));
      printf("    b = %s \n", buf);
      bignum_to_string(&sc, buf, sizeof(buf));
      printf("    c = %s \n", buf);
      bignum_to_string(&sd, buf, sizeof(buf));
      printf("    d = %s \n", buf);
      printf("\n");
    }
  }
}

/*
 * Hand-picked test cases
 */

TEST_F(bignum, hand_picked) {
    // Evil test case triggering infinite-loop - reported by jepler@github
    // See: https://github.com/kokke/tiny-bignum-c/issues/2

    struct bn a1, b1, c1;
    bignum_from_int(&a1, 1);
    bignum_init(&b1); bignum_dec(&b1); // b now holds biggest bignum
    bignum_div(&b1, &a1, &c1);

    // Test case triggering overflow-bug - reported by XYlearn@github
    // See: https://github.com/kokke/tiny-bignum-c/issues/3

    struct bn a2, b2, c2;

    bignum_from_int(&a2, 0);
    bignum_from_int(&b2, 1);
    bignum_sub(&a2, &b2, &a2);
    bignum_from_int(&b2, 3);
    bignum_add(&a2, &b2, &a2);
    bignum_from_int(&c2, 2);
    EXPECT_EQ(bignum_cmp(&a2, &c2), EQUAL);

    // Test case triggering rshift-bug - reported by serpilliere
    // See: https://github.com/kokke/tiny-bignum-c/pull/7
    struct bn n1, n2, n3;

    bignum_from_string(&n1, "11112222333344445555666677778888", 32);
    bignum_from_string(&n3, "1111222233334444", 16);
    bignum_rshift(&n1, &n2, 64);

    // Check that (0x11112222333344445555666677778888 >> 64) == 0x1111222233334444
    EXPECT_EQ(bignum_cmp(&n2, &n3), EQUAL);
}

/*
    Testing Big-Number library by calculating factorial(100) a.k.a. 100!
    ====================================================================

    For the uninitiated:
        factorial(N) := N * (N-1) * (N-2) * ... * 1

    Example:
        factorial(5) = 5 * 4 * 3 * 2 * 1 = 120

    Validated by Python implementation of big-numbers:
    --------------------------------------------------

        In [1]: import math

        In [2]: "%x" % math.factorial(100)
        Out[]: '1b30964ec395dc24069528d54bbda40d16e966ef9a70eb21b5b2943a321cdf10391745570cca9420c6ecb3b72ed2ee8b02ea2735c61a000000000000000000000000'

    ... which should also be the result of this program's calculation
*/

static void factorial(struct bn* n, struct bn* res) {
  struct bn tmp;

  /* Copy n -> tmp */
  bignum_assign(&tmp, n);

  /* Decrement n by one */
  bignum_dec(n);

  /* Begin summing products: */
  while (!bignum_is_zero(n)) {
    /* res = tmp * n */
    bignum_mul(&tmp, n, res);

    /* n -= 1 */
    bignum_dec(n);

    /* tmp = res */
    bignum_assign(&tmp, res);
  }

  /* res = tmp */
  bignum_assign(res, &tmp);
}

TEST_F(bignum, factorial) {
  struct bn num;
  struct bn result;
  char buf[8192];

  bignum_from_int(&num, 100);
  factorial(&num, &result);
  bignum_to_string(&result, buf, sizeof(buf));
  printf("factorial(100) using bignum = %s\n", buf);
  EXPECT_FALSE(strcmp("1b30964ec395dc24069528d54bbda40d16e966ef9a70eb21b5b2943a321cdf10391745570cca9420c6ecb3b72ed2ee8b02ea2735c61a000000000000000000000000", buf));
}

/*
 * Division algorithms using bitwise operations
 *
 * To showcase different methods for implementing division
 */

static int divide(int dividend, int divisor) {
  int current = 1;
  int denom = divisor;
  // This step is required to find the biggest current number which can be
  // divided with the number safely.
  while (denom <= dividend) {
    current <<= 1;
    denom <<= 1;
  }
  // Since we may have increased the denomitor more than dividend
  // thus we need to go back one shift, and same would apply for current.
  denom >>= 1;
  current >>= 1;
  int answer = 0;
  // Now deal with the smaller number.
  while (current != 0)
  {
    if (dividend >= denom)
    {
      dividend -= denom;
      answer |= current;
    }
    current >>= 1;
    denom >>= 1;
  }
  return answer;
}

static void divide2(unsigned dividend, unsigned divisor, unsigned* quotient, unsigned* remainder) {
  unsigned t, num_bits;
  unsigned q, bit, d = 0;
  unsigned i;

  *remainder = 0;
  *quotient = 0;

  if (divisor == 0)
    return;

  if (divisor > dividend) {
    *remainder = dividend;
    return;
  }

  if (divisor == dividend) {
    *quotient = 1;
    return;
  }

  num_bits = 32;

  while ((*remainder) < divisor) {
    bit = (dividend & 0x80000000) >> 31;
    *remainder = ((*remainder) << 1) | bit;
    d = dividend;
    dividend = dividend << 1;
    num_bits--;
  }

  /* The loop, above, always goes one iteration too far.
     To avoid inserting an "if" statement inside the loop
     the last iteration is simply reversed. */
  dividend = d;
  *remainder = (*remainder) >> 1;
  num_bits++;

  for (i = 0; i < num_bits; i++) {
    bit = (dividend & 0x80000000) >> 31;
    *remainder = ((*remainder) << 1) | bit;
    t = (*remainder) - divisor;
    q = !((t & 0x80000000) >> 31);
    dividend = dividend << 1;
    *quotient = ((*quotient) << 1) | q;
    if (q) {
       *remainder = t;
     }
  }
}  /* unsigned_divide */

TEST_F(bignum, division) {
  unsigned a,b,q,r;

  a = 255;
  b = 8;

  // a div b = q [with remainder r]

  printf("1: %d / %d = %d \n", a, b, divide(a, b));

  divide2(a, b, &q, &r);
  printf("2: %d / %d = %d (%d) \n", a, b, q, r);

  EXPECT_EQ(q, (a / b));
  EXPECT_EQ(r, (a % b));
}

/*
 * Test different operations
 */

enum { ADD, SUB, MUL, DIV, AND, OR, XOR, POW, MOD, RSHFT, LSHFT, ISQRT };

static int run_oper(int oper, const char * operand1, const char * operand2, const char * expected) {
  struct bn a, b, c, res;

  bignum_init(&a);
  bignum_init(&b);
  bignum_init(&c);
  bignum_init(&res);

  bignum_from_string(&a, operand1, strlen(operand1));
  bignum_from_string(&b, operand2, strlen(operand2));
  bignum_from_string(&c, expected, strlen(expected));

  struct bn a_before, b_before;
  bignum_assign(&a_before, &a);
  bignum_assign(&b_before, &b);

  switch (oper) {
    case ADD:   bignum_add(&a, &b, &res);   break;
    case SUB:   bignum_sub(&a, &b, &res);   break;
    case MUL:   bignum_mul(&a, &b, &res);   break;
    case DIV:   bignum_div(&a, &b, &res);   break;
    case AND:   bignum_and(&a, &b, &res);   break;
    case OR:    bignum_or (&a, &b, &res);   break;
    case XOR:   bignum_xor(&a, &b, &res);   break;
    case POW:   bignum_pow(&a, &b, &res);   break;
    case MOD:   bignum_mod(&a, &b, &res);   break;
    case ISQRT: bignum_isqrt(&a, &res);     break;
    case RSHFT: bignum_rshift(&a, &res, bignum_to_int(&b)); break;
    case LSHFT: bignum_lshift(&a, &res, bignum_to_int(&b)); break;
    default: // Unknown operator
      printf("\nUnknown operator\n");
      return 1;
  }

  int cmp_result = (bignum_cmp(&res, &c) == EQUAL);

  if (!cmp_result) {
    char buf[8192];
    bignum_to_string(&res, buf, sizeof(buf));
    printf("\ngot %s\n", buf);
    printf(" a  = %d \n", bignum_to_int(&a));
    printf(" b  = %d \n", bignum_to_int(&b));
    printf("res = %d \n", bignum_to_int(&res));
    printf("\n");
    return 1;
  }

  if (bignum_cmp(&a_before, &a) != EQUAL) {
    char buf[8192];
    bignum_to_string(&a_before, buf, sizeof(buf));
    printf("a_before: %s\n", buf);
    bignum_to_string(&a, buf, sizeof(buf));
    printf("a: %s\n", buf);
    return 1;
  }
  if (bignum_cmp(&b_before, &b) != EQUAL) {
    char buf[8192];
    bignum_to_string(&b_before, buf, sizeof(buf));
    printf("b_before: %s\n", buf);
    bignum_to_string(&b, buf, sizeof(buf));
    printf("b: %s\n", buf);
    return 1;
  }

  return 0;
}

TEST_F(bignum, operations) {
  EXPECT_FALSE(run_oper(DIV, "00000100", "00000080", "00000002")); // Divide 0x0100 by 0x80 and expect 0x02 as result
}

/*
  message m = 123

  P = 61                  <-- 1st prime, keep secret and destroy after generating E and D
  Q = 53                  <-- 2nd prime, keep secret and destroy after generating E and D
  N = P * Q = 3233        <-- modulo factor, give to others

  T = totient(N)          <-- used for key generation
    = (P - 1) * (Q - 1)
    = 3120

  E = 1 < E < totient(N)  <-- public exponent, give to others
  E is chosen to be 17

  find a number D such that ((E * D) / T) % T == 1
  D is chosen to be 2753  <-- private exponent, keep secret


  encrypt(T) = (T ^ E) mod N     where T is the clear-text message
  decrypt(C) = (C ^ D) mod N     where C is the encrypted cipher


  Public key consists of  (N, E)
  Private key consists of (N, D)


  RSA wikipedia example (with small-ish factors):

    public key  : n = 3233, e = 17
    private key : n = 3233, d = 2753
    message     : n = 123

    cipher = (123 ^ 17)   % 3233 = 855
    clear  = (855 ^ 2753) % 3233 = 123
*/

void bignum_lcm(const struct bn* n1, const struct bn* n2, struct bn* max) {
  struct bn tmp;
  bignum_init(&tmp);
  bignum_init(max);
  if (bignum_cmp(n1, n2), LARGER) {
    bignum_assign(max, n1);
  } else {
    bignum_assign(max, n2);
  }
  while (1) {
    bignum_mod(max, n1, &tmp);
    if (bignum_is_zero(&tmp)) {
      bignum_mod(max, n2, &tmp);
      if (bignum_is_zero(&tmp)) {
        char buf[8192];
        bignum_to_string(n1, buf, sizeof(buf));
        printf("The LCM of 0x%s", buf);
        bignum_to_string(n2, buf, sizeof(buf));
        printf("and 0x%s", buf);
        bignum_to_string(max, buf, sizeof(buf));
        printf("is 0x%s\n", buf);
        break;
      }
    }
    bignum_inc(max);
  }
}

// RSA wikipedia example (with small-ish factors):
//
// public key  : n = 3233, e = 17
// private key : n = 3233, d = 2753
// message     : n = 123
//
// cipher = (123 ^ 17)   % 3233 = 855
// clear  = (855 ^ 2753) % 3233 = 123  

TEST_F(bignum, rsa_simple) {
  /* Testing with very small and simple terms */
  char buf[8192];

  struct bn tmp;
  bignum_init(&tmp);

  struct bn p, q, n, t, e, d;
  bignum_init(&p);
  bignum_init(&q);
  bignum_init(&n);

  bignum_from_int(&p, 61);
  bignum_from_int(&q, 53);
  bignum_mul(&p, &q, &n);

  bignum_to_string(&n, buf, sizeof(buf));
  EXPECT_EQ(strcmp(buf, "ca1"), 0); // 0x0CA1 = 3233
  bignum_dec(&p);
  bignum_dec(&q);
  bignum_lcm(&p, &q, &t);
  bignum_to_string(&t, buf, sizeof(buf));
  EXPECT_EQ(strcmp(buf, "30c"), 0); // 0x030C =780
  bignum_init(&e);
  bignum_init(&d);

  bignum_from_int(&e, 17);
  bignum_from_int(&d, 413);

  struct bn one;
  bignum_init(&one);
  bignum_inc(&one);
  bignum_to_string(&one, buf, sizeof(buf));
  EXPECT_EQ(strcmp(buf, "1"), 0);
  // (1 < E and E < T)
  EXPECT_EQ(bignum_cmp(&e, &one), LARGER);
  EXPECT_EQ(bignum_cmp(&e, &t), SMALLER);
  // (E * D) % T == 1
  struct bn exd;
  bignum_init(&exd);
  bignum_mul(&e, &d, &exd);
  bignum_mod(&exd, &t, &tmp);
  EXPECT_EQ(bignum_cmp(&tmp, &one), EQUAL);

  bignum_to_string(&n, buf, sizeof(buf));
  printf("Public key: (0x%s", buf);
  bignum_to_string(&e, buf, sizeof(buf));
  printf(", 0x%s)\n", buf);
  bignum_to_string(&n, buf, sizeof(buf));
  printf("Private key: (0x%s", buf);
  bignum_to_string(&d, buf, sizeof(buf));
  printf(", 0x%s)\n", buf);

  struct bn msg, enc, dec;
  bignum_init(&msg);
  bignum_init(&enc);
  bignum_init(&dec);

  // clear-text message
  bignum_from_int(&msg, 123);
  bignum_to_string(&msg, buf, sizeof(buf));
  printf("Clean message: 0x%s\n", buf);

  // encrypted cipher: (MSG ^ E) mod N
  bignum_pow_mod(&msg, &e, &n, &enc);
  bignum_to_string(&enc, buf, sizeof(buf));
  printf("Encrypted message: 0x%s\n", buf);
  bignum_to_string(&enc, buf, sizeof(buf));
  EXPECT_EQ(strcmp(buf, "357"), 0); // 0x0357 = 855

  // decrypted message: (CRYPT ^ D) mod N
  bignum_pow_mod(&enc, &d, &n, &dec);
  bignum_to_string(&dec, buf, sizeof(buf));
  printf("Decrypted message: 0x%s\n", buf);
  EXPECT_EQ(bignum_cmp(&dec, &msg), EQUAL);
  bignum_to_string(&dec, buf, sizeof(buf));
  EXPECT_EQ(strcmp(buf, "7b"), 0); // 0x007b = 123
}

int test_bignum_main(int argc, char **argv) {
  printf("WORD_SIZE = %d\n", (int)WORD_SIZE);
  printf("BN_ARRAY_SIZE = %d\n", (int)BN_ARRAY_SIZE);
  printf("sizeof(struct bn) = %d\n", (int)sizeof(struct bn));
  return test_harness_run(argc, argv);
}
