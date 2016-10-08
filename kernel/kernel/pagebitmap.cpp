#include <pagemap.h>
#include <string.h>

namespace kos
{
	PageBitmap::PageBitmap(void* pAddress, __SIZE_TYPE__ iSize, __SIZE_TYPE__ iPageSize)
	{
		this->iTotalPages = iSize / iPageSize;
		this->iPageSize = iPageSize;
		this->pBasePtr = this->pPagePtr = (unsigned char*)pAddress;
		// Reserve space for the bitmap (8 pages / byte)
		this->pBasePtr += (this->iTotalPages / 8);
		// Align at 8 bytes
		this->pBasePtr += (iPageSize - (unsigned int)this->pBasePtr % iPageSize);

		//Console.WriteDebug( DEBUG_LEVEL_TRACE, "PageBitmap( )", "Base @ %8x. Total %i page(s) %i byte(s) each", this->pPagePtr, this->iTotalPages, this->iPageSize );
	}

	PageBitmap::~PageBitmap()
	{
	}

	void PageBitmap::Reserve(unsigned char* pBytemap, unsigned char iPages, unsigned char iOffset)
	{
		unsigned char iReserve = ( 1 << iPages ) - 1;
		iReserve = iReserve << ( 7 - iOffset );
		//Console.WriteLine( "Reserving %2x @ %8x", iReserve, pBytemap );
		*pBytemap = *pBytemap | iReserve;
	}

	bool PageBitmap::Allocate(__SIZE_TYPE__ iSize, void** pAddress)
	{
		// Currently cannot allocate larger blocks than the page size
		if (iSize > this->iPageSize)
		{
			//Console.WriteDebug( DEBUG_LEVEL_CRITICAL, "PageBitmap::Allocate( )", "Block too large! [%i]", iSize );
			return false;
		}

		// Position in the page record
		unsigned int iPos = 0;
		// Loop while we have records left
		while (iPos < this->iTotalPages / 8)
		{
			unsigned char iBytemap = this->pPagePtr[iPos++];
			// Everything already allocated?
			if (iBytemap == 0xff)
			{
				continue;
			}

			// Find out where the next free page is
			unsigned char iOffset = 0;
			for ( ; iOffset < 8; iOffset++)
			{
				if (((0x80 >> iOffset) & iBytemap) == 0)
				{
					break;
				}
			}

			// Calculate the actual address of the page
			*pAddress = this->pBasePtr + (iPos - 1) * 8 * this->iPageSize + iOffset * this->iPageSize;
			// Reserve it
			this->Reserve(this->pPagePtr + iPos - 1, 1, iOffset);
			//break;
			memset(*pAddress, 0, iSize);
	//		Console.WriteDebug( DEBUG_LEVEL_DEBUG, "PageBitmap::Allocate( )", "new [%x] @ [%8x]", iSize, *pAddress );
			return true;
		}
		return false;
	}

	bool PageBitmap::Free(void* pAddress)
	{
	//	Console.WriteDebug( DEBUG_LEVEL_DEBUG, "PageBitmap::Free( )", "destruct [%8x]", pAddress );
		unsigned int iOffset = ((unsigned char*)pAddress - pBasePtr) / this->iPageSize;
		pPagePtr[iOffset / 8] &= ~(1 << (7 - (iOffset % 8)));
		return true;
	}

	__SIZE_TYPE__ PageBitmap::GetMaxSize()
	{
		return this->iPageSize;
	}

	bool PageBitmap::IsInScope(void* pAddress)
	{
		return (((unsigned char*)pAddress >= (unsigned char*)this->pBasePtr) && ((unsigned char*)pAddress < (unsigned char*)this->pBasePtr + this->iTotalPages * this->iPageSize));
	}
}
