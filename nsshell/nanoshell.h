// CrappyScript (C) 2023 iProgramInCpp

#ifndef _NANOSHELL_H
#define _NANOSHELL_H

#include <stdio.h>
#include <stdarg.h>

#include "allocator.h"

void LogMsg(const char*, ...);
void LogMsgNoCr(const char*, ...);
void CoGetString(char* buffer, size_t szb);
void GetRidOfTrailingNewLine(char* buffer);
void ShellPrintMotd();
void ShellExecuteCommand(const char* buffer);

#endif//_NANOSHELL_H
