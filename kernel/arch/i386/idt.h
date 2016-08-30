#pragma once
#ifndef _IDT_H
#define _IDT_H 1

// Defines an IDT entry
struct IDTEntry
{
    unsigned short baseLow;
    unsigned short selector;	// Our kernel segment goes here!
    unsigned char always0;		// This will ALWAYS be set to 0!
    unsigned char flags;		// Set using the above table!
    unsigned short baseHigh;
} __attribute__((packed));
// This NEEDS to be packed

struct IDTPtr
{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));
// This NEEDS to be packed

void install_idt();
void set_idt_gate(unsigned char num, unsigned long base, unsigned short selector, unsigned char flags);

#endif
