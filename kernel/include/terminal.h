#pragma once
#include <stdint.h>
#include <stddef.h>

class Terminal
{
	static size_t terminal_row;
	static size_t terminal_column;
	static uint8_t terminal_color;
	static uint16_t* terminal_buffer;
	static size_t terminal_width;
	static size_t terminal_height;

	static void putEntryAt(char c, uint8_t color, size_t x, size_t y);

public:
	/* Hardware text mode color constants. */
	enum Color {
		COLOR_BLACK = 0,
		COLOR_BLUE = 1,
		COLOR_GREEN = 2,
		COLOR_CYAN = 3,
		COLOR_RED = 4,
		COLOR_MAGENTA = 5,
		COLOR_BROWN = 6,
		COLOR_LIGHT_GREY = 7,
		COLOR_DARK_GREY = 8,
		COLOR_LIGHT_BLUE = 9,
		COLOR_LIGHT_GREEN = 10,
		COLOR_LIGHT_CYAN = 11,
		COLOR_LIGHT_RED = 12,
		COLOR_LIGHT_MAGENTA = 13,
		COLOR_LIGHT_BROWN = 14,
		COLOR_WHITE = 15,
	};
	static void initialize(uint16_t* buffer, size_t width, size_t height);
	//static void writeString(const char* data, const Color fg = COLOR_LIGHT_GREY, const Color bg = COLOR_BLACK);
	//static void write(const char* text, ...);
	static void putChar(char c);
	static void setColor(const Color fg, const Color bg);
};
