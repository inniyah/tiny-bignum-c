from libc.stdint cimport uint64_t, uint32_t, uint16_t, uint8_t

cdef extern from "bignum.h" nogil:
	ctypedef uint32_t DTYPE
	ctypedef uint64_t DTYPE_TMP

	cdef enum:
		WORD_SIZE
		BN_ARRAY_SIZE
		DTYPE_MSB
		MAX_VAL

	cdef enum:
		SMALLER = -1
		EQUAL
		LARGER = 1

	cdef struct bn:
		DTYPE array[BN_ARRAY_SIZE]

	# Initialization functions
	void bignum_init(bn* n)
	void bignum_from_int(bn* n, DTYPE_TMP i)
	int  bignum_to_int(bn* n)
	void bignum_from_string(bn* n, const char* str, int nbytes)
	void bignum_to_string(const bn* n, char* str, int maxsize)

	# Basic arithmetic operations
	void bignum_add(const bn* a, const bn* b, bn* c)
	void bignum_sub(const bn* a, const bn* b, bn* c)
	void bignum_mul(const bn* a, const bn* b, bn* c)
	void bignum_div(const bn* a, const bn* b, bn* c)
	void bignum_mod(const bn* a, const bn* b, bn* c)
	void bignum_divmod(const bn* a, const bn* b, bn* c, bn* d)

	# Bitwise operations
	void bignum_and(const bn* a, const bn* b, bn* c)
	void bignum_or(const bn* a, const bn* b, bn* c)
	void bignum_xor(const bn* a, const bn* b, bn* c)
	void bignum_lshift(const bn* a, bn* b, int nbits)
	void bignum_rshift(const bn* a, bn* b, int nbits)

	# Special operators and comparison
	int  bignum_cmp(const bn* a, const bn* b)
	int  bignum_is_zero(const bn* n)
	void bignum_inc(bn* n)
	void bignum_dec(bn* n)
	void bignum_pow(const bn* a, const bn* b, bn* c)
	void bignum_isqrt(const bn* a, bn* b)
	void bignum_assign(bn* dst, const bn* src)

	# Power and Module operation
	void bignum_pow_mod(const bn* a, const bn* b, const bn* n, bn* res)
