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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

enum { ADD, SUB, MUL, DIV, AND, OR, XOR, POW, MOD, RSHFT, LSHFT, ISQRT };

int main(int argc, char** argv)
{

  if (argc < 5)
  {
    printf("ERROR\n\nUsage:\n    %s [oper] [operand1] [operand2] [result]\n\nWhere oper means:\n    0 = add, 1 = sub, 2 = mul, 3 = div\n\nExample:\n    %s 3 0100 80 02\n\n    [divide 0x0100 by 0x80 and expect 0x02 as result] \n\n", argv[0], argv[0]);

    return -1;
  }  

  int oper = atoi(argv[1]);

/*
  printf("program  = %s \n", argv[0]);
  printf("operator = %s [%d]\n", argv[1], oper);
  printf("operand1 = %s \n", argv[2]);
  printf("operand2 = %s \n", argv[3]);
  printf("expected = %s \n", argv[4]);
*/

  struct bn a, b, c, res;

  bignum_init(&a);
  bignum_init(&b);
  bignum_init(&c);
  bignum_init(&res);
  bignum_from_string(&a, argv[2], strlen(argv[2]));
  bignum_from_string(&b, argv[3], strlen(argv[3]));
  bignum_from_string(&c, argv[4], strlen(argv[4]));

  struct bn a_before, b_before;
  bignum_assign(&a_before, &a);
  bignum_assign(&b_before, &b);


  switch (oper)
  {
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
    case RSHFT:
    {
      bignum_rshift(&a, &res, bignum_to_int(&b));
    } break;
    case LSHFT: 
    {
      bignum_lshift(&a, &res, bignum_to_int(&b));
    } break;
    

    default:
      printf("default switch-case hit: unknown operator '%d' \n", oper);
      assert(0); 
  }

  int cmp_result = (bignum_cmp(&res, &c) == EQUAL);

  if (!cmp_result)
  {
    char buf[8192];
    bignum_to_string(&res, buf, sizeof(buf));
    printf("\ngot %s\n", buf);
    printf(" a  = %d \n", bignum_to_int(&a));
    printf(" b  = %d \n", bignum_to_int(&b));
    printf("res = %d \n", bignum_to_int(&res));
    printf("\n");
    return 1;
  }

  assert(bignum_cmp(&a_before, &a) == EQUAL);
  assert(bignum_cmp(&b_before, &b) == EQUAL);

  return 0;
}


