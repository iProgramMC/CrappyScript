#include "shell.h"
#include "tokeniser.h"
#include "parser.h"
#include "runner.h"

FILE* g_file;
jmp_buf g_errorJumpBuffer;

const char* g_errorMsgs[] = {
	"Success",
	"",
	"Tokeniser could not allocate memory",
	"Unterminated string",
	"Unterminated escape sequence",
	"Unknown escape sequence",
	"Internal error: unknown symbol token",
	"Internal error: unknown keyword token",
	"",
	"Parser could not allocate memory",
	"Unterminated block statement",
	"Unterminated command statement",
	"Unterminated function declaration",
	"Internal error: not a block statement",
	"Internal error: not a command statement",
	"Internal error: not a function statement",
	"Expected statement",
	"Expected '('",
	"Expected ')'",
	"Expected ';'",
	"Expected 'then'",
	"Expected 'do'",
	"Expected '{'",
	"Expected '}'",
	"Expected ';' or arguments",
	"Expected ')' or arguments",
	"Expected ','",
	"Expected string",
	"Expected function name",
	"Expected arguments",
	"Expected block",
	"Expected function definition",
	"Expected command",
	"Expected 'if' statement",
	"Expected 'while' statement",
	"Expected string statement",
	"Expected ';' statement",
	"Expected variable definition",
	"Expected variable name",
	"",
	"Runner could not allocate memory",
	"Internal error: unknown statement type",
	"Unknown function or variable name",
	"Arguments specified for a variable as if it were a function call",
	"Too many arguments in function call",
	"Too few arguments in function call",
	"Too many arguments in function definition",
};

char GetErrorCategory(int error)
{
	if (error <= ERROR_NONE || error >= ERROR_END) return 'U';

	if (error >= ERROR_RUNTIME_START) return 'R';
	if (error >= ERROR_PARSER_START) return 'P';
	if (error >= ERROR_TOKENIZER_START) return 'T';

	return 'U';
}

int GetErrorNumber(int error)
{
	if (error <= ERROR_NONE || error >= ERROR_END) return error;

	if (error >= ERROR_RUNTIME_START) return error - ERROR_RUNTIME_START;
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

	//TokensDump();
	Parse();

	MemDebugPrint();

	LogMsg("Running.....\n\n");

	RunnerGo();

	MemDebugPrint();

	fclose(f);
	g_file = NULL;
	return;
}
