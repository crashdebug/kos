HOST?=$(shell ../scripts/default-host.sh)
HOSTARCH:=$(shell ../scripts/target-triplet-to-arch.sh $(HOST))

BINS=../tools/bin

CC=$(BINS)/$(HOST)-gcc
CPP=$(BINS)/$(HOST)-g++
AR=$(BINS)/$(HOST)-ar
AS=$(BINS)/$(HOST)-as

CFLAGS?=-O2 -g
CPPFLAGS?=
LDFLAGS?=
LIBS?=

PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
BOOTDIR?=$(EXEC_PREFIX)/boot
INCLUDEDIR?=$(PREFIX)/include

CFLAGS:=$(CFLAGS) -ffreestanding -fbuiltin -Wall -Wextra -fno-exceptions -fno-rtti
CPPFLAGS:=$(CPPFLAGS) -Iinclude -I../libc/include 
LDFLAGS:=$(LDFLAGS)
LIBS:=$(LIBS) -nostdlib -lgcc -lk -L../libc

ARCHDIR:=arch/$(HOSTARCH)
 
include $(ARCHDIR)/make.config

CFLAGS:=$(CFLAGS) $(KERNEL_ARCH_CFLAGS)
CPPFLAGS:=$(CPPFLAGS) $(KERNEL_ARCH_CPPFLAGS)
LDFLAGS:=$(LDFLAGS) $(KERNEL_ARCH_LDFLAGS)
LIBS:=$(LIBS) $(KERNEL_ARCH_LIBS)
 
OBJS:=\
$(KERNEL_ARCH_OBJS) \
kernel/kernel.o \
kernel/terminal.o \
kernel/memory.o \
kernel/new.o \
kernel/pagebitmap.o \
kernel/pageblockmap.o \

CRTI_OBJ:=$(ARCHDIR)/crti.o
CRTBEGIN_OBJ:=$(shell $(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=crtend.o)
CRTN_OBJ:=$(ARCHDIR)/crtn.o
 
ALL_OUR_OBJS:=\
$(CRTI_OBJ) \
$(OBJS) \
$(CRTN_OBJ) \
 
OBJ_LINK_LIST:=\
$(OBJS) \
$(CRTI_OBJ) \
$(CRTN_OBJ) \
$(CRTBEGIN_OBJ) \
$(CRTEND_OBJ) \

all: kos.bin

.PHONY: all clean run

kos.bin: $(OBJ_LINK_LIST) $(ARCHDIR)/linker.ld
	$(CC) -T $(ARCHDIR)/linker.ld -o $@ $(CFLAGS) $(OBJ_LINK_LIST) $(LDFLAGS) $(LIBS)

%.o: %.cpp
	$(CPP) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)
 
%.o: %.s
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

clean: 
	rm -f kos.bin $(OBJS) $(ALL_OUR_OBJS) *.o */*.o */*/*.o

run : kos.bin
	qemu-system-i386 -kernel kos.bin
