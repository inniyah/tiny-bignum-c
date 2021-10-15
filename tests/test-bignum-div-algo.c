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

#include "../bignum.h"

#include <stdint.h>
#include <stdio.h>

/*
 * Division algorithms using bitwise operations
 * 
 * To showcase different methods for implementing division
 */


static int divide(int dividend, int divisor)
{
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

void divide2(unsigned dividend, unsigned divisor, unsigned* quotient, unsigned* remainder)
{
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

int main()
{
  unsigned a,b,q,r;

  a = 255;
  b = 8;

  /*
    a div b = q [with remainder r]
  */

  printf("1: %d / %d = %d \n", a, b, divide(a, b));

  divide2(a, b, &q, &r);
  printf("2: %d / %d = %d (%d) \n", a, b, q, r);

  assert(q == (a / b));
  assert(r == (a % b));

  return 0;
}


