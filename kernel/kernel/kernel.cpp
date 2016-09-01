#include <stdint.h>
#include <stdio.h>
#include <terminal.h>
#include <multiboot.h>
#include <time.h>

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

unsigned long long _time = 0;
extern void set_time_fn(time_t(*fn)(void));

unsigned long long ticks()
{
	return _time;
}

void set_ticks(unsigned long long t)
{
	_time = t;
}

void kernel_early()
{
	set_time_fn(&ticks);
}

void kernel_main(multiboot_info_t* mbd, uint32_t magic)
{
	Terminal::initialize((uint16_t*)0xB8000, 80, 25);
	Terminal::setColor(Terminal::Color::COLOR_LIGHT_GREY, Terminal::Color::COLOR_BLUE);
	printf("[");
	Terminal::setColor(Terminal::Color::COLOR_WHITE, Terminal::Color::COLOR_BLUE);
	printf("kOS ");
	Terminal::setColor(Terminal::Color::COLOR_CYAN, Terminal::Color::COLOR_BLUE);
	printf("v0.0.0");
	Terminal::setColor(Terminal::Color::COLOR_LIGHT_GREY, Terminal::Color::COLOR_BLUE);
	printf("]\n");
	Terminal::setColor(Terminal::Color::COLOR_LIGHT_GREY, Terminal::Color::COLOR_BLACK);
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
