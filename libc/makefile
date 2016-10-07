HOST?=$(shell ../scripts/default-host.sh)
HOSTARCH:=$(shell ../scripts/target-triplet-to-arch.sh $(HOST))

CC=g++
AR=ar
AS=as

CFLAGS?=-O2 -g
LDFLAGS?=
LIBS?=

ifeq ($(MAKECMDGOALS),test)
BUILDDIR?=../build/tests
BINS=
CPPFLAGS?=-DTEST
else
BUILDDIR?=../build
BINS?=../tools/bin/$(HOST)-
CPPFLAGS?=-D_KOS
endif

CFLAGS:=$(CFLAGS) -Wall -Wextra -Iinclude
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
time.o \
stdio/printf.o \

HOSTEDOBJS:=\
$(ARCH_HOSTEDOBJS) \
 
OBJS:=\
$(FREEOBJS) \
$(HOSTEDOBJS) \
 
#LIBK_OBJS:=$(FREEOBJS:.o=.libk.o)
 
BINARIES=libc.a libk.a #libg.a

DIRS=$(sort $(dir $(addprefix $(BUILDDIR)/libc/, $(OBJS))))

TESTS:=\
printf_tests.cpp \

all: $(DIRS) $(BUILDDIR)/libc.a

.PHONY: all clean test
 
$(BUILDDIR)/libc.a: $(addprefix $(BUILDDIR)/libc/, $(OBJS))
	$(BINS)$(AR) rcs $@ $(addprefix $(BUILDDIR)/libc/, $(OBJS))
 
$(BUILDDIR)/libg.a:
	$(BINS)$(AR) rcs $@

libk $(BUILDDIR)/libk.a: $(DIRS) $(addprefix $(BUILDDIR)/libc/, $(OBJS))
	$(BINS)$(AR) rcs $(BUILDDIR)/libk.a $(addprefix $(BUILDDIR)/libc/, $(OBJS))
#	$(AR) rcs $@ $(LIBK_OBJS)

$(BUILDDIR)/libc/%.o: %.c
	$(BINS)$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

$(BUILDDIR)/libc/%.o: %.s
	$(BINS)$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

#%.libk.o: %.c
#	$(CC) -c $< -o $(BUILDDIR)/$@ $(LIBK_CFLAGS) $(LIBK_CPPFLAGS)

#%.libk.o: %.s
#	$(CC) -c $< -o $(BUILDDIR)/$@ $(LIBK_CFLAGS) $(LIBK_CPPFLAGS)

clean:
	rm -f $(addprefix $(BUILDDIR)/, $(BINARIES)) $(addprefix $(BUILDDIR)/libc/, $(OBJS))
	rm -f $(addprefix $(BUILDDIR)/tests/, $(BINARIES)) $(addprefix $(BUILDDIR)/tests/libc/, $(OBJS))
	rm -f $(BUILDDIR)/tests/gtest-all.o $(BUILDDIR)/tests/libgtest.a $(BUILDDIR)/tests/libc/test
	rm -rf $(BUILDDIR)/libc $(BUILDDIR)/tests/libc

$(DIRS):
	mkdir -p $@

test: $(BUILDDIR)/libgtest.a $(BUILDDIR)/libk.a
	$(CC) -isystem ../googletest/googletest/include -pthread -std=c++11 -fpermissive -g -DTEST tests/main.cpp $(BUILDDIR)/libgtest.a -o $(BUILDDIR)/libc/test -lk -L$(BUILDDIR)
	$(BUILDDIR)/libc/test

$(BUILDDIR)/gtest-all.o:
	$(CC) -isystem ../googletest/googletest/include -I../googletest/googletest -pthread -c ../googletest/googletest/src/gtest-all.cc -o $(BUILDDIR)/gtest-all.o

$(BUILDDIR)/libgtest.a: $(BUILDDIR)/gtest-all.o
	$(AR) -rv $(BUILDDIR)/libgtest.a $(BUILDDIR)/gtest-all.o
