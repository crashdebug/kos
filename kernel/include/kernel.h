#pragma once
#ifndef _KERNEL_H
#define _KERNEL_H 1

#include <idriver.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

void set_ticks(time_t ticks);
time_t ticks();
void install_driver(IDriver* driver);

#ifdef __cplusplus
}
#endif

#endif
