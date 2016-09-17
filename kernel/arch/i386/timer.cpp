#include "irq.h"
#include "rtc.h"
#include "timer.h"
#include <time.h>
#include <kernel.h>
#include "util.h"

#define TIMER_E	(3579545.0 / 3.0)
// Timer frequency in Hz
#define frequency 10000

// Get the current time from BIOS/CMOS/RTC
time_t query_rtc()
{
	struct tm t;
	getDateTime(&t.tm_year, &t.tm_mon, &t.tm_mday, &t.tm_hour, &t.tm_min, &t.tm_sec);
	return mktime(&t);
}

// Handles the timer.
// Increments the 'ticks' variable every time the timer fires.
void TimerHandler(void*)
{
	time_t t = query_rtc();
	set_ticks(t);
}

// Tells the kernel about the Timer driver. 
// Called from boot.s.
extern "C" void install_timer()
{
	install_driver(&::Timer);
}

void Timer::install()
{
	// Install the tick handler
    install_irq_handler(0, TimerHandler);

	// Inform kernel what time it is
	set_ticks(query_rtc());

	// TODO: Check for APIC
	uint32_t eax, edx;
	cpuid(1, &eax, &edx);
	if ((edx & CPUID_FEAT_EDX_APIC) == CPUID_FEAT_EDX_APIC)
	{
	}

	// Install PIT
	unsigned int divisor = (unsigned int)((TIMER_E) / frequency);
	// Set up the tick frequency
	for (unsigned char i = 0; i < 5; i++)
	{
		outb(0x43, 0x34); // 00110100b
		outb(0x40, (unsigned char)(divisor & 0xff));
		outb(0x40, (unsigned char)((divisor >> 8) & 0xff));
		io_wait();
		// TODO Check values are really set?
	}
}

/*void Timer::Sleep(unsigned int milliseconds)
{
	unsigned long long target = ticks() + milliseconds * 10;
	while (ticks() < target) ;
}
*/
