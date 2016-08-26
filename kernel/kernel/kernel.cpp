#include <stdint.h>
#include <stdio.h>
#include <terminal.h>
#include <multiboot.h>

/* Check if the compiler thinks we are targeting the wrong operating system. */ 
#if defined(__linux__) 
#error "You are not using a cross-compiler" 
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */ 
#if !defined(__i386__) 
#error "This needs to be compiled with a ix86-elf compiler" 
#endif

#ifdef __cplusplus
extern "C" {
#endif

void kernel_early()
{
	
}

void kernel_main(multiboot_info_t* mbd, uint32_t magic)
{
	Terminal::initialize((uint16_t*)0xB8000, 80, 25);
	printf("[kOS v0.0.1]\n");
	printf("BootInfo: 0x%8x, Magic: 0x%x\n", mbd, magic);
	//Terminal::write("\n");
}

void __cxa_pure_virtual()
{
    // Do nothing or print an error message.
}

#ifdef __cplusplus
}
#endif
