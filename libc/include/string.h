#pragma once
#ifndef _KOS_STRING_H
#define _KOS_STRING_H 1

#include <stddef.h>
 
#ifdef __cplusplus
#ifdef TEST
namespace kos {
#endif
extern "C" {
#endif

int memcmp(const void*, const void*, size_t);
void* memcpy(void* __restrict, const void* __restrict, size_t);
void* memmove(void*, const void*, size_t);
void* memset(void*, int, size_t);
size_t strlen(const char*);
int strcmp(const char* lhs, const char* rhs);

#ifdef __cplusplus
}
#ifdef TEST
}
#endif
#endif


#endif // _KOS_STRING_H
