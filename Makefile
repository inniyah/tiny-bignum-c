#!/usr/bin/make -f

CFLAGS= \
	-Wall \
	-fwrapv \
	-fstack-protector-strong \
	-Wall \
	-Wformat \
	-Werror=format-security \
	-Wdate-time \
	-D_FORTIFY_SOURCE=2 \
	-fPIC

LDFLAGS= \
	-Wl,-O1 \
	-Wl,-Bsymbolic-functions \
	-Wl,-z,relro \
	-Wl,--as-needed \
	-Wl,--no-undefined \
	-Wl,--no-allow-shlib-undefined \
	-Wl,-Bsymbolic-functions \
	-Wl,--dynamic-list-cpp-new \
	-Wl,--dynamic-list-cpp-typeinfo

TOOLCHAIN= 

CC= $(TOOLCHAIN)gcc
CXX= $(TOOLCHAIN)g++
LINKER=$(TOOLCHAIN)ld
AS=$(TOOLCHAIN)as
OBJCOPY=$(TOOLCHAIN)objcopy

RM= rm --force --verbose

CSTD= -std=c99
CPPSTD= -std=c++98

OPTS= -g -Og

DEFS= 

LIBS= 

INCS= 

TESTS= \
	tests/test-bignum-factorial \
	tests/test-bignum-golden \
	tests/test-bignum-div-algo \
	tests/test-bignum-hand-picked \
	tests/test-bignum-load-cmp \
	tests/test-bignum-randomized \
	tests/test-bignum-rsa

.PHONY: all
all: $(TESTS)

tests/test-bignum-%: tests/test-bignum-%.o bignum.o
	$(CC) $(CSTD) $(LDFLAGS) -o $@ $+ $(LIBS)

	@#~ $(OBJCOPY) -O ihex $@ $@.hex
	@#~ $(OBJCOPY) -O binary $@ $@.bin

%.o: %.cpp
	$(CXX) $(CPPSTD) $(OPTS) -o $@ -c $< $(DEFS) $(INCS) $(CFLAGS)

%.o: %.c
	$(CC) $(CSTD) $(OPTS) -o $@ -c $< $(DEFS) $(INCS) $(CFLAGS)

.PHONY: clean
clean:
	@$(RM) $(TESTS)
	@find . -name '*.o' -exec $(RM) {} +
	@find . -name '*.a' -exec $(RM) {} +
	@find . -name '*.so' -exec $(RM) {} +
	@find . -name '*.bak' -exec $(RM) {} +
	@find . -name '*.elf' -exec $(RM) {} +
	@find . -name '*.bin' -exec $(RM) {} +
	@find . -name '*.hex' -exec $(RM) {} +
	@find . -name '*~' -exec $(RM) {} +
	@$(RM) core
