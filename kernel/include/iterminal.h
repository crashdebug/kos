#pragma once
#ifndef _KOS_ITERMINAL
#define _KOS_ITERMINAL 1

#include <stddef.h>

namespace kos
{
	class Terminal
	{
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

		static void setColor(const Color fg, const Color bg);
		static void write(const char* text, ...);
		static void putChar(const char c);
		static void moveCursor(size_t x, size_t y);
		static const char* readLine();
	};

	class ITerminal
	{
	public:
		virtual void setColor(const Terminal::Color fg, const Terminal::Color bg) = 0;
		virtual void putChar(const char c) = 0;
		virtual void moveCursor(size_t x, size_t y) = 0;
		ITerminal* operator <<(char c)
		{
			this->putChar(c);
			return this;
		}
	};

	void set_terminal(ITerminal* terminal);
}

#endif
