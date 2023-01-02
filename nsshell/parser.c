#include "shell.h"
#include "parser.h"
#include "tokeniser.h"

extern jmp_buf g_errorJumpBuffer;

void ParserOnError(int error)
{
	longjmp(g_errorJumpBuffer, error);
}

const char* const gCommonTokens[] =
{
	"",
	";",
	"{",
	"}",
	"if",
	"then",
};

enum eCommonToken
{
	TK_NOT_COMMON,
	TK_SEMICOLON,
	TK_OPENBLOCK,
	TK_CLOSEBLOCK,
	TK_IF,
	TK_THEN,
	TK_END,
};

int GetCommonTokenType(const char* token)
{
	for (int i = TK_NOT_COMMON + 1; i < TK_END; i++)
	{
		if (strcmp(token, gCommonTokens[i]) == 0)
			return i;
	}

	return TK_NOT_COMMON;
}

#define IS(token, type) (GetCommonTokenType(token) == type)

extern char** tokens;
extern size_t ntokens;

Statement * g_mainBlock;

size_t g_currentToken;

bool ParserEndOfFile()
{
	return g_currentToken >= ntokens;
}

char* PeekToken()
{
	if (ParserEndOfFile()) return NULL;
	return tokens[g_currentToken];
}

char* ConsumeToken()
{
	if (ParserEndOfFile()) return NULL;
	return tokens[g_currentToken++];
}

Statement* ParserSetupBlockStatement()
{
	Statement* pStmt = calloc(1, sizeof(Statement));
	if (!pStmt)
	{
		ParserOnError(ERROR_P_MEMORY_ALLOC_FAILURE);
		return;
	}
	pStmt->type = STMT_BLOCK;

	pStmt->m_blk_data = calloc(1, sizeof(StatementBlkData));
	if (!pStmt->m_blk_data)
	{
		ParserOnError(ERROR_P_MEMORY_ALLOC_FAILURE);
		return;
	}

	pStmt->m_blk_data->m_nstatements = 0;
	pStmt->m_blk_data->m_statements  = NULL;

	return pStmt;
}

void ParserAddStmtToBlockStmt(Statement* pBlockStmt, Statement* pAddedStmt)
{
	if (pBlockStmt->type != STMT_BLOCK)
	{
		ParserOnError(ERROR_INTERNAL_NOT_A_BLOCK_STMT);
		return;
	}

	// To the m_blk_data, add a statement.
	Statement** stmts = (Statement**)realloc(pBlockStmt->m_blk_data->m_statements, (pBlockStmt->m_blk_data->m_nstatements + 1) * sizeof(Statement*));
	if (!stmts)
	{
		ParserOnError(ERROR_P_MEMORY_ALLOC_FAILURE);
		return;
	}

	pBlockStmt->m_blk_data->m_statements = stmts;

	pBlockStmt->m_blk_data->m_statements[pBlockStmt->m_blk_data->m_nstatements++] = pAddedStmt;
}

Statement* ParseIfStatement()
{
	return NULL;
}

Statement* ParseCommandStatement()
{
	return NULL;
}

Statement* ParseEmptyStatement()
{
	ConsumeToken();
	
	Statement* pStmt = calloc(1, sizeof(Statement));
	if (!pStmt)
	{
		ParserOnError(ERROR_P_MEMORY_ALLOC_FAILURE);
		return;
	}

	pStmt->type = STMT_NULL;

	return pStmt;
}

Statement* ParseBlockStatement();

void ParseBlockStatementInside(Statement* pBlockStmt)
{
	char* tk = PeekToken();

	while (tk && !IS(tk, TK_CLOSEBLOCK))
	{
		if (IS(tk, TK_SEMICOLON))
		{
			// This is the 'null' statement. Do nothing, and consume it.
			ParserAddStmtToBlockStmt(pBlockStmt, ParseEmptyStatement());
		}
		else if (IS(tk, TK_OPENBLOCK))
		{
			// This is an 'if' statement.
			ParserAddStmtToBlockStmt(pBlockStmt, ParseBlockStatement());
		}
		else if (IS(tk, TK_IF))
		{
			// This is an 'if' statement.
			ParserAddStmtToBlockStmt(pBlockStmt, ParseIfStatement());
		}
		else
		{
			ParserAddStmtToBlockStmt(pBlockStmt, ParseCommandStatement());
		}

		tk = PeekToken();
	}
}

Statement* ParseBlockStatement()
{
	Statement* subBlockStmt = ParserSetupBlockStatement();

	ConsumeToken();
	
	ParseBlockStatementInside(subBlockStmt);
	
	char* tkn = ConsumeToken();
	if (tkn == NULL)
	{
		ParserOnError(ERROR_UNTERMINATED_BLOCK_STMT);
	}

	return subBlockStmt;
}

static void PadLineTo(int padding)
{
	char paddingStr[42];
	snprintf(paddingStr, sizeof paddingStr, "%%%ds", padding);
	LogMsgNoCr(paddingStr, "");
}

static const char* const g_ts[] =
{
	"STMT_NULL",
	"STMT_COMMAND",
	"STMT_BLOCK",
	"STMT_IF",
};

const char* GetTypeString(eStatementType type)
{
	return g_ts[type];
}

void ParserDumpStatement(Statement* pStmt, int padding)
{
	PadLineTo(padding);

	LogMsg("Statement %p. Type %s", pStmt, GetTypeString(pStmt->type));

	switch (pStmt->type)
	{
		case STMT_BLOCK:
		{
			for (int i = 0; i < pStmt->m_blk_data->m_nstatements; i++)
			{
				Statement* pStmtSub = pStmt->m_blk_data->m_statements[i];

				ParserDumpStatement(pStmtSub, padding + 4);
			}
			break;
		}
	}
}

void Parse()
{
	g_currentToken = 0;

	g_mainBlock = ParserSetupBlockStatement();

	ParseBlockStatementInside(g_mainBlock);


	// dump the main block
	ParserDumpStatement(g_mainBlock, 0);
}
