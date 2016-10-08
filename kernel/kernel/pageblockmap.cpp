#include <pagemap.h>
#include <string.h>
#include <new.h>

namespace kos
{
	struct PageBlock
	{
		unsigned char* pAddress;
		unsigned short iPages;
	};

	PageBlockmap::PageBlockmap(void* pAddress, __SIZE_TYPE__ iSize, __SIZE_TYPE__ iPageSize)
	{
		this->aPages = new (pAddress) PageBlock[iSize / iPageSize];
		this->pBaseAddress = ( (unsigned char *)pAddress + sizeof( PageBlock ) * ( iSize / iPageSize ) );
		this->pBaseAddress = ( this->pBaseAddress - ( (unsigned int)this->pBaseAddress % iPageSize ) + iPageSize );
		this->iTotalPages = ( iSize - ( this->pBaseAddress - (unsigned char *)pAddress ) ) / iPageSize;
		this->iPageSize = iPageSize;
		this->iAllocatedBlocks = 0;
		this->iAllocatedPages = 0;

		//Console.WriteDebug( DEBUG_LEVEL_MESSAGE, "PageBlockmap( )", "pPages: [%8x] pBase: [%8x] Total: %i", this->aPages, this->pBaseAddress, this->iTotalPages );
	}

	PageBlockmap::~PageBlockmap( )
	{
	}

	bool PageBlockmap::Allocate(__SIZE_TYPE__ iSize, void** pAddress)
	{
		// Anything left to allocate?
		if (this->iAllocatedBlocks >= this->iTotalPages)
		{
			return false;
		}

		unsigned char* pReturn = this->pBaseAddress;
		for (unsigned int i = 0; i < this->iTotalPages; i++)
		{
	//		Console.WriteDebug( DEBUG_LEVEL_DEBUG, "PageBlockmap::Allocate( )", "i %i pRet [%8x] pAddr [%8x]", i, pReturn, this->aPages[i].pAddress );
			// Is the address already allocated?
			if (i < this->iAllocatedBlocks && this->aPages[i].pAddress == pReturn)
			{
				// Calculate the next possible block
				pReturn = this->aPages[i].pAddress + this->aPages[i].iPages * this->iPageSize;
			}
			// No, is the unallocated block big enough?
			else
			{
				// Yes, reserve it
				if (i >= this->iAllocatedBlocks || (unsigned)(this->aPages[i].pAddress - pReturn) > iSize)
				{
					this->Reserve(i, (unsigned char *)pReturn, iSize);
					memset(pReturn, 0, iSize);
					*pAddress = pReturn;
	//				Console.WriteDebug( DEBUG_LEVEL_DEBUG, "PageBlockmap::Allocate( )", "new [%x] @ [%8x]", iSize, *pAddress );
					return true;
				}
			}
		}
		// Not enough continuous memory
		return false;
	}

	bool PageBlockmap::Free(void* pAddress)
	{
	//	Console.WriteDebug( DEBUG_LEVEL_DEBUG, "PageBlockmap::Free( )", "destruct [%8x]", pAddress );
		for (unsigned int i = 0; i < this->iAllocatedBlocks; i++)
		{
			if (this->aPages[i].pAddress != pAddress)
			{
				continue;
			}

			this->iAllocatedPages -= this->aPages[i].iPages;
			// Last allocated page?
			if (i < this->iAllocatedBlocks - 1)
			{
				// No, move rest of the allocation so no gaps remain
				memcpy(this->aPages + i * sizeof(PageBlock), this->aPages + (i + 1) * sizeof(PageBlock), (this->iAllocatedBlocks - i - 1) * sizeof(PageBlock));
			}
			else
			{
				// Yes, just reset
				this->aPages[i].pAddress = 0;
				this->aPages[i].iPages = 0;
			}
			this->iAllocatedBlocks--;
			return true;
		}
		return false;
	}

	__SIZE_TYPE__ PageBlockmap::GetMaxSize()
	{
		// TODO: Find out the size of the largest continuous block
		if (this->iAllocatedPages == 0)
		{
			return (this->iTotalPages * this->iPageSize);
		}
		else if (this->iAllocatedPages == this->iTotalPages)
		{
			return 0;
		}

		unsigned int iLargestBlock = 0;
		unsigned char * pCurr = this->pBaseAddress;
		for (unsigned int i = 0; i < this->iAllocatedBlocks; i++)
		{
			if (this->aPages[i].pAddress == pCurr)
			{
				pCurr += this->aPages[i].iPages * this->iPageSize;
			}
			else
			{
				if ((unsigned)(this->aPages[i].pAddress - pCurr) > iLargestBlock)
				{
					iLargestBlock = this->aPages[i].pAddress - pCurr;
				}
				pCurr = this->aPages[i].pAddress + this->aPages[i].iPages * this->iPageSize;
			}
		}
		if (this->iTotalPages * this->iPageSize - (pCurr - this->pBaseAddress) > iLargestBlock)
		{
			iLargestBlock = this->iTotalPages * this->iPageSize - (pCurr - this->pBaseAddress);
		}
	//	Console.WriteDebug( DEBUG_LEVEL_DEBUG, "PageBlockmap::GetMaxSize( )", "Max: %i", iLargestBlock );
		return iLargestBlock;

	}

	bool PageBlockmap::IsInScope(void* pAddress)
	{
		return (((unsigned char*)pAddress >= this->pBaseAddress) && ((unsigned char*)pAddress < this->pBaseAddress + this->iTotalPages * this->iPageSize));
	}

	void PageBlockmap::Reserve(unsigned int iPos, unsigned char* pAddress, __SIZE_TYPE__ iSize)
	{
		// How many pages do we need to allocate?
		unsigned short iPages = iSize / this->iPageSize;
		if (iSize % this->iPageSize != 0)
		{
			iPages++;
		}

		// Are we inserting a page?
		if (iPos < this->iAllocatedBlocks)
		{
	//		Console.WriteDebug( DEBUG_LEVEL_DEBUG, "PageBlockmap::Reserve( )", "Inserting %i pages [%8x] @ %i", iPages, pAddress, iPos );
			// Make room for the new page.
			memcpy(this->pBaseAddress + (iPos + 1) * sizeof(PageBlock), this->pBaseAddress + iPos * sizeof(PageBlock), (this->iAllocatedBlocks - iPos) * sizeof(PageBlock));
		}
		// No, we're appending
		else
		{
	//		Console.WriteDebug( DEBUG_LEVEL_DEBUG, "PageBlockmap::Reserve( )", "Appending %i pages [%8x] @ %i", iPages, pAddress, iPos );
		}
		this->aPages[iPos].pAddress = pAddress;
		this->aPages[iPos].iPages = iPages;
		this->iAllocatedBlocks++;
		this->iAllocatedPages += iPages;
	}
}
