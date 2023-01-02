#include "nanoshell.h"
#include "builtin.h"

char* BuiltInHelp()
{
	LogMsg("Help was called!");
	return NULL;
}

char* BuiltInVersion()
{
	LogMsg("NanoShell Shell Version %s", SHELL_VERSION_STRING);
	return NULL;
}

char* BuiltInGetVer()
{
	return StrDuplicate(SHELL_VERSION_STRING);
}

char* BuiltInEcho(char * str)
{
	LogMsg("%s", str);
	return NULL;
}

char* BuiltInEquals(char* str1, char* str2)
{
	if (strcmp(str1, str2) == 0)
		return StrDuplicate("1");
	else
		return StrDuplicate("0");
}

char* BuiltInConcat(char* str1, char* str2)
{
	size_t len1 = strlen(str1), len2 = strlen(str2);

	char* cpy = MemAllocate(len1 + len2 + 1);
	memcpy(cpy, str1, len1);
	memcpy(cpy+len1, str2, len2+1);

	return cpy;
}
