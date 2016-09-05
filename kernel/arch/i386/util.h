#pragma once
#ifndef _UTIL_H
#define _UTIL_H 1

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
void io_wait(void);

#ifdef __cplusplus
}
#endif

#endif
