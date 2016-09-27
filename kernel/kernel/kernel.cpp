#include <kernel.h>
#include <stdint.h>
#include <stdio.h>
#include <terminal.h>
#include <multiboot.h>
#include <time.h>
#include <memory.h>
#include <vector.h>
#include <idriver.h>

/* Check if the compiler thinks we are targeting the wrong operating system. */ 
#if defined(__linux__) 
#error "You are not using a cross-compiler" 
#endif

/* This will only work for the 32-bit ix86 targets. */ 
#if !defined(__i386__) 
#error "This needs to be compiled with a ix86-elf compiler" 
#endif

vector<IDriver*> _drivers;
time_t _time = 0;

#ifdef __cplusplus
extern "C" {
#endif

time_t ticks()
{
	return _time;
}

void set_ticks(time_t t)
{
	_time = t;
}

void install_driver(IDriver* driver)
{
	_drivers.push_back(driver);
}

void kernel_early(multiboot_info_t* mbd, uint32_t magic)
{
	Terminal::initialize((uint16_t*)0xB8000, 80, 25);
	printf("BootInfo: 0x%8x, Magic: 0x%x\n", mbd, magic);

	MemoryManager::Initialize(mbd);
	printf("MemoryManager initialized (%i kb available)\n", MemoryManager::Available() / 1024);
}

void kernel_main()
{
	for (unsigned int i = 0; i < _drivers.size(); i++)
	{
		_drivers.at(i)->install();
	}

	Terminal::setColor(Terminal::Color::COLOR_LIGHT_GREY, Terminal::Color::COLOR_BLUE);
	printf("[");
	Terminal::setColor(Terminal::Color::COLOR_WHITE, Terminal::Color::COLOR_BLUE);
	printf("kOS ");
	Terminal::setColor(Terminal::Color::COLOR_CYAN, Terminal::Color::COLOR_BLUE);
	printf("v0.0.0");
	Terminal::setColor(Terminal::Color::COLOR_LIGHT_GREY, Terminal::Color::COLOR_BLUE);
	printf("]\n");
	Terminal::setColor(Terminal::Color::COLOR_LIGHT_GREY, Terminal::Color::COLOR_BLACK);

	unsigned long long l = 0;
	while (true)
	{
		time_t t = time(0);
		struct tm* time = gmtime(&t);
		printf("\rDate: %2i.%2i.%4i %2i:%2i:%2i [%i]", time->tm_mday, time->tm_mon, time->tm_year, time->tm_hour, time->tm_min, time->tm_sec, l);
		delete time;
		l++;
	}
}

void __cxa_finalize(void *)
{
}

int __cxa_atexit(void (*)(void *), void *, void *)
{
	return 0;
}

// Called when a virtual method has not been overriddent.
void __cxa_pure_virtual()
{
    // Do nothing or print an error message.
}

#ifdef __cplusplus
}
#endif
