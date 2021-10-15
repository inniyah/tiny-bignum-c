#!/usr/bin/make -f

CROSS ?= 
ARCH_CFLAGS ?= 

#~ CROSS ?= /usr/bin/arm-linux-gnueabihf-
#~ ARCH_CFLAGS ?= -march=armv7-m -mtune=cortex-m0

ARCH_ID=$(shell basename '$(CROSS)')
OBJ_DIR=$(ARCH_ID)obj
PROGRAM=$(ARCH_ID)test

CC= $(CROSS)gcc
CXX= $(CROSS)g++
STRIP= $(CROSS)strip

RM= rm --force --verbose

PKGCONFIG= pkg-config
PACKAGES= 

ifndef PACKAGES
PKG_CONFIG_CFLAGS=
PKG_CONFIG_LDFLAGS=
PKG_CONFIG_LIBS=
else
PKG_CONFIG_CFLAGS=`pkg-config --cflags $(PACKAGES)`
PKG_CONFIG_LDFLAGS=`pkg-config --libs-only-L $(PACKAGES)`
PKG_CONFIG_LIBS=`pkg-config --libs-only-l $(PACKAGES)`
endif

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

CSTD=-std=gnu17
CPPSTD=-std=gnu++17

OPTS= -O2 -g

DEFS= \
	-DDEBUG

INCS= \
	-I..

LIBS=

all: $(PROGRAM)

OBJS= \
	$(OBJ_DIR)/test-main.o \
	$(OBJ_DIR)/test-bignum.o \
	$(OBJ_DIR)/bignum.o

$(PROGRAM): $(OBJS)
	$(CXX) $(CPPSTD) $(CSTD) $(LDFLAGS) $(PKG_CONFIG_LDFLAGS) -o $@ $+ $(LIBS) $(PKG_CONFIG_LIBS)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p '$(OBJ_DIR)'
	$(CXX) $(CPPSTD) $(OPTS) -o $@ -c $< $(DEFS) $(INCS) $(CFLAGS) $(ARCH_CFLAGS) $(PKG_CONFIG_CFLAGS)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p '$(OBJ_DIR)'
	$(CC) $(CSTD) $(OPTS) -o $@ -c $< $(DEFS) $(INCS) $(CFLAGS) $(ARCH_CFLAGS) $(PKG_CONFIG_CFLAGS)

$(OBJ_DIR)/%.o: ../%.cpp
	@mkdir -p '$(OBJ_DIR)'
	$(CXX) $(CPPSTD) $(OPTS) -o $@ -c $< $(DEFS) $(INCS) $(CFLAGS) $(ARCH_CFLAGS) $(PKG_CONFIG_CFLAGS)

$(OBJ_DIR)/%.o: ../%.c
	@mkdir -p '$(OBJ_DIR)'
	$(CC) $(CSTD) $(OPTS) -o $@ -c $< $(DEFS) $(INCS) $(CFLAGS) $(ARCH_CFLAGS) $(PKG_CONFIG_CFLAGS)

clean:
	$(RM) $(OBJS) $(PROGRAM)

cleanall: clean
	@find . -name '*.o' -exec $(RM) {} +
	@find . -name '*.a' -exec $(RM) {} +
	@find . -name '*.so' -exec $(RM) {} +
	@find . -name '*.pyc' -exec $(RM) {} +
	@find . -name '*.pyo' -exec $(RM) {} +
	@find . -name '*.bak' -exec $(RM) {} +
	@find . -name '*~' -exec $(RM) {} +
	@$(RM) core
	@$(RM) --recursive *obj

.PHONY: all clean
