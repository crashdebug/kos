sources = ./src/kernel.cpp ./src/terminal.cpp ./src/printf.cpp
objects = ./build/boot.o ./build/kernel.o ./build/terminal.o ./build/printf.o
images = ./build/kos.bin ./build/kos.iso
tests = ./test/printf.cpp

kos.bin : boot.o kernel
	./tools/bin/i686-elf-gcc -T ./src/linker.ld -o ./build/kos.bin -ffreestanding -O2 -nostdlib $(objects) -lgcc

boot.o : ./build ./src/boot.s
	./tools/bin/i686-elf-as ./src/boot.s -o ./build/boot.o

kernel : ./build $(sources)
	cd ./build ; \
	../tools/bin/i686-elf-g++ -c $(addprefix ., $(sources)) -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti

./build :
	mkdir ./build

#######################################################

.PHONY : clean
clean :
	rm -f $(objects) $(images) ./build/tests

#######################################################

run : kos.bin
	qemu-system-i386 -kernel ./build/kos.bin

iso kos.iso : kos.bin
	mkdir -p ./iso/boot/grub
	cp ./src/grub.cfg ./iso/boot/grub/
	cp ./build/kos.bin ./iso/boot/
	grub-mkrescue -o ./build/kos.iso iso
	rm ./iso/boot/kos.bin ./iso/boot/grub/grub.cfg
	rmdir -p iso/boot/grub

#######################################################

install : binutils gcc

binutils :
	git clone git://sourceware.org/git/binutils-gdb.git ./tools/src/binutils
	mkdir ./tools/src/binutils-build
	cd ./tools/src/binutils-build
	../binutils/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror

gcc :
	svn checkout svn://gcc.gnu.org/svn/gcc/trunk ./gcc

test : ./build
#	g++ -isystem googletest/googletest/include -Igoogletest/googletest -pthread -c googletest/googletest/src/gtest-all.cc
#	ar -rv libgtest.a gtest-all.o
	g++ -isystem googletest/googletest/include -pthread ./tests/main.cpp libgtest.a -o ./build/tests
	./build/tests
