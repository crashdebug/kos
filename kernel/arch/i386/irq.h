#pragma once
#ifndef _IRQ_H
#define _IRQ_H 1

#include "registers.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Macro to create hardware interrupt handling functions.
// It will call "irqfunc" with the register context and the IRQ number as parameters.
#define DEFIRQWRAPPER(irqnum)\
void *irq##irqnum##handler(void)\
{\
	volatile void *addr;\
	asm goto("jmp %l[endofISR]" ::: "memory" : endofISR);\
	asm volatile(".align 16" ::: "memory");\
	startofISR:\
	asm volatile("pushal\n\tpushl %%ebp\n\tmovl %%esp, %%ebp\n\tcld" ::: "memory");\
	asm volatile(\
		"pushl %%ds       \n\t"\
		"pushl %%es       \n\t"\
		"movw $16, %%cx   \n\t"\
		"movw %%cx, %%ds  \n\t"\
		"movw %%cx, %%es  \n\t"\
		"pushl %%ebp      \n\t"\
		"addl $4, (%%esp) \n\t"\
		"pushl %%ebx      \n\t"\
		"call *%%eax      \n\t"\
		"addl $8, %%esp       "\
		:: "a"(irqfunc), "b"((uint32_t) irqnum) : "memory");\
	asm volatile("popl %%es\n\tpopl %%ds\n\tleave\n\tpopal\n\tiret" ::: "memory");\
	endofISR:\
	asm goto(\
		".intel_syntax noprefix         \n\t"\
		"mov eax, offset %l[startofISR] \n\t"\
		"mov [ebx], eax                 \n\t"\
		".att_syntax                        "\
		:: "b"(&addr) : "eax", "memory" : startofISR);\
	return((void *) addr);\
}

void disable_interrupts();
void enable_interrupts();
void install_irq();
void install_irq_handler(int irq, void(*handler)(void*));
void uninstall_irq_handler(int irq);
void wait_for_irq(int irq);

//void irqfunc(uint32_t irq, void* ctx);

#ifdef __cplusplus
}
#endif

struct IRQCallback
{
	void (*handler)(void*);
	unsigned char count;
};

#endif
