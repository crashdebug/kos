#include "pic.h"
#include "util.h"

#define PIC_EOI		0x20		/* End-of-interrupt command code */

void send_pic_eoi(unsigned char irq)
{
	if (irq >= 8)
	{
		outb(PIC2_COMMAND,PIC_EOI);
	}
	outb(PIC1_COMMAND,PIC_EOI);
}

// Normally, IRQs 0 to 7 are mapped to entries 8 to 15.
// This is a problem in protected mode, because IDT entry 8 is a Double Fault!
// Without remapping, every time IRQ0 fires, you get a Double Fault Exception,
// which is NOT actually what's happening. We send commands to the Programmable
// Interrupt Controller (PICs - also called the 8259's) in  order to make
// IRQ0 to 15 be remapped to IDT entries 32 to 47

#define ICW1_ICW4		0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE		0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL		0x08		/* Level triggered (edge) mode */
#define ICW1_INIT		0x10		/* Initialization - required! */
 
#define ICW4_8086		0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO		0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM		0x10		/* Special fully nested (not) */

// Reinitialize the PIC controllers, giving them specified vector offsets
// rather than 8h and 70h, as configured by default */
void pic_remap(int master_offset, int slave_offset)
{
	unsigned char a1, a2;
 
	// save masks
	a1 = inb(PIC1_DATA);						
	a2 = inb(PIC2_DATA);
 
	// starts the initialization sequence (in cascade mode)
	outb(PIC1_COMMAND, ICW1_INIT + ICW1_ICW4);	
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT + ICW1_ICW4);
	io_wait();
	// ICW2: Master PIC vector offset
	outb(PIC1_DATA, master_offset);
	io_wait();
	// ICW2: Slave PIC vector offset
	outb(PIC2_DATA, slave_offset);
	io_wait();
	// ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	outb(PIC1_DATA, 4);
	io_wait();
	// ICW3: tell Slave PIC its cascade identity (0000 0010)
	outb(PIC2_DATA, 2);							
	io_wait();
 
	outb(PIC1_DATA, ICW4_8086);
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();
 
	// restore saved masks.
	outb(PIC1_DATA, a1);						
	outb(PIC2_DATA, a2);
}
