#pragma once
#ifndef _REGISTERS
#define _REGISTERS 1

// This defines what the stack looks like after an ISR was running
struct Registers
{
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;
};

#endif
