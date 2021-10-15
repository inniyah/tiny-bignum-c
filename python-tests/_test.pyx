cimport bignum

from libc.stdint cimport uint64_t, uint32_t, uint16_t, uint8_t
from libc.stdlib cimport calloc, malloc, free
from libc.string cimport memset, strcmp
from libc.stdio cimport printf

from libcpp cimport bool
from libcpp.memory cimport unique_ptr, shared_ptr, make_shared, allocator
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.utility cimport pair

from cpython cimport array
from cpython.ref cimport PyObject
from cython.operator cimport dereference as deref
from cpython.mem cimport PyMem_Malloc, PyMem_Realloc, PyMem_Free
from enum import IntEnum

import array
import numbers

cdef class _BigNum:
	BN_ARRAY_SIZE = bignum.BN_ARRAY_SIZE
	cdef bignum.bn * data
	def __cinit__(self):
		self.data = <bignum.bn *>PyMem_Malloc(sizeof(bignum.bn))
		memset(self.data, 0, sizeof(bignum.bn))
	def __dealloc__(self):
		PyMem_Free(self.data)
		self.data = NULL
	def fromInt(self, bignum.DTYPE_TMP num):
		bignum.bignum_from_int(self.data, num)
	def fromBytes(self, bytes data):
		bignum.bignum_from_string(self.data, <const char *>data, len(data))
	def fromBignum(self, _BigNum data):
		bignum.bignum_assign(self.data, data.data)
	def toBytes(self):
		cdef char buf[8192]
		bignum.bignum_to_string(self.data, buf, sizeof(buf))
		return bytes(buf)
	def is_zero(self):
		return True if bignum.bignum_is_zero(self.data) else False
	def is_odd(self):
		return True if (self.data.array[0] & 1) else False # if (b % 2) != 0
	def is_even(self):
		return False if (self.data.array[0] & 1) else True # if (b % 2) == 0
	def eq(self, _BigNum other):
		return True if strcmp(<const char *>self.data, <const char *>other.data) == 0 else False
	def cmp(self, _BigNum other):
		return {
			bignum.SMALLER: -1,
			bignum.EQUAL: 0,
			bignum.LARGER: 1,
		}[bignum.bignum_cmp(<bignum.bn*>self.data, <bignum.bn*>other.data)]
	def ne(self, _BigNum other):
		return self.cmp(other) != 0
	def lt(self, _BigNum other):
		return self.cmp(other) < 0
	def le(self, _BigNum other):
		return self.cmp(other) <= 0
	def gt(self, _BigNum other):
		return self.cmp(other) > 0
	def ge(self, _BigNum other):
		return self.cmp(other) >= 0
	def inc(self):
		bignum.bignum_inc(self.data)
	def dec(self):
		bignum.bignum_dec(self.data)
	def iadd(self, _BigNum other):
		bignum.bignum_add(self.data, other.data, self.data)
	def add(self, _BigNum other):
		ret = _BigNum()
		bignum.bignum_add(self.data, other.data, ret.data)
		return ret
	def isub(self, _BigNum other):
		bignum.bignum_sub(self.data, other.data, self.data)
	def sub(self, _BigNum other):
		ret = _BigNum()
		bignum.bignum_sub(self.data, other.data, ret.data)
		return ret
	def imul(self, _BigNum other):
		cdef bignum.bn tmp
		bignum.bignum_mul(self.data, other.data, &tmp)
		bignum.bignum_assign(self.data, &tmp)
	def mul(self, _BigNum other):
		ret = _BigNum()
		bignum.bignum_mul(self.data, other.data, ret.data)
		return ret
	def idiv(self, _BigNum other):
		cdef bignum.bn tmp
		bignum.bignum_div(self.data, other.data, &tmp)
		bignum.bignum_assign(self.data, &tmp)
	def div(self, _BigNum other):
		ret = _BigNum()
		bignum.bignum_div(self.data, other.data, ret.data)
		return ret
	def imod(self, _BigNum other):
		bignum.bignum_mod(self.data, other.data, self.data)
	def mod(self, _BigNum other):
		ret = _BigNum()
		bignum.bignum_mod(self.data, other.data, ret.data)
		return ret
	def divmod(self, _BigNum other):
		retd = _BigNum()
		retm = _BigNum()
		bignum.bignum_divmod(self.data, other.data, retd.data, retm.data)
		return retd, retm
	def pow(self, _BigNum other):
		ret = _BigNum()
		bignum.bignum_pow(self.data, other.data, ret.data)
		return ret
	def isqrt(self):
		ret = _BigNum()
		bignum.bignum_isqrt(self.data, ret.data)
		return ret
	def bit_and(self, _BigNum other):
		ret = _BigNum()
		bignum.bignum_and(self.data, other.data, ret.data)
		return ret
	def bit_or(self, _BigNum other):
		ret = _BigNum()
		bignum.bignum_or(self.data, other.data, ret.data)
		return ret
	def bit_xor(self, _BigNum other):
		ret = _BigNum()
		bignum.bignum_xor(self.data, other.data, ret.data)
		return ret
	def bit_lshift(self, int nbits):
		ret = _BigNum()
		bignum.bignum_lshift(self.data, ret.data, nbits)
		return ret
	def bit_rshift(self, int nbits):
		ret = _BigNum()
		bignum.bignum_rshift(self.data, ret.data, nbits)
		return ret

	@staticmethod
	def powmod(_BigNum base, _BigNum exp, _BigNum n):
		res = _BigNum()
		bignum.bignum_pow_mod(base.data, exp.data, n.data, res.data)
		return res

	@staticmethod
	def factorial(int num):
		n = _BigNum()
		n.fromInt(num)
		tmp = _BigNum()
		tmp.fromBignum(n)
		n.dec()
		while not n.is_zero():
			tmp.imul(n)
			n.dec()
		return tmp

	def __str__(self):
		s = self.toBytes().decode('utf8')
		return '0x' + s if s else '0x0'
	def __repr__(self):
		s = self.toBytes().decode('utf8')
		return '0x' + s if s else '0x0'

	def __eq__(self, other):
		if isinstance(other, _BigNum):
			return self.cmp(other) == 0
		return NotImplemented
	def __ne__(self, other):
		if isinstance(other, _BigNum):
			return self.cmp(other) != 0
		return NotImplemented
	def __lt__(self, other):
		if isinstance(other, _BigNum):
			return self.cmp(other) < 0
		return NotImplemented
	def __le__(self, other):
		if isinstance(other, _BigNum):
			return self.cmp(other) <= 0
		return NotImplemented
	def __gt__(self, other):
		if isinstance(other, _BigNum):
			return self.cmp(other) > 0
		return NotImplemented
	def __ge__(self, other):
		if isinstance(other, _BigNum):
			return self.cmp(other) >= 0
		return NotImplemented
	def __add__(self, other):
		if isinstance(other, _BigNum):
			return self.add(other)
		return NotImplemented
	def __radd__(self, other):
		if isinstance(other, _BigNum):
			return other.add(self)
		return NotImplemented
	def __iadd__(self, other):
		if isinstance(other, _BigNum):
			self.iadd(other)
			return self
		return NotImplemented
	def __sub__(self, other):
		if isinstance(other, _BigNum):
			return self.sub(other)
		return NotImplemented
	def __rsub__(self, other):
		if isinstance(other, _BigNum):
			return other.sub(self)
		return NotImplemented
	def __isub__(self, other):
		if isinstance(other, _BigNum):
			self.isub(other)
			return self
		return NotImplemented
	def __mul__(self, other):
		if isinstance(other, _BigNum):
			return self.mul(other)
		return NotImplemented
	def __rmul__(self, other):
		if isinstance(other, _BigNum):
			return other.mul(self)
		return NotImplemented
	def __imul__(self, other):
		if isinstance(other, _BigNum):
			self.imul(other)
			return self
		return NotImplemented
	def __floordiv__(self, other):
		if isinstance(other, _BigNum):
			return self.div(other)
		return NotImplemented
	def __rfloordiv__(self, other):
		if isinstance(other, _BigNum):
			return other.div(self)
		return NotImplemented
	def __ifloordiv__(self, other):
		if isinstance(other, _BigNum):
			self.idiv(other)
			return self
		return NotImplemented
	def __truediv__(self, other):
		if isinstance(other, _BigNum):
			return self.div(other)
		return NotImplemented
	def __rtruediv__(self, other):
		if isinstance(other, _BigNum):
			return other.div(self)
		return NotImplemented
	def __itruediv__(self, other):
		if isinstance(other, _BigNum):
			self.idiv(other)
			return self
		return NotImplemented

class BigNum(_BigNum):
	def __init__(self, data = None):
		if data is None:
			pass
		elif isinstance(data, str):
			tmp = "0x" + data.lstrip('0')
			if len(data) % 8 != 0:
				data = "00000000"[:8 - (len(data) % 8)] + data
			self.fromBytes(data.encode('utf8'))
			assert(tmp.lower() == str(self).lower())
		elif isinstance(data, numbers.Number):
			self.fromInt(data)
		elif isinstance(data, _BigNum):
			self.fromBignum(data)
	@staticmethod
	def op(operation, operator1, operator2):
		return getattr(operator1, operation)(operator2)
