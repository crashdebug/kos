#include <memory.h>
#include <new.h>
#include <pagemap.h>
#include <stdio.h>

// Size of each page in bytes
//unsigned short iPageSize = 0x1000;
// Total number of pages available
//unsigned int iTotalPages;

IPageMap** aPageMaps = 0;

void MemoryManager::Initialize(multiboot_info_t* mbd)
{
	// Find the largest page
	unsigned char* largestPage = 0;
	unsigned int largestSize = 0;
	multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)mbd->mmap_addr;
	while (mmap < (multiboot_memory_map_t*)(mbd->mmap_addr + mbd->mmap_length))
	{
		if ((mmap->type & MULTIBOOT_MEMORY_AVAILABLE) == MULTIBOOT_MEMORY_AVAILABLE && (largestPage == 0 || mmap->len > largestSize))
		{
			largestPage = (unsigned char*)mmap->addr;
			largestSize = mmap->len;
		}
		mmap = (multiboot_memory_map_t*)((unsigned int)mmap + mmap->size + sizeof(unsigned int));
	}

	// Allocate memory for two heaps
	aPageMaps = (IPageMap**)largestPage;
	largestPage += 24;

	// Small object heap ( <= 256 bytes )
	aPageMaps[0] = new (largestPage) PageBitmap((unsigned char*)aPageMaps + 0x1000, (largestSize - 0x1000) / 2, 256);
	largestPage += sizeof(PageBitmap*);

	// Large object heap ( > 256 bytes )
	aPageMaps[1] = new (largestPage) PageBlockmap((unsigned char*)aPageMaps + 0x1000 + (largestSize - 0x1000) / 2, (largestSize - 0x1000) / 2, 1024);
	largestPage += sizeof(PageBlockmap*);

	printf("MemoryManager initialized (%i bytes available)\n", largestSize);
}

void* MemoryManager::MemAlloc(long unsigned int iSize)
{
	if (aPageMaps == 0)
	{
		return 0;
	}

	unsigned char i = 0;
	void* pAddress;
	while (aPageMaps[i] != 0)
	{
		// TODO: If allocation fails, try the next heap
		// Find a pagemap big enough to hold the requested size
		if (iSize <= aPageMaps[i]->GetMaxSize() && aPageMaps[i]->Allocate(iSize, &pAddress))
		{
			return pAddress;
		}
		i++;
	}
	return 0;
}

void MemoryManager::MemFree(void* p)
{
	if (aPageMaps == 0)
	{
		return;
	}

	unsigned char i = 0;
	while (aPageMaps[i] != 0)
	{
		if (aPageMaps[i]->IsInScope(p) && aPageMaps[i]->Free(p))
		{
			break;
		}
		i++;
	}
}
