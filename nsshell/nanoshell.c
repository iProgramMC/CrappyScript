// NanoShell Command Interpreter (C) 2023 iProgramInCpp

#include "nanoshell.h"

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

void CoGetString(char* buffer, size_t szb)
{
	fgets(buffer, (int)szb, stdin);
}


void GetRidOfTrailingNewLine(char* buffer)
{
	char* b;
	b = strrchr(buffer, '\n');
	if (b) *b = 0;
	b = strrchr(buffer, '\r');
	if (b) *b = 0;
}

void ShellPrintMotd()
{
	LogMsg("Hello, this is NanoShell. How may I help you?");
}

void ShellExecuteCommand(const char* buffer)
{
	LogMsg("Got %s", buffer);
}
