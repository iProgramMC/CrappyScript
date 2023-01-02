// NanoShell Command Interpreter (C) 2023 iProgramInCpp

#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#include <errno.h>
#endif

#include "nanoshell.h"
#include "shell.h"

#define C_MAX_COMMAND_LEN (8192)

#ifdef _WIN32
#define getcwd _getcwd
#endif

char g_cwd[256];

void ShellExecuteBatchFile(const char* pfn)
{
	LoadFile(pfn);
}

int main()
{
	// prepare CWD
	if (!getcwd(g_cwd, sizeof g_cwd))
	{
		LogMsg("Couldn't getcwd?!? Errno: %s", strerror(errno));
		return 1;
	}

	ShellExecuteBatchFile("test.nss");

	return 0;
}
