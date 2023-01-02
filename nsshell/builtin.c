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
	return strdup(SHELL_VERSION_STRING);
}

char* BuiltInEcho(char * str)
{
	LogMsg("%s", str);
	return NULL;
}
