#pragma once

void* operator new (__SIZE_TYPE__ size, unsigned char*& address);
void* operator new[] (__SIZE_TYPE__ size, void*& address);
#ifndef __lib
void* operator new (__SIZE_TYPE__ size);
void* operator new[] (__SIZE_TYPE__ size);
void operator delete (void* address);
void operator delete[] (void* address);
void operator delete (void* address, __SIZE_TYPE__ size);
void operator delete[] (void* address, __SIZE_TYPE__ size);
#endif
