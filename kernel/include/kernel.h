#pragma once
#ifndef _KERNEL_H
#define _KERNEL_H 1

#ifdef __cplusplus
extern "C" {
#endif

void set_ticks(unsigned long long ticks);
unsigned long long ticks();

#ifdef __cplusplus
}
#endif

#endif
