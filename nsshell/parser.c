#include "shell.h"
#include "parser.h"
#include "tokeniser.h"

extern jmp_buf g_errorJumpBuffer;

NORETURN void ParserOnError(int error)
{
	longjmp(g_errorJumpBuffer, error);
}

const char* const gCommonTokens[] =
{
	"",
	";",
	"{",
	"}",
	"(",
	")",
	",",
	"if",
	"then",
	"else",
	"while",
	"do",
	"finally",
};

enum eCommonToken
{
	TK_NOT_COMMON,
	TK_SEMICOLON,
	TK_OPENBLOCK,
	TK_CLOSEBLOCK,
	TK_OPENPAREN,
	TK_CLOSEPAREN,
	TK_COMMA,
	TK_IF,
	TK_THEN,
	TK_ELSE,
	TK_WHILE,
	TK_DO,
	TK_FINALLY,
	TK_END,
};

Statement* ParseBlockStatement();
Statement* ParseGenericStatement();

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
	if (!pStmt) ParserOnError(ERROR_P_MEMORY_ALLOC_FAILURE);
	
	pStmt->type = STMT_BLOCK;

	pStmt->m_blk_data = calloc(1, sizeof(StatementBlkData));
	if (!pStmt->m_blk_data) ParserOnError(ERROR_P_MEMORY_ALLOC_FAILURE);

	pStmt->m_blk_data->m_nstatements = 0;
	pStmt->m_blk_data->m_statements  = NULL;

	return pStmt;
}

Statement* ParserSetupCommandStatement()
{
	Statement* pStmt = calloc(1, sizeof(Statement));
	if (!pStmt) ParserOnError(ERROR_P_MEMORY_ALLOC_FAILURE);

	pStmt->type = STMT_COMMAND;

	pStmt->m_cmd_data = calloc(1, sizeof(StatementCmdData));
	if (!pStmt->m_cmd_data) ParserOnError(ERROR_P_MEMORY_ALLOC_FAILURE);

	pStmt->m_cmd_data->m_name = NULL;
	pStmt->m_cmd_data->m_args = NULL;
	pStmt->m_cmd_data->m_nargs = 0;

	return pStmt;
}

Statement* ParserSetupIfStatement()
{
	Statement* pStmt = calloc(1, sizeof(Statement));
	if (!pStmt) ParserOnError(ERROR_P_MEMORY_ALLOC_FAILURE);

	pStmt->type = STMT_IF;

	pStmt->m_if_data = calloc(1, sizeof(StatementIfData));
	if (!pStmt->m_if_data) ParserOnError(ERROR_P_MEMORY_ALLOC_FAILURE);

	pStmt->m_if_data->m_condition = NULL;
	pStmt->m_if_data->m_true_part = NULL;
	pStmt->m_if_data->m_false_part = NULL;

	return pStmt;
}

void ParserAddStmtToBlockStmt(Statement* pBlockStmt, Statement* pAddedStmt)
{
	if (pBlockStmt->type != STMT_BLOCK) ParserOnError(ERROR_INTERNAL_NOT_A_BLOCK_STMT);

	// To the m_blk_data, add a statement.
	Statement** stmts = (Statement**)realloc(pBlockStmt->m_blk_data->m_statements, (pBlockStmt->m_blk_data->m_nstatements + 1) * sizeof(Statement*));
	if (!stmts) ParserOnError(ERROR_P_MEMORY_ALLOC_FAILURE);

	pBlockStmt->m_blk_data->m_statements = stmts;
	pBlockStmt->m_blk_data->m_statements[pBlockStmt->m_blk_data->m_nstatements++] = pAddedStmt;
}

void ParserAddArgToCmdStmt(Statement* pCmdStmt, Statement* arg)
{
	if (pCmdStmt->type != STMT_COMMAND) ParserOnError(ERROR_INTERNAL_NOT_A_COMMAND_STMT);

	// To the m_blk_data, add a statement.
	Statement** args = (Statement**)realloc(pCmdStmt->m_cmd_data->m_args, (pCmdStmt->m_cmd_data->m_nargs + 1) * sizeof(Statement*));
	if (!args) ParserOnError(ERROR_P_MEMORY_ALLOC_FAILURE);

	pCmdStmt->m_cmd_data->m_args = args;
	pCmdStmt->m_cmd_data->m_args[pCmdStmt->m_cmd_data->m_nargs++] = arg;
}

Statement* ParseCommandStatementInside(bool bCanExpectSemicolon)
{
	char* token = ConsumeToken();
	if (token == NULL)
	{
		ParserOnError(ERROR_EXPECTED_STATEMENT);
		return NULL;
	}

	Statement* pCmdStmt = ParserSetupCommandStatement();

	pCmdStmt->m_cmd_data->m_name = token;

	token = PeekToken();

	if (!token) ParserOnError(ERROR_EXPECTED_SEMICOLON_OR_ARGUMENTS);

	if (!IS(token, TK_OPENPAREN))
	{
		// If we're starting to close up..
		if (!IS(token, TK_CLOSEPAREN))
		{
			if (IS(token, TK_COMMA))
			{
				if (bCanExpectSemicolon)
					ParserOnError(ERROR_EXPECTED_CLOSE_PAREN_OR_ARGUMENTS);
				else
					return pCmdStmt;
			}
			
			if (IS(token, TK_SEMICOLON))
			{
				if (!bCanExpectSemicolon)
					ParserOnError(ERROR_EXPECTED_CLOSE_PAREN_OR_ARGUMENTS);
				else
					return pCmdStmt;
			}

			return pCmdStmt;
		}
		else
		{
			return pCmdStmt;
		}
	}
	else
	{
		ConsumeToken();

		token = PeekToken();

		if (IS(token, TK_CLOSEPAREN))
		{
			ConsumeToken();
			return pCmdStmt;
		}

		// Parse the arguments
		while (true)
		{
			token = PeekToken();

			if (!token)
				ParserOnError(ERROR_UNTERMINATED_COMMAND_STMT);

			Statement* pSubArg = ParseCommandStatementInside(false);

			ParserAddArgToCmdStmt(pCmdStmt, pSubArg);

			// check if we have a comma
			token = PeekToken();
			if (IS(token, TK_COMMA))
			{
				ConsumeToken();
			}
			else if (IS(token, TK_CLOSEPAREN))
			{
				ConsumeToken();
				break;
			}
		}
	}

	return pCmdStmt;
}

Statement* ParseCommandStatement()
{
	Statement* pStmt = ParseCommandStatementInside(true);

	char* token = ConsumeToken();
	if (!token) ParserOnError(ERROR_EXPECTED_SEMICOLON);
	if (!IS(token, TK_SEMICOLON)) ParserOnError(ERROR_EXPECTED_SEMICOLON);

	return pStmt;
}

Statement* ParseIfStatement()
{
	ConsumeToken();

	Statement* pIfStmt = ParserSetupIfStatement();

	// get the condition
	pIfStmt->m_if_data->m_condition = ParseCommandStatementInside(false);

	char* token;

	token = PeekToken();
	if (!IS(token, TK_THEN))
	{
		ParserOnError(ERROR_EXPECTED_THEN);
	}

	ConsumeToken();

	pIfStmt->m_if_data->m_true_part = ParseGenericStatement();

	token = PeekToken();
	if (!IS(token, TK_ELSE))
	{
		return pIfStmt;
	}

	ConsumeToken();

	pIfStmt->m_if_data->m_false_part = ParseGenericStatement();

	return pIfStmt;
}

Statement* ParseWhileStatement()
{
	ConsumeToken();

	Statement* pIfStmt = ParserSetupIfStatement();
	pIfStmt->type = STMT_WHILE;

	// get the condition
	pIfStmt->m_if_data->m_condition = ParseCommandStatementInside(false);

	char* token;

	token = PeekToken();
	if (!IS(token, TK_DO))
	{
		ParserOnError(ERROR_EXPECTED_DO);
	}

	ConsumeToken();

	pIfStmt->m_if_data->m_true_part = ParseGenericStatement();

	token = PeekToken();
	if (!IS(token, TK_FINALLY))
	{
		return pIfStmt;
	}

	ConsumeToken();

	pIfStmt->m_if_data->m_false_part = ParseGenericStatement();

	return pIfStmt;
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

Statement* ParseGenericStatement()
{
	char* tk = PeekToken();

	Statement* pStmt = NULL;

	if (IS(tk, TK_SEMICOLON))
		pStmt = ParseEmptyStatement();
	else if (IS(tk, TK_OPENBLOCK))
		pStmt = ParseBlockStatement();
	else if (IS(tk, TK_IF))
		pStmt = ParseIfStatement();
	else if (IS(tk, TK_WHILE))
		pStmt = ParseWhileStatement();
	else
		pStmt = ParseCommandStatement(false);

	return pStmt;
}

void ParseBlockStatementInside(Statement* pBlockStmt)
{
	char* tk = PeekToken();

	while (tk && !IS(tk, TK_CLOSEBLOCK))
	{
		ParserAddStmtToBlockStmt(pBlockStmt, ParseGenericStatement());

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
	"STMT_WHILE",
};

const char* GetTypeString(eStatementType type)
{
	return g_ts[type];
}

void ParserDumpStatement(Statement* pStmt, int padding)
{
	PadLineTo(padding);

	if (!pStmt)
	{
		LogMsg("NULL statement");
		return;
	}

	LogMsgNoCr("Statement %p. Type %s", pStmt, GetTypeString(pStmt->type));

	switch (pStmt->type)
	{
		case STMT_BLOCK:
		{
			LogMsg("");
			for (size_t i = 0; i < pStmt->m_blk_data->m_nstatements; i++)
			{
				Statement* pStmtSub = pStmt->m_blk_data->m_statements[i];

				ParserDumpStatement(pStmtSub, padding + 4);
			}
			break;
		}
		case STMT_COMMAND:
		{
			LogMsg("  Name: %s   Arguments:", pStmt->m_cmd_data->m_name);

			for (size_t i = 0; i < pStmt->m_cmd_data->m_nargs; i++)
			{
				Statement* pStmtSub = pStmt->m_cmd_data->m_args[i];

				ParserDumpStatement(pStmtSub, padding + 4);
			}
			break;
		}
		case STMT_IF:
		case STMT_WHILE:
		{
			LogMsg("");
			PadLineTo(padding); LogMsg("Condition: ");
			ParserDumpStatement(pStmt->m_if_data->m_condition, padding + 4);

			PadLineTo(padding); LogMsg("True branch: ");
			ParserDumpStatement(pStmt->m_if_data->m_true_part, padding + 4);

			PadLineTo(padding); LogMsg("False branch: ");
			ParserDumpStatement(pStmt->m_if_data->m_false_part, padding + 4);
		}
		default:
		{
			LogMsg("");
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
