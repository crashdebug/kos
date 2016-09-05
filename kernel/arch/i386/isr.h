#pragma once
#ifndef _ISR_H
#define _ISR_H 1

#include <stdint.h>

// Macro to create sotfware interrupt handling functions.
// It will call "intfunc" with the register context and the interrupt number as parameters.
#define DEFINTWRAPPER(intnum)\
void *int##intnum##handler(void)\
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
		:: "a"(intfunc), "b"((uint32_t) intnum) : "memory");\
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

// Macro to create exception handling functions, for exceptions with error code.
// It will call intfunc_err, with the error code, the register context, and the interrupt number as parameters.
#define DEFINTWRAPPER_ERR(intnum)\
void *int##intnum##handler(void)\
{\
	volatile void *addr;\
	asm goto("jmp %l[endofISR]" ::: "memory" : endofISR);\
	asm volatile(".align 16" ::: "memory");\
	startofISR:\
	asm volatile(\
		"pushal                \n\t"\
		"pushl %%ebp           \n\t"\
		"movl %%esp, %%ebp     \n\t"\
		"pushl %%ds            \n\t"\
		"pushl %%es            \n\t"\
		"movw $16, %%cx        \n\t"\
		"movw %%cx, %%ds       \n\t"\
		"movw %%cx, %%es       \n\t"\
		"movl 36(%%ebp), %%edx \n\t"\
		"movl %%ebp, %%esi     \n\t"\
		"addl $32, %%esi       \n\t"\
		"movl %%esi, %%edi     \n\t"\
		"addl $4, %%edi        \n\t"\
		"movl $11, %%ecx       \n\t"\
		"std                   \n\t"\
		"rep movsl             \n\t"\
		"add $4, %%esp         \n\t"\
		"cld                       "\
		::: "memory");\
	asm volatile(\
		"pushl %%edx       \n\t"\
		"pushl %%ebp       \n\t"\
		"addl $8, (%%esp)  \n\t"\
		"pushl %%ebx       \n\t"\
		"call *%%eax       \n\t"\
		"addl $12, %%esp       "\
		:: "a"(intfunc_err), "b"((uint32_t) intnum) : "memory");\
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

#ifdef __cplusplus
extern "C" {
#endif

void install_isr();

#ifdef __cplusplus
}
#endif

#endif
