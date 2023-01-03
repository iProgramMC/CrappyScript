// CrappyScript (C) 2023 iProgramInCpp

#include "allocator.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

// This allocator is designed to track allocations for memory leaks. Quick and dirty, not very performant.

typedef struct
{
	void* ptr;
	const char* file;
	int line;
}
Allocation;

static Allocation* g_allocations;
static size_t      g_nAllocations;

void DebugLogMsg(const char* fmt, ...)
{
	va_list lst;
	va_start(lst, fmt);
	char buffer[1024];
	vsnprintf(buffer, sizeof buffer, fmt, lst);

#ifdef _WIN32
	OutputDebugStringA(buffer);
	OutputDebugStringA("\n");
#else
	printf("%s\n", buffer);
#endif

	va_end(lst);
}

static void AddAllocation(Allocation* allocation)
{
	Allocation* allocs = realloc(g_allocations, (g_nAllocations + 1) * sizeof(Allocation));
	if (!allocs) abort();
	g_allocations = allocs;
	memcpy(&g_allocations[g_nAllocations++], allocation, sizeof *allocation);
}

static int LookUpAllocation(void* ptr)
{
	for (int i = 0; i < (int)g_nAllocations; i++)
	{
		if (g_allocations[i].ptr == ptr)
			return i;
	}
	return -1;
}

static void RemoveAllocation(int allocindex)
{
	memmove(&g_allocations[allocindex], &g_allocations[allocindex + 1], sizeof(Allocation) * (g_nAllocations - allocindex - 1));
	g_allocations = realloc(g_allocations, sizeof(Allocation) * (g_nAllocations - 1));
	g_nAllocations--;
	//if we should have allocations but we don't, abort
	if (!g_allocations && g_nAllocations) abort();
}

void* MemAllocateD(size_t sz, const char* file, int line)
{
	void* area = malloc(sz);
	if (!area) return area;

	Allocation all = {
		area,
		file,
		line
	};

	AddAllocation(&all);
	return area;
}

void* MemCAllocateD(size_t nmemb, size_t sz, const char* file, int line)
{
	void* area = calloc(nmemb, sz);
	if (!area) return area;

	Allocation all = {
		area,
		file,
		line
	};

	AddAllocation(&all);
	return area;
}

void* MemReAllocateD(void* oldptr, size_t newsize, const char* file, int line)
{
	if (!oldptr)
	{
		return MemAllocateD(newsize, file, line);
	}

	int areaindex = LookUpAllocation(oldptr);
	if (areaindex < 0)
	{
		DebugLogMsg("FATAL ERROR: realloc - OldPtr %p isn't part of the allocation system", oldptr);
		MemDebugPrint();
		abort();
	}

	void* area = realloc(oldptr, newsize);
	if (!area) return area;

	Allocation all = {
		area,
		file,
		line
	};

	g_allocations[areaindex] = all;
	return area;
}

void MemFree(void* ptr)
{
	if (!ptr) return;

	int areaindex = LookUpAllocation(ptr);
	if (areaindex < 0)
	{
		DebugLogMsg("FATAL ERROR: free - ptr %p isn't part of the allocation system", ptr);
		abort();
	}

	free(ptr);
	RemoveAllocation(areaindex);
}

void MemDebugPrint()
{
	DebugLogMsg("Starting MemDebugPrint!");
	for (int i = 0; i < (int)g_nAllocations; i++)
	{
		DebugLogMsg("ALLOCATION: %p (%s:%d)", g_allocations[i].ptr, g_allocations[i].file, g_allocations[i].line);
	}
}

char* StrDuplicateD(const char* text, const char * file, int line)
{
	size_t len = strlen(text);
	void* marea = MemAllocateD(len + 1, file, line);
	if (!marea) return marea;
	memcpy(marea, text, len + 1);
	return marea;
}
