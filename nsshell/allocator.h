#pragma once

#include <stdint.h>
#include <stdlib.h>

#define MEM_DEBUG

#ifdef MEM_DEBUG

void* MemAllocateD(size_t sz, const char* file, int line);
void* MemCAllocateD(size_t nmemb, size_t sz, const char* file, int line);
void* MemReAllocateD(void* ptr, size_t newsize, const char* file, int line);
void  MemFree(void* ptr);
void  MemDebugPrint();
char* StrDuplicateD(const char* c, const char* file, int line);

#define StrDuplicate(psz)          StrDuplicateD (psz,         __FILE__,__LINE__)
#define MemAllocate(sz)            MemAllocateD  (sz,          __FILE__,__LINE__)
#define MemCAllocate(nmemb,sz)     MemCAllocateD (nmemb, sz,   __FILE__,__LINE__)
#define MemReAllocate(ptr,newsize) MemReAllocateD(ptr, newsize,__FILE__,__LINE__)

#else

#define MemAllocate(sz) malloc(sz)
#define MemCAllocate(nmemb,sz) calloc(nmemb,sz)
#define MemReAllocate(ptr,newsize) realloc(ptr,newsize)
#define MemFree(ptr) free(ptr)
#define MemDebugPrint()

#endif
