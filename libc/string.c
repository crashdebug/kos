#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void* memset(void* bufptr, int value, size_t size)
{
	unsigned char* buf = (unsigned char*) bufptr;
	for (size_t i = 0; i < size; i++)
		buf[i] = (unsigned char)value;
	return bufptr;
}

void* memcpy(void* __restrict dstptr, const void* __restrict srcptr, size_t size)
{
	unsigned char* dst = (unsigned char*)dstptr;
	const unsigned char* src = (const unsigned char*)srcptr;
	for (size_t i = 0; i < size; i++)
		dst[i] = src[i];
	return dstptr;
}

#ifdef __cplusplus
}
#endif
