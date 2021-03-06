#include <stdio.h>
#include <stdarg.h>

#define FORMAT_BUFLEN 256
char formatBuffer[FORMAT_BUFLEN];

static void(*s_putchr)(char) = 0;

int itos(long long l, unsigned char radix, char* buffer, int len)
{
	char* offset = buffer;
	// Insert the zero terminator at the end of the string
	offset[0] = '\0';
	long long num = l;
	int chars = 0;
	do
	{
		char temp = (char)(num % radix);
		// Move left one character (we're going from smallest to largest)
		offset--;
		// Is a regular digit good enough?
		if (temp < 10)
		{
			*offset = temp + '0';
		}
		// No, we need hex
		else
		{
			*offset = temp - 10 + 'a';
		}
		// Reduce what has been done so far
		num /= radix;
		chars++;
	}
	// Loop while some number is left
	while (num != 0 && buffer - offset < len);
	return chars;
}

/*****************************************************************************
	Formats and outputs the specified number in the specified radix.
*****************************************************************************/
void formatNumber(long long num, unsigned char radix, int width, char pad)
{
	char* offset;
	if (width > FORMAT_BUFLEN)
	{
		width = FORMAT_BUFLEN;
	}
	// Go to the end of the buffer.
	offset = formatBuffer + FORMAT_BUFLEN - 1;
	int len = itos(num, radix, offset, FORMAT_BUFLEN);
	offset -= len;

	while (len < width )
	{
		offset--;
		*offset = pad;
		len++;
	}

	// Output
	for (; *offset != '\0'; offset++)
	{
		putc(*offset);
	}
}

int print_args(const char* __restrict text, va_list* args)
{
	unsigned char state = 0;
	unsigned int width = 0;

	// Loop thru each character in sText
	for (const char *p = text; *p; p++)
	{
		switch (state)
		{
			// STATE 0 = AWAITING
			case 0:
				// Not a '%' -> output normally
				if (*p != '%')
				{
					putc(*p);
				}
				// Found a '%' -> next state
				else
				{
					state++;
				}
				break;
			// STATE 1 = AWAITING WIDTH
			case 1:
				if (*p >= '0' && *p <= '9')
				{
					width = 10 * width + (*p - '0');
					break;
				}
				state++;
			// STATE 2 = AWAITING FORMAT
			case 2:
				// Escaped '%' -> output normally
				if (*p == '%')
				{
					putc(*p);
				}
				// d = digits
				else if (*p == 'i')
				{
					formatNumber(va_arg(*args, int), 10, width, '0');
				}
				// x = hex
				else if (*p == 'x')
				{
					formatNumber(va_arg(*args, int), 16, width, '0');
				}
				else if (*p == 'l')
				{
					formatNumber(va_arg(*args, unsigned long long), 10, width, '0');
				}
				// p = pointer
				else if (*p == 'p')
				{
					formatNumber(va_arg(*args, int), 16, width, '0');
				}
				// s = string
				else if (*p == 's')
				{
					char* s = va_arg(*args, char*);
					for (unsigned int i = 0; s[i] != 0; i++)
					{
						putc(s[i]);
					}
				}
				else
				{
					putc(*p);
				}
				// Reset
				state = 0;
				width = 0;
				break;
		}
	}
	return 0;
}

#ifdef TEST
namespace kos {
#endif

int printf(const char* __restrict text, ...)
{
	va_list args;
	va_start(args, text);
	int ret = print_args(text, &args);
	va_end(args);
	return ret;
}

/*void printf(void(*putchr)(char), const char* text, ...)
{
	va_list args;
	va_start(args, text);
	printf(putchr, text, args);
	va_end(args);
}*/

void init_printf(void(*putchr)(char))
{
	s_putchr = putchr;
}

#ifdef TEST
}
#endif
