#include "idt.h"
#include "irq.h"
#include "util.h"

extern void _irq0();
extern void _irq1();
extern void _irq2();
extern void _irq3();
extern void _irq4();
extern void _irq5();
extern void _irq6();
extern void _irq7();
extern void _irq8();
extern void _irq9();
extern void _irq10();
extern void _irq11();
extern void _irq12();
extern void _irq13();
extern void _irq14();
extern void _irq15();

// This array is actually an array of function pointers.
// They're used to handle custom IRQ handlers for a given IRQ.
struct IRQCallback aIRQs[16];

// Enables interrupts
void enableInterrupts( )
{
    __asm__ __volatile__( "sti" );
}

// Enables interrupts
void disableInterrupts( )
{
    __asm__ __volatile__( "cli" );
}

// Normally, IRQs 0 to 7 are mapped to entries 8 to 15.
// This is a problem in protected mode, because IDT entry 8 is a Double Fault!
// Without remapping, every time IRQ0 fires, you get a Double Fault Exception,
// which is NOT actually what's happening. We send commands to the Programmable
// Interrupt Controller (PICs - also called the 8259's) in  order to make
// IRQ0 to 15 be remapped to IDT entries 32 to 47
void irq_remap( )
{
    outportb(0x20, 0x11);
    outportb(0xA0, 0x11);
    outportb(0x21, 0x20);
    outportb(0xA1, 0x28);
    outportb(0x21, 0x04);
    outportb(0xA1, 0x02);
    outportb(0x21, 0x01);
    outportb(0xA1, 0x01);
    outportb(0x21, 0x00);
    outportb(0xA1, 0x00);
}

void install_irq()
{
	irq_remap();

	set_idt_gate(32, (unsigned)_irq0, 0x08, 0x8E);
	set_idt_gate(33, (unsigned)_irq1, 0x08, 0x8E);
	set_idt_gate(34, (unsigned)_irq2, 0x08, 0x8E);
	set_idt_gate(35, (unsigned)_irq3, 0x08, 0x8E);
	set_idt_gate(36, (unsigned)_irq4, 0x08, 0x8E);
	set_idt_gate(37, (unsigned)_irq5, 0x08, 0x8E);
	set_idt_gate(38, (unsigned)_irq6, 0x08, 0x8E);
	set_idt_gate(39, (unsigned)_irq7, 0x08, 0x8E);
	set_idt_gate(40, (unsigned)_irq8, 0x08, 0x8E);
	set_idt_gate(41, (unsigned)_irq9, 0x08, 0x8E);
	set_idt_gate(42, (unsigned)_irq10, 0x08, 0x8E);
	set_idt_gate(43, (unsigned)_irq11, 0x08, 0x8E);
	set_idt_gate(44, (unsigned)_irq12, 0x08, 0x8E);
	set_idt_gate(45, (unsigned)_irq13, 0x08, 0x8E);
	set_idt_gate(46, (unsigned)_irq14, 0x08, 0x8E);
    set_idt_gate(47, (unsigned)_irq15, 0x08, 0x8E);
}

// Installs a custom IRQ handler for the given IRQ.
void installHandler(int irq, void (*handler)(struct Registers*))
{
	aIRQs[irq].handler = handler;
	aIRQs[irq].count = 0;
}

// Clears the handler for a given IRQ.
void uninstallHandler(int irq)
{
    aIRQs[irq].handler = 0;
}

void waitForIRQ(int irq)
{
	while ( aIRQs[irq].count == 0 ) ;
	aIRQs[irq].count--;
}

// Each of the IRQ ISRs point to this function, rather than the "FaultHandler" in Kernel.cpp.
// The IRQ Controllers need to be told when you are done servicing them, so you need
// to send them an "End of Interrupt" command (0x20).
// There are two 8259 chips: The first exists at 0x20, the second exists at 0xA0.
// If the second controller (an IRQ from 8 to 15) gets an interrupt, you need to acknowledge the
// interrupt at BOTH controllers, otherwise, you only send an EOI command to the first controller.
// If you don't send an EOI, system won't raise any more IRQs
void _IRQHandler(struct Registers *regs)
{
	// Find out if we have a custom handler to run for this IRQ...
	aIRQs[regs->int_no - 32].count++;

	if (aIRQs[regs->int_no - 32].handler)
	{
		// Call the function
		aIRQs[regs->int_no - 32].handler(regs);
	}

	// If the IDT entry that was invoked was greater than 40 (meaning IRQ8 - 15),
	// then we need to send an EOI to the slave controller.
	if (regs->int_no >= 40)
	{
		outportb(0xA0, 0x20);
	}

	// In either case, we need to send an EOI to the master interrupt controller too
	outportb(0x20, 0x20);
}
