#include "terminal.h"
#include <stdio.h>
#include <string.h>
#include "util.h"

using namespace kos;

namespace kos::x86
{

	uint8_t make_color(enum Terminal::Color fg, enum Terminal::Color bg) {
		return fg | bg << 4;
	}

	uint16_t make_vgaentry(char c, uint8_t color) {
		uint16_t c16 = c;
		uint16_t color16 = color;
		return c16 | color16 << 8;
	}

	Terminalx86::Terminalx86()
	{
		this->_column = 0;
		this->_row = 0;
		this->_color = make_color(Terminal::Color::COLOR_LIGHT_GREY, Terminal::Color::COLOR_BLACK);
		for (short y = 0; y < this->_height; y++) {
			for (short x = 0; x < this->_width; x++) {
				const short index = y * this->_width + x;
				this->_buffer[index] = make_vgaentry(' ', this->_color);
			}
		}
	}

	void Terminalx86::setColor(const Terminal::Color fg, const Terminal::Color bg)
	{
		this->_color = make_color(fg, bg);
	}

	void Terminalx86::putEntryAt(char c, uint8_t color, size_t x, size_t y)
	{
		const size_t index = y * this->_width + x;
		this->_buffer[index] = make_vgaentry(c, color);
	}

	void Terminalx86::putChar(const char c)
	{
		switch (c)
		{
		// New line
		case '\n':
			this->_row++;
			this->_column = -1;
			break;
		// Carriage return
		case '\r':
			this->_column = -1;
			break;
		// Tab
		case '\t':
			this->_column += 4;
			break; 
		// Backspace
		case '\b':
			if (this->_column > 0)
			{
				this->_column--;
			}
			else if (this->_row > 0)
			{
				this->_row--;
			}
			else
			{
				return;
			}
			this->putEntryAt(' ', this->_color, this->_column, this->_row);
			this->_column--;
			break;
		default:
			this->putEntryAt(c, this->_color, this->_column, this->_row);
			break;
		}
		if (++this->_column >= this->_width) {
			this->_column = 0;
			if (++this->_row >= this->_height) {
				this->scroll();
				this->_row--;
			}
		}
		this->moveCursor(this->_column, this->_row);
	}

	void Terminalx86::scroll()
	{
		memcpy(this->_buffer, this->_buffer + this->_width * 2, this->_width * (this->_height - 1) * 2);
		for (short x = 0; x < this->_width; x++)
		{
			const size_t index = (this->_height - 1) * this->_width + x;
			this->_buffer[index] = make_vgaentry(' ', this->_color);
		}
	}

	void Terminalx86::moveCursor(size_t x, size_t y)
	{
		// The equation for finding the index in a linear chunk of memory can be represented by:
		// Index = [(y * width) + x]
		size_t temp = y * this->_width + x;

		// This sends a command to indicies 14 and 15 in the CRT Control Register of the VGA controller.
		// These are the high and low bytes of the index that show where the hardware cursor is to be 'blinking'.
		outb(0x3D4, 14);
		outb(0x3D5, temp >> 8);
		outb(0x3D4, 15);
		outb(0x3D5, temp);
	}
}

#ifdef __cplusplus
extern "C" {
#endif

void install_terminal()
{
	ITerminal* term = new x86::Terminalx86();
	set_terminal(term);
}

#ifdef __cplusplus
}
#endif
