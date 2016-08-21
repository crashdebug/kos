HOST?=$(shell ../scripts/default-host.sh)
HOSTARCH:=$(shell ../scripts/target-triplet-to-arch.sh $(HOST))

BINS=../tools/bin

CC=$(BINS)/$(HOST)-g++
AR=$(BINS)/$(HOST)-ar
AS=$(BINS)/$(HOST)-as

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
 
LIBK_OBJS:=$(FREEOBJS:.o=.libk.o)
 
BINARIES=libc.a #libg.a libk.a

DIRS=$(sort $(dir $(addprefix $(BUILDDIR)/libc/, $(OBJS))))

#reverse = $(if $(1),$(call reverse,$(wordlist 2,$(words $(1)),$(1)))) $(firstword $(1))

VPATH = $(BUILDDIR)/libc

all: $(DIRS) $(BINARIES)

.PHONY: all clean
 
libc.a: $(OBJS)
	$(AR) rcs $(BUILDDIR)/$@ $(addprefix $(BUILDDIR)/libc/, $(OBJS))
 
libg.a:
	$(AR) rcs $@
 
libk.a: $(LIBK_OBJS)
	$(AR) rcs $@ $(LIBK_OBJS)

%.o: %.c
	$(CC) -c $< -o $(BUILDDIR)/libc/$@ $(CFLAGS) $(CPPFLAGS)

%.o: %.s
	$(CC) -c $< -o $(BUILDDIR)/libc/$@ $(CFLAGS) $(CPPFLAGS)

%.libk.o: %.c
	$(CC) -c $< -o $(BUILDDIR)/$@ $(LIBK_CFLAGS) $(LIBK_CPPFLAGS)

%.libk.o: %.s
	$(CC) -c $< -o $(BUILDDIR)/$@ $(LIBK_CFLAGS) $(LIBK_CPPFLAGS)

clean:
	rm -f $(addprefix $(BUILDDIR)/, $(BINARIES)) $(addprefix $(BUILDDIR)/libc/, $(OBJS))
	rm -rf $(BUILDDIR)/libc

$(DIRS):
	mkdir -p $@
