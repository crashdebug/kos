#pragma once

class IPageMap
{
	public:
		virtual bool Allocate(__SIZE_TYPE__ size, void** pAddress) = 0;
		virtual bool Free(void* pAddress ) = 0;
		virtual __SIZE_TYPE__ GetMaxSize() = 0;
		virtual bool IsInScope(void* pAddress) = 0;
};

class PageBitmap : public IPageMap
{
	private:
		// Pointer to the start of the pages in memory
		unsigned char* pBasePtr;
		// Pointer to the bitmap of pages in memory
		unsigned char* pPagePtr;
		// Total number of pages available
		unsigned int iTotalPages;
		// Size of each page in bytes
		__SIZE_TYPE__ iPageSize;
		void Reserve(unsigned char* pBytemap, unsigned char iPages, unsigned char iOffset);
	public:
		PageBitmap(void* pAddress, __SIZE_TYPE__ iSize, __SIZE_TYPE__ iPageSize);
		~PageBitmap();
		bool Allocate(__SIZE_TYPE__ iSize, void** pAddress);
		bool Free(void* pAddress);
		__SIZE_TYPE__ GetMaxSize();
		bool IsInScope(void* pAddress);
};

class PageBlockmap : public IPageMap
{
	private:
		__SIZE_TYPE__ iPageSize;
		unsigned int iTotalPages;
		unsigned int iAllocatedBlocks;
		unsigned int iAllocatedPages;
		struct PageBlock* aPages;
		unsigned char* pBaseAddress;
		void Reserve(unsigned int iPos, unsigned char* pAddress, __SIZE_TYPE__ iSize);
	public:
		PageBlockmap(void* pAddress, __SIZE_TYPE__ iSize, __SIZE_TYPE__ iPageSize);
		~PageBlockmap();
		bool Allocate(__SIZE_TYPE__ iSize, void** pAddress);
		bool Free(void* pAddress);
		__SIZE_TYPE__ GetMaxSize();
		bool IsInScope(void* pAddress);
};
