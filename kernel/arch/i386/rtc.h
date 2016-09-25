#pragma once
#ifndef _RTC_H
#define _RTC_H 1

#ifdef __cplusplus
extern "C" {
#endif

void getDateTime(int* year, int* month, int* day, int* hours, int* minutes, int* seconds);
void install_rtc();

#ifdef __cplusplus
}
#endif

#endif
