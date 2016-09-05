#include "irq.h"
#include "rtc.h"
#include "timer.h"
#include <time.h>
#include <kernel.h>
#include "util.h"

//unsigned long long ticks = 0;

unsigned short frequency = (unsigned short)(0.1 / TIMER_E);

// Handles the timer.
// Increments the 'iTimeTicks' variable every time the timer fires.
void TimerHandler(void*)
{
	unsigned long long t = ticks();
	t++;
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
	// Get the current time from BIOS/CMOS/RTC
	struct tm t;
	getDateTime(&t.tm_year, &t.tm_mon, &t.tm_mday, &t.tm_hour, &t.tm_min, &t.tm_sec);
	set_ticks(mktime(&t));

	// Set up the tick frequency
	outb(0x43, 0x34); // 00110100b
	outb(0x40, (unsigned char)(frequency & 0xff));
	outb(0x40, (unsigned char)((frequency >> 8) & 0xff));

	// Install the tick handler
    install_irq_handler(0, TimerHandler);
}

/*void Timer::Sleep(unsigned int milliseconds)
{
	unsigned long long target = ticks() + milliseconds * 10;
	while (ticks() < target) ;
}

void Timer::GetDateTime(unsigned long long* milliseconds)
{
	*milliseconds = ticks();
}*/
