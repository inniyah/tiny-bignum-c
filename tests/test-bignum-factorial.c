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

#include "../bignum.h"

#include <stdio.h>
#include <string.h>

void factorial(struct bn* n, struct bn* res) {
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

int main() {
  struct bn num;
  struct bn result;
  char buf[8192];

  bignum_from_int(&num, 100);
  factorial(&num, &result);
  bignum_to_string(&result, buf, sizeof(buf));
  printf("factorial(100) using bignum = %s\n", buf);
  assert(strcmp("1b30964ec395dc24069528d54bbda40d16e966ef9a70eb21b5b2943a321cdf10391745570cca9420c6ecb3b72ed2ee8b02ea2735c61a000000000000000000000000", buf) == 0);

  return 0;
}
