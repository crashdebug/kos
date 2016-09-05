#pragma once
#ifndef _KERNEL_H
#define _KERNEL_H 1

#include <idriver.h>

#ifdef __cplusplus
extern "C" {
#endif

void set_ticks(unsigned long long ticks);
unsigned long long ticks();
void install_driver(IDriver* driver);

#ifdef __cplusplus
}
#endif

#endif
