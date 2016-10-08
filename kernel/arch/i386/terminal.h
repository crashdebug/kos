#pragma once
#include <stdint.h>
#include <stddef.h>
#include <iterminal.h>

namespace kos::x86
{
	class Terminalx86 : public ITerminal
	{
		short _width = 80;
		short _height = 25;
		short _row;
		short _column;
		uint8_t _color;
		uint16_t* _buffer = (uint16_t*)0xB8000;

		void putEntryAt(char c, uint8_t color, size_t x, size_t y);
		void scroll();
	public:
		Terminalx86();
		void putChar(const char c);
		void setColor(const Terminal::Color fg, const Terminal::Color bg);
		void moveCursor(size_t x, size_t y);
	};
}

#ifdef __cplusplus
extern "C" {
#endif

void install_terminal();

#ifdef __cplusplus
}
#endif
