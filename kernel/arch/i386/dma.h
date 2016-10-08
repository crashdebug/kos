#ifndef DMA_H
#define DMA_H

namespace kos::x86
{
	class DMABlock
	{
		public:
			DMABlock( );
			~DMABlock( );
			void Load( void * pAddr, int iLen );
			union
			{
				unsigned char bytes[4];
				unsigned long value;
			} oAddress, oLen;
		/*
		char iPage;
		unsigned long iOffset;
		unsigned int iLength;
		*/
	};

	// Gets the segment of far pointer addr
	#define FP_SEGMENT(addr) ((unsigned int)((unsigned long)addr >> 16))

	// Gets the offset of far pointer addr
	#define FP_OFFSET(addr) ((unsigned int)addr)

	// Creates a far pointer from a segmented address
	#define MK_FP(seg, off) ((void*)(((unsigned long)seg << 16) | (unsigned)off)

	void StartDMA(unsigned char iDMAChannel, DMABlock* oBlock, unsigned char iMode);
	void LoadPageAndOffset(DMABlock* oBlock, unsigned char* aData);
	void StopDMA(unsigned char iDMAChannel);
	void FloppyDMAInit(bool bRead, unsigned char* aBuffer);
}

#endif
