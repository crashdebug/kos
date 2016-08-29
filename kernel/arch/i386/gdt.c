#include <gdt.h>

struct GDTEntry aGDT[3];
struct GDTPtr _gdtptr;

// In arch/.../boot.s
// Used to properly reload the new segment registers.
extern void _GDTFlush();

// Setup a descriptor in the Global Descriptor Table
void setGate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char granularity)
{
    // Setup the descriptor base address
    aGDT[num].iBaseLow = (base & 0xffff);
    aGDT[num].iBaseMiddle = (base >> 16) & 0xff;
    aGDT[num].iBaseHigh = (base >> 24) & 0xff;

    // Setup the descriptor limits
    aGDT[num].iLimitLow = (limit & 0xffff);
    aGDT[num].iGranularity = ((limit >> 16) & 0x0f);

    // Finally, set up the granularity and access flags
    aGDT[num].iGranularity |= (granularity & 0xf0);
    aGDT[num].iAccess = access;
}

// This will setup the GDT pointer, set up the first 3 entries in our GDT,
// and then finally call GDTFlush( ) in KernelStart.asm in order to tell
// the processor where the new GDT is and update the new segment registers.
void install_gdt()
{
    // Setup the GDT pointer and limit
    _gdtptr.iLimit = (sizeof(struct GDTEntry) * 3) - 1;
    _gdtptr.iBase = (unsigned int)aGDT;

    // Our NULL descriptor
    setGate(0, 0, 0, 0, 0);

    // The second entry is our Code Segment.
    // The base address is 0, the limit is 4GBytes, it uses 4KByte granularity,
    // uses 32-bit opcodes, and is a Code Segment descriptor.
    setGate(1, 0, 0xffffffff, 0x9a, 0xcf);

    // The third entry is our Data Segment.
    // It's EXACTLY the same as our code segment, but the descriptor type in
    // this entry's access byte says it's a Data Segment
    setGate(2, 0, 0xffffffff, 0x92, 0xcf);

    // Flush out the old GDT and install the new changes!
    _GDTFlush();
}
