#include "threads.h"
#include "irq.h"
#include "rtc.h"
#include <time.h>
#include <kernel.h>
#include "util.h"
#include <stdio.h>

#define TIMER_E	(3579545.0 / 3.0)
// Timer frequency in Hz
#define frequency 10000

// Handles the timer.
// Increments the 'ticks' variable every time the timer fires.
void thread_handler(void*)
{
	/*if ((++_t % 10) == 0)
	{
		time_t t = ticks();
		t += 1;
		set_ticks(t);
	}*/

	// TODO: Thread switching
}

// Tells the kernel about the Timer driver. 
// Called from boot.s.
extern "C" void install_threads()
{
	// Install the tick handler
//	install_irq_handler(0, thread_handler);

	// TODO: Check for APIC
	uint32_t eax, edx;
	cpuid(1, &eax, &edx);
	if ((edx & CPUID_FEAT_EDX_APIC) == CPUID_FEAT_EDX_APIC)
	{
		printf("APIC available. ");
	}
/*
	// Install PIT
	unsigned int divisor = (unsigned int)((TIMER_E) / frequency);
	printf("Tick freq = %i\n", divisor);
	// Set up the tick frequency
	for (unsigned char i = 0; i < 5; i++)
	{
		outb(0x43, 0x34); // 00110100b
		outb(0x40, (unsigned char)(divisor & 0xff));
		outb(0x40, (unsigned char)((divisor >> 8) & 0xff));
		io_wait();
		// TODO Check values are really set?
	}*/
}
