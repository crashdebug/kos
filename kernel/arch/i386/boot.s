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

	# Setup terminal to be able to output on screen
	call install_terminal

	# Push magic number and the location of the multiboot info table
	# on to the stack. These will be passed to kernel_early() as arguments. 
	push $0xc0ffee
	pushl bootinfo

	# Initialize the core kernel before running the global constructors.
	call kernel_early

	# Call the global constructors.
	call _init
	call __cxa_finalize

	# Install drivers before calling kernel_main().
	call install_rtc
	call install_threads
	call install_keyboard

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

.global _DMAComplete			# Allows the C code to link to this
.type _DMAComplete, @function
.set x_DMAPort,	8
_DMAComplete:
	push	%ebp
	mov	%esp, %ebp

	mov	$0x0c, %dx
	mov	$0xff, %al
	out	%al, %dx

	mov	-x_DMAPort(%ebp), %edx
	in	%dx, %al
	mov	%al, %bl
	in	%dx, %al
	mov	%al, %bh
	in	%dx, %al
	mov	%al, %ah
	in	%dx, %al
	xchg	%al, %ah
	sub	%ax, %bx
	cmp	$0x40, %bx
	jg	_DMAComplete
	cmp	$0xffc0, %bx
	jl	_DMAComplete

	mov %ebp, %esp
	pop %ebp
	ret

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

# Set the size of the _start symbol to the current location '.' minus its start. 
# This is useful when debugging or when you implement call tracing. 
.size _start, . - _start
