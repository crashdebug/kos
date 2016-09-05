#include "rtc.h"
#include <stdbool.h>
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
