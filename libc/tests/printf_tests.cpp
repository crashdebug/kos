#include <gtest/gtest.h>
#include "../include/stdio.h"
#include <stdint.h>

TEST(PrintFTests, FormatNumberTest)
{
	static char buffer[256];
	static unsigned int pos = 0;

	class Buffer 
	{
public:
		static void clear()
		{
			while (--pos > 0)
			{
				buffer[pos] = 0;
			}
		}
		static void putchr(char c)
		{
			buffer[pos++] = c;
		}
	};

	kos::init_printf(&Buffer::putchr);

	kos::printf("hello");
	ASSERT_EQ(5, pos);

	Buffer::clear();
	kos::printf("%i", 12345);
	ASSERT_STREQ("12345", buffer);

	Buffer::clear();
	kos::printf("%5i", 123);
	ASSERT_STREQ("00123", buffer);

	Buffer::clear();
	kos::printf("%x", 258);
	ASSERT_STREQ("102", buffer);

	Buffer::clear();
	kos::printf("%5x", 258);
	ASSERT_STREQ("00102", buffer);

	Buffer::clear();
	kos::printf("pre:%i", 8);
	ASSERT_STREQ("pre:8", buffer);

	/*struct st {
		unsigned int size;
		unsigned long long addr;
		unsigned long long len;
		unsigned int type;
	};
	st* s = new st{ 20, 0xffffffffffff, 654336, 1 };

	auto f = [](const char* text, ...) {
		va_list args;
		va_start(args, text);
		format(&Buffer::putchr, text, args);
		va_end(args);
	};
	Buffer::clear();
	f("[%i] 0x%8x : %i/%i\n", s->type, s->addr, s->size, s->len);
	ASSERT_STREQ("[1] 0xffffffffffff : 20/654336\n", buffer);*/
}
