#include <gtest/gtest.h>
#include <pagemap.h>
#include <multiboot.h>
#include <memory.h>

TEST(MemoryTests, PageBitmapTest)
{
	size_t len = 4096;
	size_t pagelen = 256;
	unsigned char* buffer = new unsigned char[len];
	kos::PageBitmap pb(buffer, len, pagelen);

	int* ptr = 0;
	ASSERT_TRUE(pb.Allocate(4, (void**)&ptr));
	ASSERT_NE(0, ptr);

	ASSERT_TRUE(pb.Free(ptr));
}

TEST(MemoryTests, PageBlockmapTest)
{
	size_t len = 4096;
	size_t pagelen = 1024;
	unsigned char* buffer = new unsigned char[len];
	kos::PageBlockmap pb(buffer, len, pagelen);

	int* ptr = 0;
	ASSERT_TRUE(pb.Allocate(512, (void**)&ptr));
	ASSERT_NE(0, ptr);

	ASSERT_TRUE(pb.Free(ptr));
}
