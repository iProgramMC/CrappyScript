#include "shell.h"
#include "tokeniser.h"

FILE* g_file;
jmp_buf g_errorJumpBuffer;

const char* g_errorMsgs[] = {
	"Success",
	"",
	"Could not allocate memory",
	"Unterminated string",
	"Unterminated escape sequence",
	"Unknown escape sequence",
	"",
};

char GetErrorCategory(int error)
{
	if (error <= ERROR_NONE || error >= ERROR_END) return 'U';

	if (error >= ERROR_PARSER_START) return 'P';
	if (error >= ERROR_TOKENIZER_START) return 'T';

	return 'U';
}

int GetErrorNumber(int error)
{
	if (error <= ERROR_NONE || error >= ERROR_END) return error;

	if (error >= ERROR_PARSER_START) return error - ERROR_PARSER_START;
	if (error >= ERROR_TOKENIZER_START) return error - ERROR_TOKENIZER_START;

	return error;
}

const char* GetErrorMessage(int error)
{
	if (error < ERROR_NONE || error >= ERROR_END) return "Unknown error";

	return g_errorMsgs[error];
}

void LoadFile(const char* pfn)
{
	FILE* f = fopen(pfn, "r");
	if (!f)
	{
		LogMsg("File %s not found.", pfn);
		return;
	}
	g_file = f;
	
	int error = setjmp(g_errorJumpBuffer);
	if (error)
	{
		LogMsg("ERROR %c%04d: %s", GetErrorCategory(error), GetErrorNumber(error), GetErrorMessage(error));
		
		fclose(f);
		g_file = NULL;
		return;
	}

	Tokenise();
	TokensDump();

	fclose(f);
	g_file = NULL;
	return;
}
