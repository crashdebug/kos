#pragma once
#include "multiboot.h"

class MemoryManager
{
	public:
//		MemoryManager( );
//		~MemoryManager( );
		static void Initialize(multiboot_info_t* mbd);
		static void* MemAlloc(long unsigned int);
		static void MemFree(void*);
		static unsigned long long Available();
	private:
		static void Reserve(unsigned char * pBytemap, unsigned char iPages, unsigned char iOffset);
};
