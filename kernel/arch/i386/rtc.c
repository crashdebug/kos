#include "rtc.h"
#include <stdbool.h>
#include <time.h>
#include <kernel.h>
#include "irq.h"
#include "pic.h"
#include "util.h"

#define RTC_ADDRESS_PORT	0x70
#define RTC_DATA_PORT		0x71

#define RTC_YEAR_REGISTER	0x09
#define RTC_MONTH_REGISTER	0x08
#define RTC_DAY_REGISTER	0x07
#define RTC_HOUR_REGISTER	0x04
#define RTC_MINUTE_REGISTER	0x02
#define RTC_SECOND_REGISTER	0x00

/// Assume BCD on. Most PC's return digits in BCD format (0x10 = 10).
bool bcd = true;

/// Converts 0x12 -> 12
/// BCD values are hex number representing a decimal number (0x10 = 10).
/// Returns false if no conversion could be made. Signals to switch BCD off.
bool convertFromBCD(unsigned char value, unsigned char* out)
{
	// Values like 0x0a, 0x1b, 0x2c, 0x3d, 0x4f cannot be BCD values
	if ((value & 0xf) >= 0xa && (value & 0xf) <= 0xf)
	{
		*out = value;
		return false;
	}

	*out = 0;
	while (value >= 0x10)
	{
		*out += 10;
		value -= 0x10;
	}
	*out += value;
	return true;
}

/// Get time and date from RTC
void getDateTime(int* year, int* month, int* day, int* hours, int* minutes, int* seconds)
{
	// Wait until RTC becomes available to read
	while (true)
	{
		outb(RTC_ADDRESS_PORT, 0x0a);
		if ((inb(RTC_DATA_PORT) & 0x80) != 0x80)
			break;
	}

	unsigned char temp;

	// Get seconds
	outb(RTC_ADDRESS_PORT, RTC_SECOND_REGISTER);
	temp = inb( RTC_DATA_PORT);
	// Check if BCD is used
	if (bcd && !convertFromBCD(temp, &temp))
		bcd = false;
	*seconds = temp;

	// Get minutes
	outb(RTC_ADDRESS_PORT, RTC_MINUTE_REGISTER);
	temp = inb(RTC_DATA_PORT);
	// Check if BCD is used
	if (bcd && !convertFromBCD(temp, &temp))
		bcd = false;
	*minutes = temp;

	// Get hours
	outb(RTC_ADDRESS_PORT, RTC_HOUR_REGISTER);
	temp = inb(RTC_DATA_PORT);
	// Check if BCD is used
	if (bcd && !convertFromBCD(temp, &temp))
		bcd = false;
	*hours = temp;

	// Get day
	outb(RTC_ADDRESS_PORT, RTC_DAY_REGISTER);
	temp = inb(RTC_DATA_PORT);
	// Check if BCD is used
	if (bcd && !convertFromBCD(temp, &temp))
		bcd = false;
	*day = temp;

	// Get month
	outb(RTC_ADDRESS_PORT, RTC_MONTH_REGISTER);
	temp = inb(RTC_DATA_PORT);
	// Check if BCD is used
	if (bcd && !convertFromBCD(temp, &temp))
		bcd = false;
	*month = temp;

	// Get year
	outb(RTC_ADDRESS_PORT, RTC_YEAR_REGISTER);
	temp = inb(RTC_DATA_PORT);
	// Check if BCD is used
	if (bcd && !convertFromBCD(temp, &temp))
		bcd = false;
	*year = temp + 2000U;
}

static float _ticks = 0;
static time_t _time = 0;

// Get the current time from BIOS/CMOS/RTC
void set_time()
{
	struct tm t;
	getDateTime(&t.tm_year, &t.tm_mon, &t.tm_mday, &t.tm_hour, &t.tm_min, &t.tm_sec);
	_time = mktime(&t);
	set_ticks(_time);
}

// Handles the timer.
// Increments the 'ticks' variable every time the timer fires.
void rtc_handler(void*)
{
	// Register C must be read, otherwise the interrupt will not trigger again.
	// select register C
	outb(0x70, 0x0C);
	// just throw away contents
	inb(0x71);

	if (_ticks >= 16)
	{
		set_time();
		_ticks -= 16;
	}
	_ticks++;
}

void install_rtc()
{
	char prev;

	// Install the tick handler on IRQ 8
    install_irq_handler(8, rtc_handler);

	// Make sure IRQ8 is not masked
	pic_clear_mask(8);

	// Select register B, and disable NMI
	outb(0x70, 0x8B);
	// Read the current value of register B
	prev = inb(0x71);
	// Set the index again (a read will reset the index to register D)
	outb(0x70, 0x8B);
	// Write the previous value ORed with 0x40. This turns on bit 6 of register B
	outb(0x71, prev | 0x40);

	// rate must be above 2 and not over 15
	char rate = 0x06; // 0x06 = 1024Hz
	// set index to register A, disable NMI
	outb(0x70, 0x8A);
	// get initial value of register A
	prev = inb(0x71);
	// reset index to A
	outb(0x70, 0x8A);
	// Write only our rate to A. Note, rate is the bottom 4 bits.
	outb(0x71, (prev & 0xF0) | rate);
}
