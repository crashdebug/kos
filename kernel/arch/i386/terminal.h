#pragma once
#include <stdint.h>
#include <stddef.h>
#include <iterminal.h>

class Terminalx86 : public ITerminal
{
	size_t _width = 80;
	size_t _height = 25;
	size_t _row;
	size_t _column;
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

#ifdef __cplusplus
extern "C" {
#endif

void install_terminal();

#ifdef __cplusplus
}
#endif
