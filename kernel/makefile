HOST?=$(shell ../scripts/default-host.sh)
HOSTARCH:=$(shell ../scripts/target-triplet-to-arch.sh $(HOST))

ifneq ($(MAKECMDGOALS),test)
BINS=../tools/bin/$(HOST)-
BUILDDIR?=../build
CPPFLAGS?=
LIBFLAGS=
else
BINS=
BUILDDIR?=../build/tests
CPPFLAGS=-DTEST -D__lib -fpermissive
LIBFLAGS=$(BUILDDIR)/libk.a BINS= CPPFLAGS="-D TEST -D__kos_libc -Iinclude"
endif

CC=g++
CPP=g++
AR=ar
AS=as

CFLAGS?=-O2
LDFLAGS?=
LIBS?=

CFLAGS:=$(CFLAGS) -ffreestanding -fbuiltin -Wall -Wextra -fno-exceptions -Iinclude -I../libc/include
CPPFLAGS:=$(CPPFLAGS) -fno-rtti 
LDFLAGS:=$(LDFLAGS)
LIBS:=$(LIBS) -nostdlib -lgcc -lc -L$(BUILDDIR)

ARCHDIR:=arch/$(HOSTARCH)
 
include $(ARCHDIR)/make.config

CFLAGS:=$(CFLAGS) $(KERNEL_ARCH_CFLAGS)
CPPFLAGS:=$(CPPFLAGS) $(KERNEL_ARCH_CPPFLAGS)
LDFLAGS:=$(LDFLAGS) $(KERNEL_ARCH_LDFLAGS)
LIBS:=$(LIBS) $(KERNEL_ARCH_LIBS)
 
OBJS:=\
kernel/kernel.o \

LIBOBJS:=\
kernel/terminal.o \
kernel/memory.o \
kernel/pagebitmap.o \
kernel/pageblockmap.o \
kernel/new.o \

TEMPLATES:=\
include/vector.h \

CRTI_OBJ:=$(ARCHDIR)/crti.o
CRTBEGIN_OBJ:=$(shell $(BINS)$(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(BINS)$(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=crtend.o)
CRTN_OBJ:=$(ARCHDIR)/crtn.o

ALL_OUR_OBJS:=\
$(KERNEL_ARCH_OBJS) \
$(CRTI_OBJ) \
$(OBJS) \
$(LIBOBJS) \
$(CRTN_OBJ) \

OBJ_LINK_LIST:=\
$(ALL_OUR_OBJS) \
$(CRTBEGIN_OBJ) \
$(CRTEND_OBJ) \

DIRS=$(sort $(dir $(addprefix $(BUILDDIR)/kernel/, $(ALL_OUR_OBJS))))

all: $(BUILDDIR)/kos.bin

.PHONY: all clean run test

$(BUILDDIR)/kos.bin: $(DIRS) $(addprefix $(BUILDDIR)/kernel/, $(ALL_OUR_OBJS)) $(ARCHDIR)/linker.ld $(BUILDDIR)/libc.a
	$(BINS)$(CC) -T $(ARCHDIR)/linker.ld -o $@ $(CFLAGS) $(addprefix $(BUILDDIR)/kernel/, $(ALL_OUR_OBJS)) $(CRTBEGIN_OBJ) $(CRTEND_OBJ) $(LDFLAGS) $(LIBS)

$(BUILDDIR)/kernel/%.o: %.cpp
	$(BINS)$(CPP) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

$(BUILDDIR)/kernel/%.o: %.c
	$(BINS)$(CC) -c $< -o $@ $(CFLAGS)

$(BUILDDIR)/kernel/%.o: %.s
	$(BINS)$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

$(BUILDDIR)/libc.a:
	cd ../libc ; \
	make $(LIBFLAGS) BUILDDIR=$(BUILDDIR)

$(DIRS):
	mkdir -p $@

clean: 
	rm -f $(BUILDDIR)/kos.bin $(BUILDDIR)/libkos.a $(addprefix $(BUILDDIR)/kernel/, $(ALL_OUR_OBJS))
	rm -f $(addprefix $(BUILDDIR)/tests/kernel/, $(ALL_OUR_OBJS)) $(BUILDDIR)/tests/libkos.a
	rm -f $(BUILDDIR)/tests/gtest-all.o $(BUILDDIR)/tests/libgtest.a $(BUILDDIR)/tests/kernel/test
	rm -rf $(BUILDDIR)/kernel $(BUILDDIR)/tests/kernel 

run: $(BUILDDIR)/kos.bin
	qemu-system-i386 -kernel $(BUILDDIR)/kos.bin

$(BUILDDIR)/libkos.a: $(DIRS) $(addprefix $(BUILDDIR)/kernel/, $(LIBOBJS)) $(ARCHDIR)/linker.ld $(BUILDDIR)/libc.a
	$(BINS)$(AR) rcsT $(BUILDDIR)/libkos.a $(addprefix $(BUILDDIR)/kernel/, $(LIBOBJS))
	$(BINS)$(AR) qT $(BUILDDIR)/libkos.a $(BUILDDIR)/libk.a

test: $(DIRS) $(BUILDDIR)/libgtest.a $(BUILDDIR)/libkos.a
	g++ -isystem ../googletest/googletest/include -pthread -std=c++11 -fpermissive -Iinclude -g -DTEST tests/main.cpp $(BUILDDIR)/libgtest.a -o $(BUILDDIR)/kernel/test -lkos -L$(BUILDDIR)
	$(BUILDDIR)/kernel/test

$(BUILDDIR)/gtest-all.o:
	g++ -isystem ../googletest/googletest/include -I../googletest/googletest -pthread -c ../googletest/googletest/src/gtest-all.cc -o $(BUILDDIR)/gtest-all.o

$(BUILDDIR)/libgtest.a: $(BUILDDIR)/gtest-all.o
	ar -rv $(BUILDDIR)/libgtest.a $(BUILDDIR)/gtest-all.o
