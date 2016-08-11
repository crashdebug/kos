#include "terminal.h"
#include "multiboot.h"

/* Check if the compiler thinks we are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This needs to be compiled with a ix86-elf compiler"
#endif

using namespace kos;

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif
void kernel_main(multiboot_info_t* mbd) {
	/* Initialize terminal interface */
	Terminal::initialize((uint16_t*)0xB8000, 80, 25);
	Terminal::printf("[kOS v0.0.1]\n");
	Terminal::printf("Magic: %x\n", mbd);
	Terminal::printf("");
	/*Terminal::writeString("[ ", Terminal::Color::COLOR_DARK_GREY, Terminal::Color::COLOR_BLUE);
	Terminal::writeString("k", Terminal::Color::COLOR_LIGHT_CYAN, Terminal::Color::COLOR_BLUE);
	Terminal::writeString("OS", Terminal::Color::COLOR_CYAN, Terminal::Color::COLOR_BLUE);
	Terminal::writeString(" v", Terminal::Color::COLOR_LIGHT_GREY, Terminal::Color::COLOR_BLUE);
	Terminal::writeString("0.0.0.1", Terminal::Color::COLOR_WHITE, Terminal::Color::COLOR_BLUE);
	Terminal::writeString(" ]\n", Terminal::Color::COLOR_DARK_GREY, Terminal::Color::COLOR_BLUE);*/
}
