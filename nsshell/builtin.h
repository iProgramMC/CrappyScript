#pragma once

#include "shell.h"

char* BuiltInHelp();
char* BuiltInVersion();
char* BuiltInGetVer();
char* BuiltInEcho(char* str);
char* BuiltInEquals(char* str1, char* str2);
char* BuiltInConcat(char* str1, char* str2);
