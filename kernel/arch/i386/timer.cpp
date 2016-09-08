#include "irq.h"
#include "rtc.h"
#include "timer.h"
#include <time.h>
#include <kernel.h>
#include "util.h"

#define TIMER_E	( 3000.0 / 3579545 )
float frequency = 10000; // Hz

static time_t last_rtc_refresh = 0;

// Handles the timer.
// Increments the 'iTimeTicks' variable every time the timer fires.
void TimerHandler(void*)
{
	if (last_rtc_refresh < 100)
	{
		unsigned long long t = ticks();
		t++;
		set_ticks(t);
	}
	else
	{
		// Get the current time from BIOS/CMOS/RTC
		struct tm t;
		getDateTime(&t.tm_year, &t.tm_mon, &t.tm_mday, &t.tm_hour, &t.tm_min, &t.tm_sec);
		set_ticks(mktime(&t));
		last_rtc_refresh = 0;
	}
	last_rtc_refresh++;
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

	// Get the current time from BIOS/CMOS/RTC
	struct tm t;
	getDateTime(&t.tm_year, &t.tm_mon, &t.tm_mday, &t.tm_hour, &t.tm_min, &t.tm_sec);
	set_ticks(mktime(&t));

	unsigned int divisor = (unsigned int)((3579545.0 / 3.0) / frequency);
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
