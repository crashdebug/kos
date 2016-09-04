#pragma once
#ifndef _UTIL_H
#define _UTIL_H 1

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

unsigned char inportb(unsigned short port);
void outportb(unsigned short port, unsigned char data);
void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
void io_wait(void);

#ifdef __cplusplus
}
#endif

#endif
