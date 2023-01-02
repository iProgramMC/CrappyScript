// NanoShell Command Interpreter (C) 2023 iProgramInCpp

#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <direct.h>
#include "nanoshell.h"
#include "shell.h"

#define C_MAX_COMMAND_LEN (8192)

#define getcwd _getcwd

char g_cwd[256];

void ShellExecuteBatchFile(const char* pfn)
{
	LoadFile(pfn);
}

int main()
{
	// prepare CWD
	getcwd(g_cwd, sizeof g_cwd);

	ShellExecuteBatchFile("test.nss");

	return 0;
}
