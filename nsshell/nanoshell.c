// CrappyScript (C) 2023 iProgramInCpp

#include "nanoshell.h"

#ifndef NANOSHELL

void LogMsgNoCr(const char* fmt, ...)
{
	va_list lst;
	va_start(lst, fmt);
	vprintf(fmt, lst);
	va_end(lst);
}

void LogMsg(const char* fmt, ...)
{
	va_list lst;
	va_start(lst, fmt);
	vprintf(fmt, lst);
	puts("");
	va_end(lst);
}

#endif
