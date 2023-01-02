#include "nanoshell.h"
#include "tokeniser.h"
#include "shell.h"

extern jmp_buf g_errorJumpBuffer;
extern FILE* g_file;

const char * g_singleSymbolTokens = "!@#$%^&*();:,.+_-={}[]|\\";

char** tokens = NULL;
size_t ntokens = 0;

NORETURN void TokenOnError(int error)
{
	longjmp(g_errorJumpBuffer, error);
}

// note: this could be done better
void TokenAppend(char** token, size_t* sz, char chr)
{
	if (*sz == 0)
	{
		*sz = 1;
	}

	char* newToken = realloc(*token, (*sz) + 1);

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

// same with this. TODO reduce the number of reallocs for TokenAdd and TokenAppend

// note: this must take a malloc'ed buffer
void TokenAdd(char* token)
{
	if (!token) return;

	char** newTokens = realloc(tokens, sizeof(char*) * (ntokens + 1));
	if (!newTokens)
	{
		TokenOnError(ERROR_MEMORY_ALLOC_FAILURE);
		return;
	}

	tokens = newTokens;

	tokens[ntokens++] = token;
}

void Tokenise()
{
	char* currentToken = NULL;
	size_t currentTokenSize = 0;

	while (!feof(g_file))
	{
		int cint = fgetc(g_file);
		if (cint == EOF) break; // Er, but feof is false? Just making sure..

		char c = (char)cint;

		// if this is white space
		if (isspace(c) || iscntrl(c))
		{
			// push the current token, if there is one
			TokenAdd(currentToken);
			currentToken = NULL;
			currentTokenSize = 0;
			continue;
		}

		// if it is one of these symbols..
		char* match = strchr(g_singleSymbolTokens, c);
		if (match != NULL)
		{
			// push the current token, if there is one
			TokenAdd(currentToken);
			currentToken = NULL;
			currentTokenSize = 0;

			// add a NEW token, this being the single symbol
			currentToken = malloc(2);
			currentToken[0] = c;
			currentToken[1] = 0;
			TokenAdd(currentToken);
			currentToken = NULL;
			continue;
		}

		// if it's a quotation mark
		if (c == '"')
		{
			// push the current token, if there is one
			TokenAdd(currentToken);
			currentToken = NULL;
			currentTokenSize = 0;

			// while we have characters
			while (!feof(g_file))
			{
				int cint = fgetc(g_file);
				if (cint == EOF) break; // Er, but feof is false? Just making sure..

				char c = (char)cint;
				char toAppend = c;

				if (c == '"')
				{
					//well, we're done
					TokenAdd(currentToken);
					currentToken = NULL;
					currentTokenSize = 0;
					break;
				}
				if (c == '\\')
				{
					// Allow escaping some characters. Read another.
					int cint2 = fgetc(g_file);
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

void TokensDump()
{
	for (size_t i = 0; i < ntokens; i++)
	{
		LogMsg("TOKEN: '%s'", tokens[i]);
	}
}
