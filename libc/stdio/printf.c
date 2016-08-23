#include <stdio.h>

#define FORMAT_BUFLEN 256
char formatBuffer[FORMAT_BUFLEN];

void(*s_putchr)(char) = 0;

int itos(long l, unsigned char radix, char* buffer, int len)
{
	char* offset = buffer;
	// Insert the zero terminator at the end of the string
	offset[0] = '\0';
	unsigned long num = l;
	int chars = 0;
	do
	{
		unsigned long temp = (unsigned long)num % radix;
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
		num = (unsigned long)num / radix;
		chars++;
	}
	// Loop while some number is left
	while (num != 0 && buffer - offset < len);
	return chars;
}

/*****************************************************************************
	Formats and outputs the specified number in the specified radix.
*****************************************************************************/
void formatNumber(long num, unsigned char radix, int width, char pad, void (*putchr)(char))
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
		putchr(*offset);
	}
}

#ifdef TEST
namespace kos {
#endif

//void printf(void (*putchr)(char), const char* text, va_list args)
int printf(const char* __restrict text, ...)
{
	va_list args;
	va_start(args, text);

	unsigned char state = 0;
	unsigned int width = -1;

	// Loop thru each character in sText
	for (char *p = (char*)text; *p; p++)
	{
		switch (state)
		{
			// STATE 0 = AWAITING
			case 0:
				// Not a '%' -> output normally
				if (*p != '%')
				{
					s_putchr(*p);
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
					s_putchr(*p);
				}
				// d = digits
				else if (*p == 'i')
				{
					formatNumber(va_arg(args, long), 10, width, '0', s_putchr);
				}
				// x = hex
				else if (*p == 'x')
				{
					formatNumber(va_arg(args, long), 16, width, '0', s_putchr);
				}
				// p = pointer
				else if (*p == 'p')
				{
					formatNumber(va_arg(args, long), 16, width, '0', s_putchr);
				}
				// s = string
				else if (*p == 's')
				{
					char* s = va_arg(args, char*);
					for (unsigned int i = 0; s[i] != 0; i++)
					{
						s_putchr(s[i]);
					}
				}
				else
				{
					s_putchr(*p);
				}
				// Reset
				state = 0;
				width = 0;
				break;
		}
	}
	va_end(args);
	return 0;
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
