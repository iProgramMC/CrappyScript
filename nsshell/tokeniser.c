// CrappyScript (C) 2023 iProgramInCpp

#include "nanoshell.h"
#include "tokeniser.h"
#include "shell.h"

extern jmp_buf g_errorJumpBuffer;
extern FILE* g_file;

int g_lineNum = 0;

const char* g_doubleSymbolTokens = "<>=";

// note: this must match the order in the enum eToken from TK_SYMBOL_START
const char* g_singleSymbolTokens = ";{}(),=+-*/<>&^|!~";

// note: this must match the exact order in the enum eToken from TK_KEYWORD_START
const char* gKeywordKeys[] =
{
	"",
	"if",
	"then",
	"else",
	"while",
	"do",
	"finally",
	"function",
	"fun",
	"let",
	"var",
	"set",
	"to",
	"be",
	"return",
};

Token** tokens = NULL;
size_t ntokens = 0;

NORETURN void TokenOnError(int error)
{
	LogMsg("At line %d:", g_lineNum);
	longjmp(g_errorJumpBuffer, error);
}

// note: this could be done better
void TokenAppend(char** token, size_t* sz, char chr)
{
	if (*sz == 0)
	{
		*sz = 1;
	}

	char* newToken = MemReAllocate(*token, (*sz) + 1);

	if (!newToken)
	{
		TokenOnError(ERROR_MEMORY_ALLOC_FAILURE);
		return;
	}

	*token = newToken;

	(*token)[(*sz - 1)] = chr;
	(*token)[*sz] = 0;
	(*sz)++;
}

// same with this. TODO reduce the number of MemReAllocates for TokenAdd and TokenAppend

// note: this must take a MemAllocate'ed buffer, or NULL

void TokenAdd(int type, char* data, int line)
{
	// don't add at all if it's an empty key word
	if (type == TK_KEYWORD_START && data == NULL)
		return;

	Token** newTokens = MemReAllocate(tokens, sizeof(Token*) * (ntokens + 1));
	if (!newTokens) TokenOnError(ERROR_MEMORY_ALLOC_FAILURE);

	tokens = newTokens;

	Token* pToken = MemCAllocate(1, sizeof(Token));
	if (!pToken) TokenOnError(ERROR_MEMORY_ALLOC_FAILURE);

	tokens[ntokens++] = pToken;

	pToken->m_type = type;
	pToken->m_data = data;
	pToken->m_line = line;

	// determine the type
	if (type == TK_SYMBOL_START)
	{
		pToken->m_data = NULL;

		if (data[1])
		{
			switch (*data)
			{
				case '<': pToken->m_type = data[1] == '=' ? TK_LE : TK_LSHIFT; break;
				case '>': pToken->m_type = data[1] == '=' ? TK_GE : TK_RSHIFT; break;
				case '=': pToken->m_type = TK_DEQUALS; break;
			}
		}
		else switch (*data)
		{
			case ';': pToken->m_type = TK_SEMICOLON;  break;
			case '{': pToken->m_type = TK_OPENBLOCK;  break;
			case '}': pToken->m_type = TK_CLOSEBLOCK; break;
			case '(': pToken->m_type = TK_OPENPAREN;  break;
			case ')': pToken->m_type = TK_CLOSEPAREN; break;
			case ',': pToken->m_type = TK_COMMA;      break;
			case '=': pToken->m_type = TK_EQUALS;     break;
			case '+': pToken->m_type = TK_PLUS;       break;
			case '-': pToken->m_type = TK_MINUS;      break;
			case '*': pToken->m_type = TK_TIMES;      break;
			case '/': pToken->m_type = TK_DIVIDE;     break;
			case '<': pToken->m_type = TK_LT;         break;
			case '>': pToken->m_type = TK_GT;         break;
			case '&': pToken->m_type = TK_AND;        break;
			case '^': pToken->m_type = TK_XOR;        break;
			case '|': pToken->m_type = TK_OR;         break;
			case '!': pToken->m_type = TK_NOT;        break;
			case '~': pToken->m_type = TK_BNOT;       break;
			default: TokenOnError(ERROR_INTERNAL_UNKNOWN_SYMBOL_TOKEN);
		}

		MemFree(data);
	}

	if (type == TK_KEYWORD_START)
	{
		for (int i = TK_KEYWORD_START; i < TK_KEYWORD_END; i++)
		{
			if (strcmp(pToken->m_data, gKeywordKeys[i - TK_KEYWORD_START]) == 0)
			{
				MemFree(pToken->m_data);
				pToken->m_type = i;
				pToken->m_data = NULL;
				return;
			}
		}

		// If this key word is made up purely of numbers, create a number.
		for (char* pchr = pToken->m_data; *pchr != 0; pchr++)
		{
			if (!isdigit(*pchr))
				return;
		}

		// This is actually a number
		long long number = atoll(pToken->m_data);

		pToken->m_type = TK_NUMBER;
		MemFree(pToken->m_data);
		pToken->m_data = MemAllocate(sizeof(long long));
		*((long long*)pToken->m_data) = number;
		return;
	}
}

bool IsSpaceSafe(char c)
{
	if (c < -1) return false;
	return isspace(c);
}

bool IsControlSafe(char c)
{
	if (c < -1) return false;
	return iscntrl(c);
}

int FileGetChar(FILE* f)
{
	#ifndef NANOSHELL
	return fgetc(f);
	#else
	int chr = 0;
	size_t sz = fread(&chr, 1, 1, f);
	if (sz <= 0)
		return -1;
	return (int)chr;
	#endif
}

void Tokenise()
{
	char* currentToken = NULL;
	size_t currentTokenSize = 0;
	g_lineNum = 1;

	while (!feof(g_file))
	{
		int cint = FileGetChar(g_file);
		char c = (char)cint;
	DontRead:
		if (cint == EOF) break; // Er, but feof is false? Just making sure..

		// If this is a hash, we're starting a new comment.
		if (c == '#')
		{
			while (c != EOF)
			{
				c = (char)FileGetChar(g_file);
				if (c == '\n')
					break;
			}
			continue;
		}

		// if this is white space
		if (IsSpaceSafe(c) || IsControlSafe(c))
		{
			// push the current token, if there is one
			TokenAdd(TK_KEYWORD_START, currentToken, g_lineNum);
			currentToken = NULL;
			currentTokenSize = 0;

			if (c == '\n')
			{
				g_lineNum++;
			}

			continue;
		}

		// if it is one of these symbols..
		char* match = strchr(g_singleSymbolTokens, c);
		if (match != NULL)
		{
			bool shouldNotRead = false;
			char nc;
			if (strchr(g_doubleSymbolTokens, c))
			{
				nc = FileGetChar(g_file);
				
				if (nc == '=' || nc == c)
				{
					// push the current token, if there is one
					TokenAdd(TK_KEYWORD_START, currentToken, g_lineNum);
					currentToken = NULL;
					currentTokenSize = 0;

					currentToken = MemAllocate(3);
					currentToken[0] = c;
					currentToken[1] = nc;
					currentToken[2] = 0;
					TokenAdd(TK_SYMBOL_START, currentToken, g_lineNum);
					currentToken = NULL;
					continue;
					// that character should be skipped
				}

				shouldNotRead = true;
			}

			// push the current token, if there is one
			TokenAdd(TK_KEYWORD_START, currentToken, g_lineNum);
			currentToken = NULL;
			currentTokenSize = 0;

			// add a NEW token, this being the single symbol
			currentToken = MemAllocate(2);
			currentToken[0] = c;
			currentToken[1] = 0;
			TokenAdd(TK_SYMBOL_START, currentToken, g_lineNum);
			currentToken = NULL;

			if (shouldNotRead)
			{
				c = nc;
				goto DontRead;
			}

			continue;
		}

		// if it's a quotation mark
		if (c == '"')
		{
			// push the current token, if there is one
			TokenAdd(TK_KEYWORD_START, currentToken, g_lineNum);
			currentToken = NULL;
			currentTokenSize = 0;

			// while we have characters
			while (!feof(g_file))
			{
				int cint = FileGetChar(g_file);
				if (cint == EOF) break; // Er, but feof is false? Just making sure..

				char c = (char)cint;
				char toAppend = c;

				if (c == '"')
				{
					//well, we're done
					if (!currentToken)
						currentToken = StrDuplicate("");
					TokenAdd(TK_STRING, currentToken, g_lineNum);
					currentToken = NULL;
					currentTokenSize = 0;
					break;
				}
				if (c == '\\')
				{
					// Allow escaping some characters. Read another.
					int cint2 = FileGetChar(g_file);
					if (cint2 == EOF)
						TokenOnError(ERROR_UNTERMINATED_ESC_SEQ);

					char c2 = (char)cint2;
					switch (c2)
					{
						case 'n': toAppend = '\n'; break;
						case 'e': toAppend = '\033'; break;
						case 't': toAppend = '\t'; break;
						case 'b': toAppend = '\b'; break;
						case '"': toAppend = '"'; break;
						default:
						{
							if (c2 >= '1' && c2 <= '9')
							{
								toAppend = c2 - '1';
								break;
							}

							TokenOnError(ERROR_UNKNOWN_ESC_SEQ);
							break;
						}
					}
				}

				// Append the 'toAppend' character.
				TokenAppend(&currentToken, &currentTokenSize, toAppend);
			}
			continue;
		}

		TokenAppend(&currentToken, &currentTokenSize, c);
	}
}

void TokenDump()
{
	for (size_t i = 0; i < ntokens; i++)
	{
		LogMsg("TOKEN: '%s'", tokens[i]);
	}
}

void TokenTeardown()
{
	for (size_t i = 0; i < ntokens; i++)
	{
		MemFree(tokens[i]->m_data);
		MemFree(tokens[i]);
	}
	MemFree(tokens);
	tokens = NULL;
	ntokens = 0;
}

