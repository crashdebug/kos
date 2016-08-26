#ifndef _KOS_STDIO
#define _KOS_STDIO 1

/*****************************************************************************
	Minimal subfunction for printf. Outputs each character and formats
	any additional arguments.

	%[flag][width][.prec][mod][conv]
	flag:
		-	left justify, pad right w/ blanks		no
		0	pad left w/ 0 for numerics				no
		+	always print sign, + or -				no
		' '	(blank)									no
		#	(???)									no

	width:		(field width)						no

	prec:		(precision)							no

	conv:
		d,i	decimal int								DONE
		u	decimal unsigned						no
		o	octal									no
		x,X	hex										no
		f,e,g,E,G float								no
		c	char									no
		s	string									no
		p	ptr										no

	mod:
		N	near ptr								no
		F	far ptr									no
		h	short (16-bit) int						no
		l	long (32-bit) int						no
		L	long long (64-bit) int					no
*****************************************************************************/

#ifdef TEST
namespace kos {
#endif

int printf(const char* __restrict text, ...);
void init_printf(void(*putchr)(char));

#ifdef TEST
}
#endif

#endif
