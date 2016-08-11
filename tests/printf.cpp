#include <gtest/gtest.h>
#include "../src/printf.cpp"

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
	format(&Buffer::putchr, "hello");
	ASSERT_EQ(5, pos);

	Buffer::clear();
	format(&Buffer::putchr, "%i", 12345);
	ASSERT_STREQ("12345", buffer);

	Buffer::clear();
	format(&Buffer::putchr, "%5i", 123);
	ASSERT_STREQ("00123", buffer);

	Buffer::clear();
	format(&Buffer::putchr, "%x", 258);
	ASSERT_STREQ("0x102", buffer);

	Buffer::clear();
	format(&Buffer::putchr, "%5x", 258);
	ASSERT_STREQ("0x00102", buffer);

	Buffer::clear();
	format(&Buffer::putchr, "pre:%i", 8);
	ASSERT_STREQ("pre:8", buffer);
}
