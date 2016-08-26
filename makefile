BUILDDIR?=./build

TESTS=memory.cpp \
	printf.cpp \
	terminal.cpp \

all: kernel

.PHONY: all test kernel iso run install binutils gcc

test:
	cd ./libc ; \
	make test BUILDDIR=$(BUILDDIR)
	cd ./kernel ; \
	make test BUILDDIR=$(BUILDDIR)

kernel $(BUILDDIR)/kos.bin: $(BUILDDIR)/libc.a
	cd ./kernel ; \
	make BUILDDIR=$(BUILDDIR) 

$(BUILDDIR)/libc.a:
	cd ./libc ; \
	make BUILDDIR=$(BUILDDIR)

$(BUILDDIR)/libgtest.a:
	g++ -isystem googletest/googletest/include -Igoogletest/googletest -pthread -c googletest/googletest/src/gtest-all.cc -o $(BUILDDIR)/gtest-all.o
	ar -rv $@ $(BUILDDIR)/gtest-all.o

iso $(BUILDDIR)/kos.iso: $(BUILDDIR)/kos.bin grub.cfg
	mkdir -p $(BUILDDIR)/iso/boot/grub
	cp grub.cfg $(BUILDDIR)/iso/boot/grub/
	cp $(BUILDDIR)/kos.bin $(BUILDDIR)/iso/boot/
	grub-mkrescue -o $(BUILDDIR)/kos.iso iso
	rm $(BUILDDIR)/iso/boot/kos.bin $(BUILDDIR)/iso/boot/grub/grub.cfg
	rmdir -p $(BUILDDIR)/iso/boot/grub

run : $(BUILDDIR)/kos.bin
	qemu-system-i386 -kernel $(BUILDDIR)/kos.bin

install : binutils gcc

binutils :
	git clone git://sourceware.org/git/binutils-gdb.git ./tools/src/binutils
	mkdir ./tools/src/binutils-build
	cd ./tools/src/binutils-build
	../binutils/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror

gcc :
	svn checkout svn://gcc.gnu.org/svn/gcc/trunk ./gcc
