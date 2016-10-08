#include "dma.h"
#include "irq.h"
#include "util.h"
#include <stdio.h>

// In boot.s
extern "C" unsigned int DMAComplete(unsigned char iDMAChannel);

namespace kos::x86
{
    unsigned char MaskReg[8]   = { 0x0A, 0x0A, 0x0A, 0x0A, 0xD4, 0xD4, 0xD4, 0xD4 };
    unsigned char ModeReg[8]   = { 0x0B, 0x0B, 0x0B, 0x0B, 0xD6, 0xD6, 0xD6, 0xD6 };
    unsigned char ClearReg[8]  = { 0x0C, 0x0C, 0x0C, 0x0C, 0xD8, 0xD8, 0xD8, 0xD8 };

    unsigned char PagePort[8]  = { 0x87, 0x83, 0x81, 0x82, 0x8F, 0x8B, 0x89, 0x8A };
    unsigned char AddrPort[8]  = { 0x00, 0x02, 0x04, 0x06, 0xC0, 0xC4, 0xC8, 0xCC };
    unsigned char CountPort[8] = { 0x01, 0x03, 0x05, 0x07, 0xC2, 0xC6, 0xCA, 0xCE };

    DMABlock::DMABlock( )
    {
    }

    DMABlock::~DMABlock( )
    {
    }

    void DMABlock::Load(void* pAddr, int iLen)
    {
        this->oAddress.value = (long unsigned int)pAddr;
        this->oLen.value = iLen - 1;

        // Check that address is at most 24-bits (under 16MB)
        // Check that count is at most 16-bits (DMA limit)
        // Check that if we add count and address we don't get a carry
        // (DMA can't deal with such a carry, this is the 64k boundary limit)
        if ((this->oAddress.value >> 24) || (this->oLen.value >> 16) || (((this->oAddress.value & 0xffff) + this->oLen.value) >> 16))
        {
            printf("FloppyDMAInit: static buffer [%8x] problem!", pAddr);
            return;
        }
    }

    void StartDMA(unsigned char iDMAChannel, DMABlock *oBlock, unsigned char iMode)
    {
        // Don't let anyone else mess up what we're doing
        disable_interrupts();

        // First, make sure our 'mode' is using the DMA channel specified.
        iMode |= iDMAChannel;

        // Set up the DMA channel so we can use it.
        // This tells the DMA that we're going to be using this channel. (It's masked)
        outb(MaskReg[iDMAChannel], 0x04 | iDMAChannel);
        outb(ClearReg[iDMAChannel], 0xff);

        // Send the offset address.
        // The first byte is the low base offset, the second byte is the high offset.
        outb(AddrPort[iDMAChannel], oBlock->oAddress.bytes[0]);
        outb(AddrPort[iDMAChannel], oBlock->oAddress.bytes[1]);

        // Send the physical page that the data lies on
        outb(PagePort[iDMAChannel], oBlock->oAddress.bytes[2]);

        // Reset flip-flop
        outb(ClearReg[iDMAChannel], 0xff);

        // Send the length of the data.  Again, low byte first
        outb(CountPort[iDMAChannel], oBlock->oLen.bytes[0]);
        outb(CountPort[iDMAChannel], oBlock->oLen.bytes[1]);

        // Send the specified mode to the DMA.
        outb(ModeReg[iDMAChannel], iMode);

        // Ok, we're done.  Enable the DMA channel (clear the mask).
        outb(MaskReg[iDMAChannel], iDMAChannel);

        // Re-enable interrupts before we leave.
        enable_interrupts( );
    }

    void PauseDMA(unsigned char iDMAChannel)
    {
        // All we have to do is mask the DMA channel's bit on.
        outb(MaskReg[iDMAChannel], 0x04 | iDMAChannel);
    }

    void ResumeDMA(unsigned char iDMAChannel)
    {
        // Simply clear the mask, and the DMA continues where it left off
        outb(MaskReg[iDMAChannel], iDMAChannel);
    }

    void StopDMA(unsigned char iDMAChannel)
    {
        // We need to set the mask bit for this channel, and then clear the selected channel.
        // Then we can clear the mask.
        PauseDMA(iDMAChannel);

        // Send the clear command.
        outb(ClearReg[iDMAChannel], 0x00);

        // And clear the mask.
        ResumeDMA(iDMAChannel);
    }
}

/*
unsigned int DMAComplete( unsigned char iDMAChannel )
{
    // Register variables are compiled to use registers in C, not memory.
    register int z;

    z = CountPort[iDMAChannel];
    _outportb( 0x0C, 0xFF );

    return _AX;
}
*/
