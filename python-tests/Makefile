#!/usr/bin/make -f

# sudo apt install pkg-config cython3 libpython3-dev

CROSS ?= 
ARCH_CFLAGS ?= 

ARCH_ID=$(shell basename '$(CROSS)')
OBJ_DIR=$(ARCH_ID)obj
PROGRAM=$(ARCH_ID)test
PYMODULE=$(ARCH_ID)_test.so

CC= $(CROSS)gcc
CXX= $(CROSS)g++
STRIP= $(CROSS)strip

PYTHON= python3
CYTHON= cython3

RM= rm --force --verbose

PKGCONFIG= pkg-config
PACKAGES= python3-embed

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

CYFLAGS= \
	-3 \
	--cplus \
	-X language_level=3 \
	-X boundscheck=False

CSTD=-std=gnu17
CPPSTD=-std=gnu++17

OPTS= -O2 -g

DEFS= \
	-DDEBUG

INCS= \
	-I..

CYINCS= \
	-I..

LIBS=

all: $(PYMODULE)

OBJS= \
	$(OBJ_DIR)/bignum.o

PYX_SRCS= \
	_test.pyx

PYX_CPPS= $(addprefix $(OBJ_DIR)/,$(subst .pyx,.cpp,$(PYX_SRCS)))
PYX_OBJS= $(addprefix $(OBJ_DIR)/,$(subst .pyx,.o,$(PYX_SRCS)))

$(PROGRAM): $(OBJS) $(OBJ_DIR)/main.o
	$(CXX) $(CPPSTD) $(CSTD) $(LDFLAGS) $(PKG_CONFIG_LDFLAGS) -o $@ $+ $(LIBS) $(PKG_CONFIG_LIBS)

$(PYMODULE): $(OBJS) $(PYX_OBJS)
	$(CXX) -shared $(CPPSTD) $(CSTD) $(LDFLAGS) $(PKG_CONFIG_LDFLAGS) -o $@ $+ $(LIBS) $(PKG_CONFIG_LIBS)

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

%.cpp: %.pyx
	@mkdir -p '$(OBJ_DIR)'
	$(CYTHON) $(CYFLAGS) $(CYINCS) -o $@ $<

clean:
	$(RM) $(OBJS) $(PYX_OBJS) $(PYX_CPPS) $(OBJ_DIR)/main.o $(PROGRAM) $(PYMODULE)
	$(RM) $(subst .pyx,.cpp,$(PYX_SRCS))
	$(RM) $(subst .pyx,_api.cpp,$(PYX_SRCS))
	$(RM) $(subst .pyx,.h,$(PYX_SRCS))
	$(RM) $(subst .pyx,_api.h,$(PYX_SRCS))

cleanall: clean
	@find . -name '*.o' -exec $(RM) {} +
	@find . -name '*.a' -exec $(RM) {} +
	@find . -name '*.so' -exec $(RM) {} +
	@find . -name '*.pyc' -exec $(RM) {} +
	@find . -name '*.pyo' -exec $(RM) {} +
	@find . -name '*.bak' -exec $(RM) {} +
	@find . -name '*~' -exec $(RM) {} +
	@rmdir -v *obj 2>/dev/null 1>/dev/null || true
	@$(RM) core

.PHONY: all clean
