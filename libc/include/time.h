#pragma once
/*
ticks/y		=          3 153 600 000 000
ticks/1000y	=      3 153 600 000 000 000
max ulong	= 18 446 744 073 709 551 615
*/

#ifndef _KOS_TIME_H
#define _KOS_TIME_H 1

#ifdef TEST
namespace kos {
#endif

#ifdef __cplusplus
extern "C" {
#endif 

typedef unsigned long long time_t;

struct tm
{
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
};

time_t time(time_t* arg);
time_t mktime(struct tm* time);
struct tm* gmtime(const time_t* time);

#ifdef TEST
}
#endif

#ifdef __cplusplus
}
#endif 

#define TICKS_PER_MILLISECOND	time_t(1)
#define TICKS_PER_SECOND		time_t(1000 * TICKS_PER_MILLISECOND)
#define TICKS_PER_MINUTE		time_t(60 * TICKS_PER_SECOND)
#define TICKS_PER_HOUR			time_t(60 * TICKS_PER_MINUTE)
#define TICKS_PER_DAY			time_t(24 * TICKS_PER_HOUR)
#define SECONDS_PER_YEAR		time_t(31536000ULL)
#define SECONDS_PER_LEAP_YEAR	time_t(31622400ULL)

#endif
