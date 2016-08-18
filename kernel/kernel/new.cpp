#include <new.h>
#include <memory.h>

void* operator new (__SIZE_TYPE__, unsigned char*& address)
{
	return address;
}

void* operator new[] (__SIZE_TYPE__, void*& address)
{
	return address;
}

/// Overload the operator "new"
void * operator new (__SIZE_TYPE__ size)
{
	return MemoryManager::MemAlloc(size);
}

/// Overload the operator "new[]"
void * operator new[] (__SIZE_TYPE__ size)
{
	return MemoryManager::MemAlloc(size);
}

/// Overload the operator "delete"
void operator delete (void* p)
{
    MemoryManager::MemFree(p);
}

/// Overload the operator "delete[]"
void operator delete[] (void* p)
{
    MemoryManager::MemFree(p);
}

/// Overload the operator "delete"
void operator delete (void* p, __SIZE_TYPE__)
{
    MemoryManager::MemFree(p);
}

/// Overload the operator "delete[]"
void operator delete[] (void* p, __SIZE_TYPE__)
{
    MemoryManager::MemFree(p);
}
