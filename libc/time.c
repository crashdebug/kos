#include <time.h>

unsigned int SecondsPerMonth[]		= { 2678400, 2419200, 2678400, 2592000, 2678400, 2592000, 2678400, 2678400, 2592000, 2678400, 2592000, 2678400 };
unsigned int SecondsPerMonthLeap[]	= { 2678400, 2505600, 2678400, 2592000, 2678400, 2592000, 2678400, 2678400, 2592000, 2678400, 2592000, 2678400 };
unsigned char DaysPerMonth[]		= { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
unsigned char DaysPerMonthLeap[]	= { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

// Provided by kernel
extern "C" time_t ticks();

#ifdef __cplusplus
extern "C" {
#endif 

time_t time(time_t* arg)
{
	time_t t(ticks());
	if (arg != 0)
	{
		*arg = t;
	}
	return t;
}

bool is_leap_year(int year )
{
	return (year != 0 && ((year % 100) != 0 || (year % 400) == 0) && (year % 4) == 0);
}

time_t mktime(struct tm* time)
{
	time_t t = 0;
	t += TICKS_PER_SECOND * time->tm_sec;
	t += TICKS_PER_MINUTE * time->tm_min;
	t += TICKS_PER_HOUR * time->tm_hour;
	t += TICKS_PER_DAY * time->tm_mday;

	int month = time->tm_mon;
	bool leap = is_leap_year(time->tm_year); 
	while (month > 0)
	{
		if (leap)
		{
			t += SecondsPerMonthLeap[month] * TICKS_PER_SECOND;
		}
		else
		{
			t += SecondsPerMonth[month] * TICKS_PER_SECOND;
		}
		month--;
	}

	int year = time->tm_year;
	while (year > 0)
	{
		if (is_leap_year(year))
		{
			t += SECONDS_PER_LEAP_YEAR * TICKS_PER_SECOND;
		}
		else
		{
			t += SECONDS_PER_YEAR * TICKS_PER_SECOND;
		}
		year--;
	}
	return t;
}

int year(time_t* remain)
{
	int year = 0;
	while (true)
	{
		if (is_leap_year(year))
		{
			*remain -= SECONDS_PER_LEAP_YEAR * TICKS_PER_SECOND;
		}
		else
		{
			*remain -= SECONDS_PER_YEAR * TICKS_PER_SECOND;
		}
		year++;
		if (is_leap_year(year))
		{
			if (*remain < SECONDS_PER_LEAP_YEAR * TICKS_PER_SECOND)
			{
				break;
			}
		}
		else
		{
			if (*remain < SECONDS_PER_YEAR * TICKS_PER_SECOND)
			{
				break;
			}
		}
	}
	return year;
}

int month(time_t* remain, bool leap)
{
	unsigned char month = 0;
	while (*remain > (long long)(leap ? SecondsPerMonthLeap[month] : SecondsPerMonth[month]) * TICKS_PER_SECOND)
	{
		*remain -= (long long)(leap ? SecondsPerMonthLeap[month] : SecondsPerMonth[month]) * TICKS_PER_SECOND;
		month++;
	}
	return month;
}

int day(time_t* remain)
{
	unsigned char day = 0;
	while (*remain > TICKS_PER_DAY)
	{
		day++;
		*remain -= TICKS_PER_DAY;
	}
	return day;
}

int hours(time_t* remain)
{
	unsigned char hours = 0;
	while (*remain > TICKS_PER_HOUR)
	{
		hours++;
		*remain -= TICKS_PER_HOUR;
	}
	return hours;
}

int minutes(time_t* remain)
{
	unsigned char minutes = 0;
	while (*remain > TICKS_PER_MINUTE)
	{
		minutes++;
		*remain -= TICKS_PER_MINUTE;
	}
	return minutes;
}

int seconds(time_t* remain)
{
	unsigned char seconds = 0;
	while (*remain > TICKS_PER_SECOND)
	{
		seconds++;
		*remain -= TICKS_PER_SECOND;
	}
	return seconds;
}

struct tm* gmtime(const time_t *time)
{
	tm* t = new tm;
	if (*time > 0)
	{
		time_t temp = *time;
		t->tm_year = year(&temp);
		t->tm_mon = month(&temp, is_leap_year(t->tm_year));
		t->tm_mday = day(&temp);
		t->tm_hour = hours(&temp);
		t->tm_min = minutes(&temp);
		t->tm_sec = seconds(&temp);
	}
	else
	{
		t->tm_year = 0;
		t->tm_mon = 0;
		t->tm_mday = 0;
		t->tm_hour = 0;
		t->tm_min = 0;
		t->tm_sec = 0;
	}
	return t;
}

#ifdef __cplusplus
}
#endif 

/*
DateTime::DateTime( unsigned short iYear, unsigned char iMonth, unsigned char iDay, unsigned char iHours, unsigned char iMinutes, unsigned char iSeconds )
{
	this->Set( iYear, iMonth, iDay, iHours, iMinutes, iSeconds );
}

DateTime::DateTime( )
{
	Timer.GetDateTime( &this->lTicks );
}

void DateTime::Set( unsigned short iYear, unsigned char iMonth, unsigned char iDay, unsigned char iHours, unsigned char iMinutes, unsigned char iSeconds )
{
	this->lTicks = 0;

	this->lTicks += TICKS_PER_SECOND * iSeconds;
	this->lTicks += TICKS_PER_MINUTE * iMinutes;
	this->lTicks += TICKS_PER_HOUR * iHours;
	this->lTicks += TICKS_PER_DAY * iDay;

	while ( iMonth > 0 )
	{
		if ( IsLeapYear( iYear ) )
			this->lTicks += SecondsPerMonthLeap[iMonth] * TICKS_PER_SECOND;
		else
			this->lTicks += SecondsPerMonth[iMonth] * TICKS_PER_SECOND;
		iMonth--;
	}

	while ( iYear > 0 )
	{
		if ( IsLeapYear( iYear ) )
			this->lTicks += SECONDS_PER_LEAP_YEAR * TICKS_PER_SECOND;
		else
			this->lTicks += SECONDS_PER_YEAR * TICKS_PER_SECOND;
		iYear--;
	}
}

unsigned short DateTime::GetYear( )
{
	unsigned long long l;
	return this->GetYear( &l );
}

unsigned short DateTime::GetYear( unsigned long long * lRemain )
{
	unsigned short iYear = 0;
	*lRemain = this->lTicks;
	while ( true )
	{
		if ( IsLeapYear( iYear ) )
		{
			*lRemain -= SECONDS_PER_LEAP_YEAR * TICKS_PER_SECOND;
		}
		else
		{
			*lRemain -= SECONDS_PER_YEAR * TICKS_PER_SECOND;
		}
		iYear++;
		if ( IsLeapYear( iYear ) )
		{
			if ( *lRemain < SECONDS_PER_LEAP_YEAR * TICKS_PER_SECOND )
				break;
		}
		else
		{
			if ( *lRemain < SECONDS_PER_YEAR * TICKS_PER_SECOND )
				break;
		}
	}
	return iYear;
}

unsigned char DateTime::GetMonth( )
{
	unsigned long long lRemain;
	return this->GetMonth( &lRemain );
}

unsigned char DateTime::GetMonth( unsigned long long * lRemain )
{
	bool bLeapYear = IsLeapYear( this->GetYear( lRemain ) );
	unsigned char iMonth = 0;
	while ( *lRemain > (long long)( bLeapYear ? SecondsPerMonthLeap[iMonth] : SecondsPerMonth[iMonth] ) * TICKS_PER_SECOND )
	{
		*lRemain -= (long long)( bLeapYear ? SecondsPerMonthLeap[iMonth] : SecondsPerMonth[iMonth] ) * TICKS_PER_SECOND;
		iMonth++;
	}
	return iMonth;
}

unsigned char DateTime::GetDay( )
{
	unsigned long long lRemain;
	return this->GetDay( &lRemain );
}

unsigned char DateTime::GetDay( unsigned long long * lRemain )
{
	this->GetMonth( lRemain );
	unsigned char iDay = 0;
	while ( *lRemain > TICKS_PER_DAY )
	{
		iDay++;
		*lRemain -= TICKS_PER_DAY;
	}
	return iDay;
}

unsigned char DateTime::GetHours( )
{
	unsigned long long lRemain;
	return this->GetHours( &lRemain );
}

unsigned char DateTime::GetHours( unsigned long long * lRemain )
{
	this->GetDay( lRemain );
	unsigned char iHours = 0;
	while ( *lRemain > TICKS_PER_HOUR )
	{
		iHours++;
		*lRemain -= TICKS_PER_HOUR;
	}
	return iHours;
}

unsigned char DateTime::GetMinutes( )
{
	unsigned long long lRemain;
	return this->GetMinutes( &lRemain );
}

unsigned char DateTime::GetMinutes( unsigned long long * lRemain )
{
	this->GetHours( lRemain );
	unsigned char iMinutes = 0;
	while ( *lRemain > TICKS_PER_MINUTE )
	{
		iMinutes++;
		*lRemain -= TICKS_PER_MINUTE;
	}
	return iMinutes;
}

unsigned char DateTime::GetSeconds( )
{
	unsigned long long lRemain;
	return this->GetSeconds( &lRemain );
}

unsigned char DateTime::GetSeconds( unsigned long long * lRemain )
{
	this->GetMinutes( lRemain );
	unsigned char iSeconds = 0;
	while ( *lRemain > TICKS_PER_SECOND )
	{
		iSeconds++;
		*lRemain -= TICKS_PER_SECOND;
	}
	return iSeconds;
}

unsigned short DateTime::GetMilliseconds( )
{
	unsigned long long lRemain;
	return this->GetMilliseconds( &lRemain );
}

unsigned short DateTime::GetMilliseconds( unsigned long long * lRemain )
{
	this->GetSeconds( lRemain );
	unsigned short iMilliseconds = 0;
	while ( *lRemain > TICKS_PER_MILLISECOND )
	{
		iMilliseconds++;
		*lRemain -= TICKS_PER_MILLISECOND;
	}
	return iMilliseconds;
}

unsigned long long DateTime::GetTotalSeconds( )
{
	return ( this->lTicks / TICKS_PER_SECOND );
}

unsigned long long DateTime::GetTotalTicks( )
{
	return this->lTicks;
}

bool DateTime::IsLeapYear( )
{
	return IsLeapYear( this->GetYear( ) );
}

bool DateTime::IsLeapYear( unsigned short iYear )
{
	return ( iYear != 0 && ( ( iYear % 100 ) != 0 || ( iYear % 400 ) == 0 ) && ( iYear % 4 ) == 0 );
}

void DateTime::AddYears( unsigned short iYears )
{
	while ( iYears > 0 )
	{
		this->lTicks += ( this->IsLeapYear( ) ? SECONDS_PER_LEAP_YEAR : SECONDS_PER_YEAR ) * TICKS_PER_SECOND;
		iYears--;
	}
}

void DateTime::AddMonths( unsigned short iMonths )
{
	while ( iMonths > 12 )
	{
		this->AddYears( 1 );
		iMonths -= 12;
	}
	bool bLeapYear = this->IsLeapYear( );
	while ( iMonths > 0 )
	{
		this->lTicks += ( bLeapYear ? SecondsPerMonthLeap[iMonths] : SecondsPerMonth[iMonths] ) * TICKS_PER_SECOND;
		iMonths--;
	}
}

void DateTime::AddDays( unsigned short iDays )
{
	this->lTicks += iDays * TICKS_PER_DAY;
}

void DateTime::AddHours( unsigned int iHours )
{
	this->lTicks += iHours * TICKS_PER_HOUR;
}

void DateTime::AddMinutes( unsigned long long iMinutes )
{
	this->lTicks += iMinutes * TICKS_PER_MINUTE;
}

void DateTime::AddSeconds( unsigned long long iSeconds )
{
	this->lTicks += iSeconds * TICKS_PER_SECOND;
}

void DateTime::AddMilliseconds( unsigned long long iMilliseconds )
{
	this->lTicks += iMilliseconds * TICKS_PER_MILLISECOND;
}
*/
