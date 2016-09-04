#include "idt.h"
#include <string.h>

// Declare an IDT of 256 entries. Although we will only use the first 32 entries in this tutorial,
// the rest exists as a bit of a trap. If any undefined IDT entry is hit, it normally will cause an "Unhandled Interrupt" exception.
// Any descriptor for which the 'presence' bit is cleared (0) will generate "Unhandled Interrupt" exception
struct IDTEntry _idt[256];
struct IDTPtr _idtptr;

// This exists in KernelStart.asm, and is used to load our IDT
extern void _IDTLoad();

// Use this function to set an entry in the IDT. Alot simpler than twiddling with the GDT
void set_idt_gate(unsigned char num, unsigned long base, unsigned short selector, unsigned char flags)
{
    // The interrupt routine's base address
    _idt[num].baseLow = (base & 0xffff);
    _idt[num].baseHigh = (base >> 16) & 0xffff;

    // The segment or 'selector' that this IDT entry will use,
    // along with any access flags
    _idt[num].selector = selector;
    _idt[num].always0 = 0;
    _idt[num].flags = flags;
}

// Installs the IDT
void install_idt()
{
    // Sets the special IDT pointer up, just like in 'gdt.c'
    _idtptr.limit = (sizeof(struct IDTEntry) * 256) - 1;
    _idtptr.base = (unsigned int)_idt;

    // Clear out the entire IDT, initializing it to zeros
    memset(&_idt, 0, sizeof(struct IDTEntry) * 256);

    // Add any new ISRs to the IDT here using set_idt_gate()

    // Points the processor's internal register to the new IDT
    _IDTLoad();
}
