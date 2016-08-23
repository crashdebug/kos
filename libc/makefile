HOST?=$(shell ../scripts/default-host.sh)
HOSTARCH:=$(shell ../scripts/target-triplet-to-arch.sh $(HOST))

BINS=../tools/bin/$(HOST)-

CC=g++
AR=ar
AS=as

CFLAGS?=-O2 -g
CPPFLAGS?=
LDFLAGS?=
LIBS?=

BUILDDIR?=../build
#PREFIX?=/usr/local
#EXEC_PREFIX?=$(PREFIX)
#INCLUDEDIR?=$(PREFIX)/include
#LIBDIR?=$(EXEC_PREFIX)/lib

CFLAGS:=$(CFLAGS) -Wall -Wextra
CPPFLAGS:=$(CPPFLAGS) -D__kos_libc -Iinclude
LIBK_CFLAGS:=$(CFLAGS) -ffreestanding -fbuiltin
LIBK_CPPFLAGS:=$(CPPFLAGS) -D__kos_kernel
 
ARCHDIR:=arch/$(HOSTARCH)
 
include $(ARCHDIR)/make.config
 
CFLAGS:=$(CFLAGS) $(ARCH_CFLAGS)
CPPFLAGS:=$(CPPFLAGS) $(ARCH_CPPFLAGS)
LIBK_CFLAGS:=$(LIBK_CFLAGS) $(KERNEL_ARCH_CFLAGS)
LIBK_CPPFLAGS:=$(LIBK_CPPFLAGS) $(KERNEL_ARCH_CPPFLAGS)
 
FREEOBJS:=\
$(ARCH_FREEOBJS) \
string.o \
stdio/printf.o \

HOSTEDOBJS:=\
$(ARCH_HOSTEDOBJS) \
 
OBJS:=\
$(FREEOBJS) \
$(HOSTEDOBJS) \
 
#LIBK_OBJS:=$(FREEOBJS:.o=.libk.o)
 
BINARIES=libc.a libk.a #libg.a

DIRS=$(sort $(dir $(addprefix $(BUILDDIR)/libc/, $(OBJS))))
TESTDIRS=$(sort $(dir $(addprefix $(BUILDDIR)/tests/libc/, $(OBJS))))

TESTS:=\
printf_tests.cpp \

VPATH=$(BUILDDIR)/libc #$(BUILDDIR)/tests $(BUILDDIR)/tests/libc tests

all: $(DIRS) libc.a

.PHONY: all clean test
 
libc.a: $(OBJS)
	$(AR) rcs $(BUILDDIR)/$@ $(addprefix $(BUILDDIR)/libc/, $(OBJS))
 
libg.a:
	$(AR) rcs $@
 
libk.a: $(OBJS)
#	$(AR) rcs $@ $(LIBK_OBJS)
	$(AR) rcs $(BUILDDIR)/$@ $(addprefix $(BUILDDIR)/libc/, $(OBJS))

%.o: %.c
	$(CC) -c $< -o $(BUILDDIR)/libc/$@ $(CFLAGS) $(CPPFLAGS)

%.o: %.s
	$(CC) -c $< -o $(BUILDDIR)/libc/$@ $(CFLAGS) $(CPPFLAGS)

#%.libk.o: %.c
#	$(CC) -c $< -o $(BUILDDIR)/$@ $(LIBK_CFLAGS) $(LIBK_CPPFLAGS)

%.libk.o: %.s
	$(CC) -c $< -o $(BUILDDIR)/$@ $(LIBK_CFLAGS) $(LIBK_CPPFLAGS)

clean:
	rm -f $(addprefix $(BUILDDIR)/, $(BINARIES)) $(addprefix $(BUILDDIR)/libc/, $(OBJS))
	rm -f $(addprefix $(BUILDDIR)/tests/, $(BINARIES)) $(addprefix $(BUILDDIR)/tests/libc/, $(OBJS))
	rm -f $(BUILDDIR)/tests/gtest-all.o $(BUILDDIR)/tests/libgtest.a $(BUILDDIR)/tests/libc/test
	rm -rf $(BUILDDIR)/libc $(BUILDDIR)/tests/libc

$(DIRS) $(TESTDIRS):
	mkdir -p $@

test: $(TESTDIRS) libgtest.a TESTCONFIG libk.a
	g++ -isystem ../googletest/googletest/include -pthread -std=c++11 -fpermissive -D TEST tests/main.cpp $(BUILDDIR)/libgtest.a -o $(BUILDDIR)/libc/test -lk -L$(BUILDDIR)
	$(BUILDDIR)/libc/test

TESTCONFIG:
	$(eval BINS=)
	$(eval VPATH=$(BUILDDIR)/tests $(BUILDDIR)/tests/libc tests)
	$(eval BUILDDIR=$(BUILDDIR)/tests)
	$(eval CPPFLAGS=$(CPPFLAGS) -D TEST)

gtest-all.o:
	g++ -isystem ../googletest/googletest/include -I../googletest/googletest -pthread -c ../googletest/googletest/src/gtest-all.cc -o $(BUILDDIR)/tests/gtest-all.o

libgtest.a: gtest-all.o
	ar -rv $(BUILDDIR)/tests/libgtest.a $(BUILDDIR)/tests/gtest-all.o
