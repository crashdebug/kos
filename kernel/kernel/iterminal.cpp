#include "iterminal.h"
#include <stdarg.h>

extern int print_args(const char*, va_list* args);

static kos::ITerminal* s_terminal = 0;

/* Called from stdio.h (putc) */
void put_char(char c)
{
	s_terminal->putChar(c);
}

namespace kos
{
	/* Called from architecture specific methods */ 
	void set_terminal(kos::ITerminal* terminal)
	{
		s_terminal = terminal;
	}

	void Terminal::putChar(const char c)
	{
		s_terminal->putChar(c);
	}

	void Terminal::moveCursor(size_t x, size_t y)
	{
		s_terminal->moveCursor(x, y);
	}

	void Terminal::setColor(const Color fg, const Color bg)
	{
		s_terminal->setColor(fg, bg);
	}

	void Terminal::write(const char* text, ...)
	{
		va_list args;
		va_start(args, text);

		print_args(text, &args);

		va_end(args);
	}
}
