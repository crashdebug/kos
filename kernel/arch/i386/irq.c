#include "idt.h"
#include "irq.h"
#include "util.h"
#include "pic.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// This array is actually an array of function pointers.
// They're used to handle custom IRQ handlers for a given IRQ.
struct IRQCallback s_irqs[16];

// Enables interrupts
void enable_interrupts( )
{
    __asm__ __volatile__( "sti" );
}

// Enables interrupts
void disable_interrupts( )
{
    __asm__ __volatile__( "cli" );
}

// Each of the IRQ ISRs point to this function, rather than the "FaultHandler" in Kernel.cpp.
// The IRQ Controllers need to be told when you are done servicing them, so you need
// to send them an "End of Interrupt" command (0x20).
// There are two 8259 chips: The first exists at 0x20, the second exists at 0xA0.
// If the second controller (an IRQ from 8 to 15) gets an interrupt, you need to acknowledge the
// interrupt at BOTH controllers, otherwise, you only send an EOI command to the first controller.
// If you don't send an EOI, system won't raise any more IRQs
void irqfunc(uint32_t irq, void* ctx)
{
	s_irqs[irq].count++;

	// Find out if we have a custom handler to run for this IRQ...
	if (s_irqs[irq].handler != 0)
	{
		// Call the function
		s_irqs[irq].handler(ctx);
	}

	send_pic_eoi((unsigned char)irq);
}

// Define low level IRQ handlers. These functions will call 
DEFIRQWRAPPER(0)
DEFIRQWRAPPER(1)
DEFIRQWRAPPER(2)
DEFIRQWRAPPER(3)
DEFIRQWRAPPER(4)
DEFIRQWRAPPER(5)
DEFIRQWRAPPER(6)
DEFIRQWRAPPER(7)
DEFIRQWRAPPER(8)
DEFIRQWRAPPER(9)
DEFIRQWRAPPER(10)
DEFIRQWRAPPER(11)
DEFIRQWRAPPER(12)
DEFIRQWRAPPER(13)
DEFIRQWRAPPER(14)
DEFIRQWRAPPER(15)

void install_irq()
{
    memset(&s_irqs, 0, sizeof(struct IRQCallback) * 16);

	// Map IRQs from 0...15 to 32...47
	pic_remap(0x20, 0x28);

	// Install IRQ handlers
	set_idt_gate(0x20, (unsigned long)irq0handler(), 0x08, 0x8E);
	set_idt_gate(0x21, (unsigned long)irq1handler(), 0x08, 0x8E);
	set_idt_gate(0x22, (unsigned long)irq2handler(), 0x08, 0x8E);
	set_idt_gate(0x23, (unsigned long)irq3handler(), 0x08, 0x8E);
	set_idt_gate(0x24, (unsigned long)irq4handler(), 0x08, 0x8E);
	set_idt_gate(0x25, (unsigned long)irq5handler(), 0x08, 0x8E);
	set_idt_gate(0x26, (unsigned long)irq6handler(), 0x08, 0x8E);
	set_idt_gate(0x27, (unsigned long)irq7handler(), 0x08, 0x8E);
	set_idt_gate(0x28, (unsigned long)irq8handler(), 0x08, 0x8E);
	set_idt_gate(0x29, (unsigned long)irq9handler(), 0x08, 0x8E);
	set_idt_gate(0x2a, (unsigned long)irq10handler(), 0x08, 0x8E);
	set_idt_gate(0x2b, (unsigned long)irq11handler(), 0x08, 0x8E);
	set_idt_gate(0x2c, (unsigned long)irq12handler(), 0x08, 0x8E);
	set_idt_gate(0x2d, (unsigned long)irq13handler(), 0x08, 0x8E);
	set_idt_gate(0x2e, (unsigned long)irq14handler(), 0x08, 0x8E);
    set_idt_gate(0x2f, (unsigned long)irq15handler(), 0x08, 0x8E);
}

// Installs a custom IRQ handler for the given IRQ.
void install_irq_handler(int irq, void (*handler)(void*))
{
	s_irqs[irq].handler = handler;
	s_irqs[irq].count = 0;
}

// Clears the handler for a given IRQ.
void uninstall_irq_handler(int irq)
{
    s_irqs[irq].handler = 0;
}

void wait_for_irq(int irq)
{
	if (s_irqs[irq].handler == 0) return;

	while (s_irqs[irq].count == 0) ;
	s_irqs[irq].count--;
}

#ifdef __cplusplus
}
#endif
