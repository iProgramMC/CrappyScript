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

void ShellExecuteFile(const char* pfn, int argc, char** argv);

int main(int argc, char ** argv)
{
	if (argc < 2)
	{
		LogMsg("Usage: %s <script file> <arguments>", argv[0]);
		return 0;
	}

	ShellExecuteFile(argv[1], argc - 2, argv + 2);

	return 0;
}
