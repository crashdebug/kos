#include <memory.h>
#include <new.h>
#include <pagemap.h>
#include <stdio.h>

// Size of each page in bytes
//unsigned short iPageSize = 0x1000;
// Total number of pages available
//unsigned int iTotalPages;

IPageMap** g_pageMaps = 0;
unsigned long long g_memsize = 0;

void MemoryManager::Initialize(multiboot_info_t* mbd)
{
	// Find the largest page
	unsigned char* largestPage = 0;
	multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)mbd->mmap_addr;
	while (mmap < (multiboot_memory_map_t*)(mbd->mmap_addr + mbd->mmap_length))
	{
		if ((mmap->type & MULTIBOOT_MEMORY_AVAILABLE) == MULTIBOOT_MEMORY_AVAILABLE && (largestPage == 0 || mmap->len > g_memsize))
		{
			largestPage = (unsigned char*)mmap->addr;
			g_memsize = mmap->len;
		}
		mmap = (multiboot_memory_map_t*)((unsigned int)mmap + mmap->size + sizeof(unsigned int));
	}

	// Allocate memory for two heaps
	g_pageMaps = (IPageMap**)largestPage;
	largestPage += sizeof(void*) * 3;

	// Small object heap ( <= 256 bytes )
	g_pageMaps[0] = new (largestPage) PageBitmap((unsigned char*)g_pageMaps + 0x1000, (g_memsize - 0x1000) / 2, 256);
	largestPage += sizeof(PageBitmap*);

	// Large object heap ( > 256 bytes )
	g_pageMaps[1] = new (largestPage) PageBlockmap((unsigned char*)g_pageMaps + 0x1000 + (g_memsize - 0x1000) / 2, (g_memsize - 0x1000) / 2, 1024);
	largestPage += sizeof(PageBlockmap*);

	// Terminate list
	g_pageMaps[2] = 0;
}

unsigned long long MemoryManager::Available()
{
	return g_memsize;
}

void* MemoryManager::MemAlloc(long unsigned int size)
{
	if (g_pageMaps == 0)
	{
		return 0;
	}

	unsigned char i = 0;
	void* p;
	while (g_pageMaps[i] != 0)
	{
		// TODO: If allocation fails, try the next heap, or resize heaps.
		// Find a pagemap big enough to hold the requested size
		if (size <= g_pageMaps[i]->GetMaxSize() && g_pageMaps[i]->Allocate(size, &p))
		{
			return p;
		}
		i++;
	}
	return 0;
}

void MemoryManager::MemFree(void* p)
{
	if (g_pageMaps == 0)
	{
		return;
	}

	unsigned char i = 0;
	while (g_pageMaps[i] != 0)
	{
		if (g_pageMaps[i]->IsInScope(p) && g_pageMaps[i]->Free(p))
		{
			break;
		}
		i++;
	}
}
