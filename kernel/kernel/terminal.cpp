#include <terminal.h>
#include <stdio.h>

#ifdef TEST
using namespace kos;
#endif

uint8_t make_color(enum Terminal::Color fg, enum Terminal::Color bg) {
	return fg | bg << 4;
}

uint16_t make_vgaentry(char c, uint8_t color) {
	uint16_t c16 = c;
	uint16_t color16 = color;
	return c16 | color16 << 8;
}
 
size_t Terminal::terminal_width;
size_t Terminal::terminal_height;
size_t Terminal::terminal_row;
size_t Terminal::terminal_column;
uint8_t Terminal::terminal_color;
uint16_t* Terminal::terminal_buffer;

void Terminal::initialize(uint16_t* buffer, size_t width, size_t height)
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
	terminal_buffer = buffer;
	terminal_width = width;
	terminal_height = height;

	for (size_t y = 0; y < height; y++) {
		for (size_t x = 0; x < width; x++) {
			const size_t index = y * width + x;
			terminal_buffer[index] = make_vgaentry(' ', terminal_color);
		}
	}

	init_printf(&Terminal::putChar);
}

void Terminal::setColor(const Color fg, const Color bg)
{
	terminal_color = make_color(fg, bg);
}

void Terminal::putEntryAt(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * terminal_width + x;
	terminal_buffer[index] = make_vgaentry(c, color);
}

void Terminal::putChar(char c)
{
	if (c == '\n') {
		terminal_row++;
		terminal_column = -1;
	} else if (c == '\t') {
		terminal_column += 4; 
	} else {
		putEntryAt(c, terminal_color, terminal_column, terminal_row);
	}
	if (++terminal_column >= terminal_width) {
		terminal_column = 0;
		if (++terminal_row >= terminal_height) {
			terminal_row = 0;
		}
	}
}

/*void Terminal::writeString(const char* data, const Color fg, const Color bg)
{
	terminal_color = make_color(fg, bg);
	size_t datalen = strlen(data);
	for (size_t i = 0; i < datalen; i++)
		putChar(data[i]);
}*/

/*void Terminal::write(const char* text, ...)
{
	va_list args;
	va_start(args, text);
	printf(&Terminal::putChar, text, args);
	va_end(args);
}*/
