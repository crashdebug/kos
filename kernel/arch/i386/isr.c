#include "isr.h"
#include "idt.h"
//#include "registers.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// This contains the messages that correspond to each and every exception.
// We get the correct message by accessing like: exceptionMessages[interrupt_number]
const char * exceptionMessages[] =
{
	"Division By Zero",
	"Debug",
	"Non Maskable Interrupt",
	"Breakpoint",
	"Into Detected Overflow",
	"Out Of Bounds",
	"Invalid Opcode",
	"No Coprocessor",
	"Double Fault",
	"Coprocessor Segment Overrun",
	"Bad TSS",
	"Segment Not Present",
	"Stack Fault",
	"General Protection Fault",
	"Page Fault",
	"Unknown Interrupt",
	"Coprocessor Fault",
};

// This function is called in all the software interrupts, and in exceptions without error code.
// It will call the corresponding function in the intfuncs array, as long as it's not NULL.
// If the called function returns a non-NULL pointer, that pointer will be used as a stack to switch the task.
void intfunc(uint32_t intnum, void *ctx)
{
	printf("Exception 0x%2lx: %s! System halted.", intnum, exceptionMessages[intnum]);
	while(1 == 1);
}

// This function is called in exceptions with error code.
// It will call the corresponding function in the intfuncs_err array, as long as it's not NULL.
// If the called function returns a non-NULL pointer, that pointer will be used as a stack to switch the task.
void intfunc_err(uint32_t intnum, void *ctx, uint32_t errcode)
{
	printf("Exception 0x%2lx: %s; %li! System halted.", intnum, exceptionMessages[intnum], errcode);
	while(1 == 1);
}

// Exceptions without error code
DEFINTWRAPPER(0) // division by 0
DEFINTWRAPPER(1) // debug
DEFINTWRAPPER(2) // NMI
DEFINTWRAPPER(3) // breakpoint
DEFINTWRAPPER(4) // INTO
DEFINTWRAPPER(5) // BOUND
DEFINTWRAPPER(6) // invalid opcode
DEFINTWRAPPER(7) // coprocessor not available
DEFINTWRAPPER(9) // coprocessor segment overrun
DEFINTWRAPPER(16) // coprocessor error
 
// Exceptions with error code
DEFINTWRAPPER_ERR(8) // double fault
DEFINTWRAPPER_ERR(10) // TSS error
DEFINTWRAPPER_ERR(11) // segment not present
DEFINTWRAPPER_ERR(12) // stack fault
DEFINTWRAPPER_ERR(13) // general protection fault
DEFINTWRAPPER_ERR(14) // page fault
 
// Software interrupts
DEFINTWRAPPER(0x80) // system call

// We set the first 32 entries in the IDT to the first 32 ISRs.
// We set the access flags to 0x8E. This means that the entry is present, is  running in ring 0 (kernel level),
// and has the lower 5 bits set to the required '14', which is represented by 'E' in hex.
void install_isr()
{
	set_idt_gate(0, (unsigned long)int0handler(), 0x08, 0x8E);
	set_idt_gate(1, (unsigned long)int1handler(), 0x08, 0x8E);
	set_idt_gate(2, (unsigned long)int2handler(), 0x08, 0x8E);
	set_idt_gate(3, (unsigned long)int3handler(), 0x08, 0x8E);
	set_idt_gate(4, (unsigned long)int4handler(), 0x08, 0x8E);
	set_idt_gate(5, (unsigned long)int5handler(), 0x08, 0x8E);
	set_idt_gate(6, (unsigned long)int6handler(), 0x08, 0x8E);
	set_idt_gate(7, (unsigned long)int7handler(), 0x08, 0x8E);
	set_idt_gate(8, (unsigned long)int8handler(), 0x08, 0x8E);
	set_idt_gate(9, (unsigned long)int9handler(), 0x08, 0x8E);
	set_idt_gate(10, (unsigned long)int10handler(), 0x08, 0x8E);
	set_idt_gate(11, (unsigned long)int11handler(), 0x08, 0x8E);
	set_idt_gate(12, (unsigned long)int12handler(), 0x08, 0x8E);
	set_idt_gate(13, (unsigned long)int13handler(), 0x08, 0x8E);
	set_idt_gate(14, (unsigned long)int14handler(), 0x08, 0x8E);
	set_idt_gate(16, (unsigned long)int16handler(), 0x08, 0x8E);

	set_idt_gate(0x80, (unsigned long)int0x80handler(), 0x08, 0x8E);
}

#ifdef __cplusplus
}
#endif
