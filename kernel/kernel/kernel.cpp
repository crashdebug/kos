#include <kernel.h>
#include <stdint.h>
#include <stdio.h>
#include <iterminal.h>
#include <multiboot.h>
#include <time.h>
#include <memory.h>
#include <vector.h>
#include <idriver.h>
#include <command.h>

/* Check if the compiler thinks we are targeting the wrong operating system. */ 
#if defined(__linux__) 
#error "You are not using a cross-compiler" 
#endif

/* This will only work for the 32-bit ix86 targets. */ 
#if !defined(__i386__) 
#error "This needs to be compiled with a ix86-elf compiler" 
#endif

using namespace kos;

static vector<IDriver*> s_drivers;
static time_t s_time = 0;

static keycode_t s_keyBuffer[32];
volatile uint8_t s_keyBufferPos = 0;

void install_driver(IDriver* driver)
{
	s_drivers.push_back(driver);
}

keycode_t read_key()
{
	while (s_keyBufferPos == 0)
	{
	}
	keycode_t key = s_keyBuffer[0];
	if (s_keyBufferPos > 1)
	{
		memcpy(s_keyBuffer, s_keyBuffer + 1, sizeof(keycode_t) * (s_keyBufferPos - 1));
	}
	s_keyBufferPos--;
	return key;
}

#ifdef __cplusplus
extern "C" {
#endif

time_t ticks()
{
	return s_time;
}

void set_ticks(time_t t)
{
	s_time = t;
}

void key_pressed(keycode_t key)
{
	if (s_keyBufferPos < sizeof(s_keyBuffer))
	{
		s_keyBuffer[s_keyBufferPos++] = key;
	}
}

void kernel_early(multiboot_info_t* mbd, uint32_t magic)
{
	Terminal::write("BootInfo: 0x%8x, Magic: 0x%x\n", mbd, magic);

	MemoryManager::Initialize(mbd);
	Terminal::write("MemoryManager initialized (%i kb available)\n", MemoryManager::Available() / 1024);
}

void kernel_main()
{
	for (unsigned int i = 0; i < s_drivers.size(); i++)
	{
		s_drivers.at(i)->install();
	}

	Terminal::setColor(Terminal::Color::COLOR_LIGHT_GREY, Terminal::Color::COLOR_BLUE);
	Terminal::write("[");
	Terminal::setColor(Terminal::Color::COLOR_WHITE, Terminal::Color::COLOR_BLUE);
	Terminal::write("kOS ");
	Terminal::setColor(Terminal::Color::COLOR_CYAN, Terminal::Color::COLOR_BLUE);
	Terminal::write("v%s", KOS_VERSION_STR);
	Terminal::setColor(Terminal::Color::COLOR_LIGHT_GREY, Terminal::Color::COLOR_BLUE);
	Terminal::write("]\n");
	Terminal::setColor(Terminal::Color::COLOR_LIGHT_GREY, Terminal::Color::COLOR_BLACK);

	vector<char>* cmdbuffer = new vector<char>(255);
	cmdbuffer->push_back(0);
	
	while (true)
	{
		time_t t = time(0);
		struct tm* time = gmtime(&t);
		Terminal::write("\r[%4i-%2i-%2i %2i:%2i:%2i.%3i]> %s", time->tm_year, time->tm_mon, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec, (unsigned int)(t % 1000), cmdbuffer->begin());
		delete time;

		keycode_t key = read_key();
		switch (key.Key)
		{
			case '\n':
				Terminal::write("\n");
				if (cmdbuffer->size() > 1)
				{
					ICommand* cmd = CommandParser::Parse(cmdbuffer->begin());
					if (cmd != 0)
					{
						cmd->Execute();
						delete cmd;
					}
					cmdbuffer->clear();
					cmdbuffer->push_back(0);
				}
				break;
			case '\b':
				Terminal::write("\b");
				cmdbuffer->pop_back();
				cmdbuffer->pop_back();
				cmdbuffer->push_back(0);
				break;
			default:
				cmdbuffer->pop_back();
				cmdbuffer->push_back(key.Key);
				cmdbuffer->push_back(0);
				break;
		}
	}
}

// Called when a virtual method has not been overriddent.
void __cxa_pure_virtual()
{
    // Do nothing or print an error message.
}

#ifdef __cplusplus
}
#endif
