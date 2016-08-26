#include <gtest/gtest.h>
#include <pagemap.h>
#include <multiboot.h>
#include <memory.h>

TEST(MemoryTests, MemoryAllocationTest)
{
	size_t len = 4096;
	unsigned char* buffer = new unsigned char[len];
	multiboot_memory_map_t mmap;
	mmap.addr = (multiboot_uint64_t)buffer;
	mmap.len = len;
	mmap.type = MULTIBOOT_MEMORY_AVAILABLE;
	mmap.size = 20;
	multiboot_info_t info;
	info.flags = MULTIBOOT_INFO_MEM_MAP;
	info.mmap_addr = (multiboot_uint32_t)&mmap;
	info.mmap_length = 20;

	MemoryManager::Initialize(&info);
}
