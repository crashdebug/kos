# Declare constants used for creating a multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# Declare a multiboot header that marks the program as a kernel. These are magic 
# values that are documented in the multiboot standard. The bootloader will 
# search for this signature in the first 8 KiB of the kernel file, aligned at a 
# 32-bit boundary. The signature is in its own section so the header can be 
# forced to be within the first 8 KiB of the kernel file. .section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# The address of the multiboot info
bootinfo:
.long 0

# The multiboot standard does not define the value of the stack pointer register 
# (esp) and it is up to the kernel to provide a stack. This allocates room for a 
# small stack by creating a symbol at the bottom of it, then allocating 16384 
# bytes for it, and finally creating a symbol at the top. The stack grows 
# downwards on x86. The stack is in its own section so it can be marked nobits, 
# which means the kernel file is smaller because it does not contain an 
# uninitialized stack. The stack on x86 must be 16-byte aligned according to the 
# System V ABI standard and de-facto extensions. The compiler will assume the 
# stack is properly aligned and failure to align the stack will result in 
# undefined behavior.
# Reserve a stack for the initial thread.
.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

# The linker script specifies _start as the entry point to the kernel and the 
# bootloader will jump to this position once the kernel has been loaded. It 
# doesn't make sense to return from this function as the bootloader is gone. 
.section .text
.global _start
.type _start, @function
_start:
	movl %ebx, bootinfo

	# To set up a stack, we set the esp register to point to the top of our 
	# stack (as it grows downwards on x86 systems). This is necessarily done 
	# in assembly as languages such as C cannot function without a stack. 
  	movl $stack_top, %esp

	# The bootloader has loaded us into 32-bit protected mode on a x86 
	# machine. Interrupts are disabled. Paging is disabled. The processor 
	# state is as defined in the multiboot standard. The kernel has full 
	# control of the CPU. The kernel can only make use of hardware features 
	# and any code it provides as part of itself. There's no printf 
	# function, unless the kernel provides its own <stdio.h> header and a 
	# printf implementation. There are no security restrictions, no 
	# safeguards, no debugging mechanisms, only what the kernel provides 
	# itself. It has absolute and complete power over the 
	# machine.

	# This is a good place to initialize crucial processor state before the 
	# high-level kernel is entered. It's best to minimize the early 
	# environment where crucial features are offline. Note that the 
	# processor is not fully initialized yet: Features such as floating 
	# point instructions and instruction set extensions are not initialized 
	# yet. The GDT should be loaded here. Paging should be enabled here. 
	# C++ features such as global constructors and exceptions will require 
	# runtime support to work as well. 

	# Setup global descriptor table
	call install_gdt

	# Setup interrupt descriptor table
	call install_idt

	# Setup interrupt handlers
	call install_irq

	# Setup interrupt service routines
	call install_isr

	# Push magic number and the location of the multiboot info table
	# on to the stack. These will be passed to kernel_early() as arguments. 
	push $0xc0ffee
	pushl bootinfo

	# Initialize the core kernel before running the global constructors.
	call kernel_early

	# Call the global constructors.
	call _init

	# Enable interrupts
	sti

	# Enter the high-level kernel. The ABI requires the stack is 16-byte 
	# aligned at the time of the call instruction (which afterwards pushes 
	# the return pointer of size 4 bytes). The stack was originally 16-byte 
	# aligned above and we've since pushed a multiple of 16 bytes to the 
	# stack since (pushed 0 bytes so far) and the alignment is thus 
	# preserved and the call is well defined. 
	# Transfer control to the main kernel.
	call kernel_main

	# If the system has nothing more to do, put the computer into an 
	# infinite loop. To do that: 
	# 1) Disable interrupts with cli (clear interrupt enable in eflags). 
	#    They are already disabled by the bootloader, so this is not needed. 
	#    Mind that you might later enable interrupts and return from 
	#    kernel_main (which is sort of nonsensical to do). 
	# 2) Wait for the next interrupt to arrive with hlt (halt instruction). 
	#    Since they are disabled, this will lock up the computer. 
	# 3) Jump to the hlt instruction if it ever wakes up due to a 
	#    non-maskable interrupt occurring or due to system management mode. 
	cli
.Lhang:
	hlt
	jmp .Lhang

# This will set up our new segment registers. We need to do
# something special in order to set CS. We do what is called a
# far jump. A jump that includes a segment as well as an offset.

.extern _gdtptr				# Says that '_oGDTPtr' is in another file
.global _GDTFlush			# Allows the C code to link to this
.type _GDTFlush, @function

_GDTFlush:
	lgdt _gdtptr			# Load the GDT with our '_gp' which is a special pointer
	mov	$0x10, %ax			# 0x10 is the offset in the GDT to our data segment
	mov	%ax, %ds
	mov	%ax, %es
	mov	%ax, %fs
	mov	%ax, %gs
	mov	%ax, %ss
	ljmp $0x08, $Flush2		# 0x08 is the offset to our code segment: Far jump!
Flush2:
	ret						# Returns back to the C code!

# Loads the IDT defined in '_idtp' into the processor.
# This is declared in C as 'extern void idt_load();'
.extern oIDTPtr
.global _IDTLoad
.type _IDTLoad, @function
_IDTLoad:
	lidt _idtptr
	ret

#  0: Divide By Zero Exception
.global _isr0
.type _isr0, @function
_isr0:
	cli
	push	$0
	push	$0
	jmp	ISRCommonStub

#  1: Debug Exception
.global _isr1
.type _isr1, @function
_isr1:
	cli
	push	$0
	push	$1
	jmp	ISRCommonStub

#  2: Non Maskable Interrupt Exception
.global _isr2
.type _isr2, @function
_isr2:
	cli
	push	$0
	push	$2
	jmp	ISRCommonStub

#  3: Int 3 Exception
.global _isr3
.type _isr3, @function
_isr3:
	cli
	push	$0
	push	$3
	jmp	ISRCommonStub

# 4: INTO Exception
.global _isr4
.type _isr4, @function
_isr4:
	cli
	push	$0
	push	$4
	jmp	ISRCommonStub

#  5: Out of Bounds Exception
.global _isr5
.type _isr5, @function
_isr5:
	cli
	push	$0
	push	$5
	jmp	ISRCommonStub

#  6: Invalid Opcode Exception
.global _isr6
.type _isr6, @function
_isr6:
	cli
	push	$0
	push	$6
	jmp	ISRCommonStub

# 7: Coprocessor Not Available Exception
.global _isr7
.type _isr7, @function
_isr7:
	cli
	push	$0
	push	$7
	jmp	ISRCommonStub

# 8: Double Fault Exception (With Error Code!)
.global _isr8
.type _isr8, @function
_isr8:
	cli
	push	$8
	jmp	ISRCommonStub

#  9: Coprocessor Segment Overrun Exception
.global _isr9
.type _isr9, @function
_isr9:
	cli
	push	$0
	push	$9
	jmp	ISRCommonStub

# 10: Bad TSS Exception (With Error Code!)
.global _isr10
.type _isr10, @function
_isr10:
	cli
	push	$10
	jmp	ISRCommonStub

# 11: Segment Not Present Exception (With Error Code!)
.global _isr11
.type _isr11, @function
_isr11:
	cli
	push	$11
	jmp	ISRCommonStub

# 12: Stack Fault Exception (With Error Code!)
.global _isr12
.type _isr12, @function
_isr12:
	cli
	push	$12
	jmp	ISRCommonStub

# 13: General Protection Fault Exception (With Error Code!)
.global _isr13
.type _isr13, @function
_isr13:
	cli
	push	$13
	jmp	ISRCommonStub

# 14: Page Fault Exception (With Error Code!)
.global _isr14
.type _isr14, @function
_isr14:
	cli
	push	$14
	jmp	ISRCommonStub

# 15: Reserved Exception
.global _isr15
.type _isr15, @function
_isr15:
	cli
	push	$0
	push	$15
	jmp	ISRCommonStub

# 16: Floating Point Exception
.global _isr16
.type _isr16, @function
_isr16:
	cli
	push	$0
	push	$16
	jmp	ISRCommonStub

# 17: Alignment Check Exception
.global _isr17
.type _isr17, @function
_isr17:
	cli
	push	$0
	push	$17
	jmp	ISRCommonStub

# 18: Machine Check Exception
.global _isr18
.type _isr18, @function
_isr18:
	cli
	push	$0
	push	$18
	jmp	ISRCommonStub

# 19: Reserved
.global _isr19
.type _isr19, @function
_isr19:
	cli
	push	$0
	push	$19
	jmp	ISRCommonStub

# 20: Reserved
.global _isr20
.type _isr20, @function
_isr20:
	cli
	push	$0
	push	$20
	jmp	ISRCommonStub

# 21: Reserved
.global _isr21
.type _isr21, @function
_isr21:
	cli
	push	$0
	push	$21
	jmp	ISRCommonStub

# 22: Reserved
.global _isr22
.type _isr22, @function
_isr22:
	cli
	push	$0
	push	$22
	jmp	ISRCommonStub

# 23: Reserved
.global _isr23
.type _isr23, @function
_isr23:
	cli
	push	$0
	push	$23
	jmp	ISRCommonStub

# 24: Reserved
.global _isr24
.type _isr24, @function
_isr24:
	cli
	push	$0
	push	$24
	jmp	ISRCommonStub

# 25: Reserved
.global _isr25
.type _isr25, @function
_isr25:
	cli
	push	$0
	push	$25
	jmp	ISRCommonStub

# 26: Reserved
.global _isr26
.type _isr26, @function
_isr26:
	cli
	push	$0
	push	$26
	jmp	ISRCommonStub

# 27: Reserved
.global _isr27
.type _isr27, @function
_isr27:
	cli
	push	$0
	push	$27
	jmp	ISRCommonStub

# 28: Reserved
.global _isr28
.type _isr28, @function
_isr28:
	cli
	push	$0
	push	$28
	jmp	ISRCommonStub

# 29: Reserved
.global _isr29
.type _isr29, @function
_isr29:
	cli
	push	$0
	push	$29
	jmp	ISRCommonStub

# 30: Reserved
.global _isr30
.type _isr30, @function
_isr30:
	cli
	push	$0
	push	$30
	jmp	ISRCommonStub

# 31: Reserved
.global _isr31
.type _isr31, @function
_isr31:
	cli
	push	$0
	push	$31
	jmp	ISRCommonStub


# We call a C function in here. We need to let the assembler know
# that '_fault_handler' exists in another file
.extern _FaultHandler

# This is our common ISR stub. It saves the processor state, sets
# up for kernel mode segments, calls the C-level fault handler,
# and finally restores the stack frame.
ISRCommonStub:
	pusha
	push	%ds				# Store the situation
	push	%es
	push	%fs
	push	%gs

	mov	$0x10, %ax			# Setup segments
	mov	%ax, %ds
	mov	%ax, %es
	mov	%ax, %fs
	mov	%ax, %gs
	mov	%esp, %eax
	push	%eax

	mov	_FaultHandler, %eax 
	call	*%eax			# Special jump to eax

	pop	%eax				# Return everything as it was
	pop	%gs
	pop	%fs
	pop	%es
	pop	%ds
	popa
	add $8, %esp
	iret

# Set the size of the _start symbol to the current location '.' minus its start. 
# This is useful when debugging or when you implement call tracing. 
.size _start, . - _start
