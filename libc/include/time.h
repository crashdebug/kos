#pragma once
/*
ticks/y		=          3 153 600 000 000
ticks/1000y	=      3 153 600 000 000 000
max ulong	= 18 446 744 073 709 551 615
*/

#ifndef _TIME_H
#define _TIME_H 1

#define TICKS_PER_MILLISECOND	(long long)10
#define TICKS_PER_SECOND		(long long)10000
#define TICKS_PER_MINUTE		(long long)600000
#define TICKS_PER_HOUR			(long long)36000000
#define TICKS_PER_DAY			(long long)864000000
#define SECONDS_PER_YEAR		(long long)31536000
#define SECONDS_PER_LEAP_YEAR	(long long)31622400

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

#ifdef __cplusplus
}
#endif 

#endif
