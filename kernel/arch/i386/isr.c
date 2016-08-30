#include "idt.h"
#include "isr.h"
#include "registers.h"

/* These are function prototypes for all of the exception
*  handlers: The first 32 entries in the IDT are reserved
*  by Intel, and are designed to service exceptions! */
extern void _isr0();
extern void _isr1();
extern void _isr2();
extern void _isr3();
extern void _isr4();
extern void _isr5();
extern void _isr6();
extern void _isr7();
extern void _isr8();
extern void _isr9();
extern void _isr10();
extern void _isr11();
extern void _isr12();
extern void _isr13();
extern void _isr14();
extern void _isr15();
extern void _isr16();
extern void _isr17();
extern void _isr18();
extern void _isr19();
extern void _isr20();
extern void _isr21();
extern void _isr22();
extern void _isr23();
extern void _isr24();
extern void _isr25();
extern void _isr26();
extern void _isr27();
extern void _isr28();
extern void _isr29();
extern void _isr30();
extern void _isr31();

// We set the first 32 entries in the IDT to the first 32 ISRs.
// We set the access flags to 0x8E. This means that the entry is present, is  running in ring 0 (kernel level),
// and has the lower 5 bits set to the required '14', which is represented by 'E' in hex.
void install_isr()
{
    set_idt_gate(0, (unsigned)_isr0, 0x08, 0x8E);
    set_idt_gate(1, (unsigned)_isr1, 0x08, 0x8E);
    set_idt_gate(2, (unsigned)_isr2, 0x08, 0x8E);
    set_idt_gate(3, (unsigned)_isr3, 0x08, 0x8E);
    set_idt_gate(4, (unsigned)_isr4, 0x08, 0x8E);
    set_idt_gate(5, (unsigned)_isr5, 0x08, 0x8E);
    set_idt_gate(6, (unsigned)_isr6, 0x08, 0x8E);
    set_idt_gate(7, (unsigned)_isr7, 0x08, 0x8E);
    set_idt_gate(8, (unsigned)_isr8, 0x08, 0x8E);
    set_idt_gate(9, (unsigned)_isr9, 0x08, 0x8E);
    set_idt_gate(10, (unsigned)_isr10, 0x08, 0x8E);
    set_idt_gate(11, (unsigned)_isr11, 0x08, 0x8E);
    set_idt_gate(12, (unsigned)_isr12, 0x08, 0x8E);
    set_idt_gate(13, (unsigned)_isr13, 0x08, 0x8E);
    set_idt_gate(14, (unsigned)_isr14, 0x08, 0x8E);
    set_idt_gate(15, (unsigned)_isr15, 0x08, 0x8E);
    set_idt_gate(16, (unsigned)_isr16, 0x08, 0x8E);
    set_idt_gate(17, (unsigned)_isr17, 0x08, 0x8E);
    set_idt_gate(18, (unsigned)_isr18, 0x08, 0x8E);
    set_idt_gate(19, (unsigned)_isr19, 0x08, 0x8E);
    set_idt_gate(20, (unsigned)_isr20, 0x08, 0x8E);
    set_idt_gate(21, (unsigned)_isr21, 0x08, 0x8E);
    set_idt_gate(22, (unsigned)_isr22, 0x08, 0x8E);
    set_idt_gate(23, (unsigned)_isr23, 0x08, 0x8E);
    set_idt_gate(24, (unsigned)_isr24, 0x08, 0x8E);
    set_idt_gate(25, (unsigned)_isr25, 0x08, 0x8E);
    set_idt_gate(26, (unsigned)_isr26, 0x08, 0x8E);
    set_idt_gate(27, (unsigned)_isr27, 0x08, 0x8E);
    set_idt_gate(28, (unsigned)_isr28, 0x08, 0x8E);
    set_idt_gate(29, (unsigned)_isr29, 0x08, 0x8E);
    set_idt_gate(30, (unsigned)_isr30, 0x08, 0x8E);
    set_idt_gate(31, (unsigned)_isr31, 0x08, 0x8E);
}

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
	"Alignment Check",
	"Machine Check",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
};

// All of our Exception handling Interrupt Service Routines will point to this function.
// This will tell us what exception has happened!
// Right now, we simply halt the system by hitting an endless loop.
// All ISRs disable interrupts while they are being serviced as a 'locking' mechanism to prevent an IRQ from
// happening and messing up kernel data structures
void _FaultHandler(struct Registers *regs)
{
    // Is this a fault whose number is from 0 to 31?
    if (regs->int_no < 32)
    {
        // Display the description for the Exception that occurred.
        /*
        Console.Write( "\n[%i]: ", oRegs->int_no );
        Console.Write( aExceptionMessages[oRegs->int_no] );
        Console.WriteLine( " Exception!" );

        Console.WriteDebug( DEBUG_LEVEL_DEBUG, 0, "\teax: %8x  ebx: %8x  ecx: %8x  edx: %8x", oRegs->eax, oRegs->ebx, oRegs->ecx, oRegs->edx );
        Console.WriteDebug( DEBUG_LEVEL_DEBUG, 0, "\tebp: %8x  esp: %8x  edi: %8x  esi: %8x", oRegs->ebp, oRegs->esp, oRegs->edi, oRegs->esi );

        Console.SetFGColor( ConsoleRed );
        Console.Write( "System Halted!" );

        Halt( );*/
    }
}
