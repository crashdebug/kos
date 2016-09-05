#pragma once
#ifndef _GDT_H
#define _GDT_H 1

#ifdef __cplusplus
extern "C" {
#endif

// Defines a GDT entry.
struct GDTEntry
{
	unsigned short iLimitLow;
	unsigned short iBaseLow;
	unsigned char iBaseMiddle;
	unsigned char iAccess;
	unsigned char iGranularity;
	unsigned char iBaseHigh;
} __attribute__((packed));
// This NEEDS to be packed

// Special pointer which includes the limit:
// The max bytes taken up by the GDT, minus 1.
struct GDTPtr
{
    unsigned short iLimit;
    unsigned int iBase;
} __attribute__( ( packed ) );
// This NEEDS to be packed

void install_gdt();
void set_gdt_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char granularity);

#ifdef __cplusplus
}
#endif

#endif
