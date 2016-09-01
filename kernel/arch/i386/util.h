#pragma once
#ifndef _UTIL_H
#define _UTIL_H 1

#ifdef __cplusplus
extern "C" {
#endif

unsigned char inportb(unsigned short port);
void outportb(unsigned short port, unsigned char data);

#ifdef __cplusplus
}
#endif

#endif
