#pragma once
#ifndef _RTC_H
#define _RTC_H 1

#ifdef __cplusplus
extern "C" {
#endif

void install_rtc();
void sleep(unsigned long long msecs);

#ifdef __cplusplus
}
#endif

#endif
