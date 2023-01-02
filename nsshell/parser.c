#include "shell.h"
#include "parser.h"
#include "tokeniser.h"

extern jmp_buf g_errorJumpBuffer;

NORETURN void ParserOnError(int error)
{
	longjmp(g_errorJumpBuffer, error);
}

Statement* ParseBlockStatement();
Statement* ParseGenericStatement();
Statement* ParseStringStatement();

#define IS(token, type) (token->m_type == type)

extern Token** tokens;
extern size_t ntokens;

Statement * g_mainBlock;

size_t g_currentToken;

bool ParserEndOfFile()
{
	return g_currentToken >= ntokens;
}

Token* PeekToken()
{
	if (ParserEndOfFile()) return NULL;
	return tokens[g_currentToken];
}

Token* ConsumeToken()
{
	if (ParserEndOfFile()) return NULL;
	return tokens[g_currentToken++];
}

Statement* ParserSetupBlockStatement()
{
	Statement* pStmt = MemCAllocate(1, sizeof(Statement));
	if (!pStmt) ParserOnError(ERROR_P_MEMORY_ALLOC_FAILURE);
	
	pStmt->type = STMT_BLOCK;

	pStmt->m_blk_data = MemCAllocate(1, sizeof(StatementBlkData));
	if (!pStmt->m_blk_data) ParserOnError(ERROR_P_MEMORY_ALLOC_FAILURE);

	pStmt->m_blk_data->m_nstatements = 0;
	pStmt->m_blk_data->m_statements  = NULL;

	return pStmt;
}

Statement* ParserSetupCommandStatement()
{
	Statement* pStmt = MemCAllocate(1, sizeof(Statement));
	if (!pStmt) ParserOnError(ERROR_P_MEMORY_ALLOC_FAILURE);

	pStmt->type = STMT_COMMAND;

	pStmt->m_cmd_data = MemCAllocate(1, sizeof(StatementCmdData));
	if (!pStmt->m_cmd_data) ParserOnError(ERROR_P_MEMORY_ALLOC_FAILURE);

	pStmt->m_cmd_data->m_name = NULL;
	pStmt->m_cmd_data->m_args = NULL;
	pStmt->m_cmd_data->m_nargs = 0;

	return pStmt;
}

Statement* ParserSetupIfStatement()
{
	Statement* pStmt = MemCAllocate(1, sizeof(Statement));
	if (!pStmt) ParserOnError(ERROR_P_MEMORY_ALLOC_FAILURE);

	pStmt->type = STMT_IF;

	pStmt->m_if_data = MemCAllocate(1, sizeof(StatementIfData));
	if (!pStmt->m_if_data) ParserOnError(ERROR_P_MEMORY_ALLOC_FAILURE);

	pStmt->m_if_data->m_condition = NULL;
	pStmt->m_if_data->m_true_part = NULL;
	pStmt->m_if_data->m_false_part = NULL;

	return pStmt;
}

Statement* ParserSetupStringStatement()
{
	Statement* pStmt = MemCAllocate(1, sizeof(Statement));
	if (!pStmt) ParserOnError(ERROR_P_MEMORY_ALLOC_FAILURE);

	pStmt->type = STMT_STRING;

	pStmt->m_str_data = MemCAllocate(1, sizeof(StatementStrData));
	if (!pStmt->m_str_data) ParserOnError(ERROR_P_MEMORY_ALLOC_FAILURE);

	pStmt->m_str_data->m_str = "";

	return pStmt;
}

Statement* ParserSetupFunctionStatement()
{
	Statement* pStmt = MemCAllocate(1, sizeof(Statement));
	if (!pStmt) ParserOnError(ERROR_P_MEMORY_ALLOC_FAILURE);

	pStmt->type = STMT_FUNCTION;

	pStmt->m_fun_data = MemCAllocate(1, sizeof(StatementFunData));
	if (!pStmt->m_fun_data) ParserOnError(ERROR_P_MEMORY_ALLOC_FAILURE);

	pStmt->m_fun_data->m_name      = "";
	pStmt->m_fun_data->m_statement = NULL;
	pStmt->m_fun_data->m_args      = NULL;
	pStmt->m_fun_data->m_nargs     = 0;

	return pStmt;
}

Statement* ParserSetupVariableStatement()
{
	Statement* pStmt = MemCAllocate(1, sizeof(Statement));
	if (!pStmt) ParserOnError(ERROR_P_MEMORY_ALLOC_FAILURE);

	pStmt->type = STMT_VARIABLE;

	pStmt->m_var_data = MemCAllocate(1, sizeof(StatementFunData));
	if (!pStmt->m_var_data) ParserOnError(ERROR_P_MEMORY_ALLOC_FAILURE);

	pStmt->m_var_data->m_name      = "";
	pStmt->m_var_data->m_statement = NULL;

	return pStmt;
}

void ParserAddStmtToBlockStmt(Statement* pBlockStmt, Statement* pAddedStmt)
{
	if (pBlockStmt->type != STMT_BLOCK) ParserOnError(ERROR_INTERNAL_NOT_A_BLOCK_STMT);

	// To the m_blk_data, add a statement.
	Statement** stmts = (Statement**)MemReAllocate(pBlockStmt->m_blk_data->m_statements, (pBlockStmt->m_blk_data->m_nstatements + 1) * sizeof(Statement*));
	if (!stmts) ParserOnError(ERROR_P_MEMORY_ALLOC_FAILURE);

	pBlockStmt->m_blk_data->m_statements = stmts;
	pBlockStmt->m_blk_data->m_statements[pBlockStmt->m_blk_data->m_nstatements++] = pAddedStmt;
}

void ParserAddArgToCmdStmt(Statement* pCmdStmt, Statement* arg)
{
	if (pCmdStmt->type != STMT_COMMAND) ParserOnError(ERROR_INTERNAL_NOT_A_COMMAND_STMT);

	// To the m_blk_data, add a statement.
	Statement** args = (Statement**)MemReAllocate(pCmdStmt->m_cmd_data->m_args, (pCmdStmt->m_cmd_data->m_nargs + 1) * sizeof(Statement*));
	if (!args) ParserOnError(ERROR_P_MEMORY_ALLOC_FAILURE);

	pCmdStmt->m_cmd_data->m_args = args;
	pCmdStmt->m_cmd_data->m_args[pCmdStmt->m_cmd_data->m_nargs++] = arg;
}

void ParserAddArgToFunStmt(Statement* pFunStmt, char* arg)
{
	if (pFunStmt->type != STMT_FUNCTION) ParserOnError(ERROR_INTERNAL_NOT_A_FUNCTION_STMT);

	// To the m_blk_data, add a statement.
	char** args = (char**)MemReAllocate(pFunStmt->m_fun_data->m_args, (pFunStmt->m_fun_data->m_nargs + 1) * sizeof(char*));
	if (!args) ParserOnError(ERROR_P_MEMORY_ALLOC_FAILURE);

	pFunStmt->m_fun_data->m_args = args;
	pFunStmt->m_fun_data->m_args[pFunStmt->m_fun_data->m_nargs++] = arg;
}

Statement* ParseCommandStatementInside(bool bCanExpectSemicolon)
{
	Token* token = PeekToken();
	if (token == NULL)
	{
		ParserOnError(ERROR_EXPECTED_STATEMENT);
		return NULL;
	}

	if (IS(token, TK_STRING))
		return ParseStringStatement();

	ConsumeToken();

	Statement* pCmdStmt = ParserSetupCommandStatement();

	pCmdStmt->m_cmd_data->m_name = StrDuplicate(token->m_data);

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
		}

		return pCmdStmt;
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
	if (!PeekToken()) ParserOnError(ERROR_EXPECTED_COMMAND_STATEMENT);

	Statement* pStmt = ParseCommandStatementInside(true);

	Token* token = ConsumeToken();
	if (!token) ParserOnError(ERROR_EXPECTED_SEMICOLON);
	if (!IS(token, TK_SEMICOLON)) ParserOnError(ERROR_EXPECTED_SEMICOLON);

	return pStmt;
}

Statement* ParseIfStatement()
{
	if (!PeekToken()) ParserOnError(ERROR_EXPECTED_IF_STATEMENT);
	if (!IS(PeekToken(), TK_IF)) ParserOnError(ERROR_EXPECTED_IF_STATEMENT);

	ConsumeToken();

	Statement* pIfStmt = ParserSetupIfStatement();

	// get the condition
	pIfStmt->m_if_data->m_condition = ParseCommandStatementInside(false);

	Token* token;

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
	if (!PeekToken()) ParserOnError(ERROR_EXPECTED_WHILE_STATEMENT);
	if (!IS(PeekToken(), TK_WHILE)) ParserOnError(ERROR_EXPECTED_WHILE_STATEMENT);

	ConsumeToken();

	Statement* pIfStmt = ParserSetupIfStatement();
	pIfStmt->type = STMT_WHILE;

	// get the condition
	pIfStmt->m_if_data->m_condition = ParseCommandStatementInside(false);

	Token* token;

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
	if (!PeekToken()) ParserOnError(ERROR_EXPECTED_EMPTY_STATEMENT);
	if (!IS(PeekToken(), TK_SEMICOLON)) ParserOnError(ERROR_EXPECTED_SEMICOLON);

	ConsumeToken();
	
	Statement* pStmt = MemCAllocate(1, sizeof(Statement));
	if (!pStmt)
	{
		ParserOnError(ERROR_P_MEMORY_ALLOC_FAILURE);
		return;
	}

	pStmt->type = STMT_NULL;

	return pStmt;
}

Statement* ParseStringStatement()
{
	if (!PeekToken()) ParserOnError(ERROR_EXPECTED_STRING_STATEMENT);
	if (!IS(PeekToken(), TK_STRING)) ParserOnError(ERROR_EXPECTED_STRING_STATEMENT);

	Statement* pStmt = ParserSetupStringStatement();

	if (!PeekToken())
	{
		ParserOnError(ERROR_EXPECTED_STRING);
	}

	pStmt->m_str_data->m_str = StrDuplicate(ConsumeToken()->m_data);

	return pStmt;
}

Statement* ParseFunctionStatement()
{
	if (!PeekToken()) ParserOnError(ERROR_EXPECTED_FUNCTION_STATEMENT);
	if (!IS(PeekToken(), TK_FUNCTION) && !IS(PeekToken(), TK_FUNCTION_SHORT)) ParserOnError(ERROR_EXPECTED_FUNCTION_STATEMENT);

	ConsumeToken(); // consume the 'function' word

	Token* token;

	token = PeekToken();
	if (!token) ParserOnError(ERROR_EXPECTED_FUNCTION_NAME);
	if (!IS(token, TK_KEYWORD_START)) ParserOnError(ERROR_EXPECTED_FUNCTION_NAME);

	Statement* pFunction = ParserSetupFunctionStatement();
	pFunction->m_fun_data->m_name = StrDuplicate(token->m_data);

	// Consume the name token.
	ConsumeToken();

	// Peek into the next token.
	token = PeekToken();

	// Check if this is the start of an argument list.
	if (IS(token, TK_OPENPAREN))
	{
		ConsumeToken();

		while (true)
		{
			token = PeekToken();

			if (!token) ParserOnError(ERROR_UNTERMINATED_FUNCTION_DECL);
			if (!IS(token, TK_KEYWORD_START)) ParserOnError(ERROR_EXPECTED_ARGUMENTS);

			ParserAddArgToFunStmt(pFunction, StrDuplicate(token->m_data));

			ConsumeToken();

			// The next token should be a comma.
			token = PeekToken();
			if (!token) ParserOnError(ERROR_UNTERMINATED_FUNCTION_DECL);
			if (!IS(token, TK_COMMA))
			{
				if (IS(token, TK_CLOSEPAREN))
					break;

				ParserOnError(ERROR_EXPECTED_COMMA);
			}

			ConsumeToken();
		}

		ConsumeToken();
	}

	// Now get the function body.
	pFunction->m_fun_data->m_statement = ParseGenericStatement();

	return pFunction;
}

Statement* ParseLetStatement()
{
	Token* tk = PeekToken();
	if (!tk) ParserOnError(ERROR_EXPECTED_LET_STATEMENT);
	if (!IS(tk, TK_LET) && !IS(tk, TK_VAR)) ParserOnError(ERROR_EXPECTED_LET_STATEMENT);

	ConsumeToken();

	tk = PeekToken();
	if (!IS(tk, TK_KEYWORD_START)) ParserOnError(ERROR_EXPECTED_VARIABLE_NAME);

	ConsumeToken();

	Statement* pVarStmt = ParserSetupVariableStatement();

	pVarStmt->m_var_data->m_name = StrDuplicate(tk->m_data);

	// Check the next token. If it's an equals sign, that means afterwards there will be a new statement.
	tk = PeekToken();

	if (IS(tk, TK_EQUALS))
	{
		ConsumeToken();

		// okay, now parse a statement
		pVarStmt->m_var_data->m_statement = ParseGenericStatement();

		// hack for now: If this is a STMT_COMMAND statement, it already ate the semicolon
		if (pVarStmt->m_var_data->m_statement->type == STMT_COMMAND)
			return pVarStmt;
	}

	// Ensure this declaration is finished off with a semicolon
	tk = PeekToken();
	if (!IS(tk, TK_SEMICOLON))
	{
		ParserOnError(ERROR_EXPECTED_SEMICOLON);
	}

	return pVarStmt;
}

Statement* ParseGenericStatement()
{
	if (!PeekToken()) ParserOnError(ERROR_EXPECTED_STATEMENT);

	Token* tk = PeekToken();

	Statement* pStmt = NULL;

	if (IS(tk, TK_SEMICOLON))
		pStmt = ParseEmptyStatement();
	else if (IS(tk, TK_OPENBLOCK))
		pStmt = ParseBlockStatement();
	else if (IS(tk, TK_IF))
		pStmt = ParseIfStatement();
	else if (IS(tk, TK_WHILE))
		pStmt = ParseWhileStatement();
	else if (IS(tk, TK_STRING))
		pStmt = ParseStringStatement();
	else if (IS(tk, TK_FUNCTION) || IS(tk, TK_FUNCTION_SHORT))
		pStmt = ParseFunctionStatement();
	else if (IS(tk, TK_LET) || IS(tk, TK_VAR))
		pStmt = ParseLetStatement();
	else
		pStmt = ParseCommandStatement(false);

	return pStmt;
}

void ParseBlockStatementInside(Statement* pBlockStmt)
{
	Token* tk = PeekToken();

	while (tk && !IS(tk, TK_CLOSEBLOCK))
	{
		ParserAddStmtToBlockStmt(pBlockStmt, ParseGenericStatement());

		tk = PeekToken();
	}
}

Statement* ParseBlockStatement()
{
	if (!PeekToken()) ParserOnError(ERROR_EXPECTED_BLOCK_STATEMENT);
	if (!IS(PeekToken(), TK_OPENBLOCK)) ParserOnError(ERROR_EXPECTED_BLOCK_STATEMENT);

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
	"STMT_STRING",
	"STMT_FUNCTION",
	"STMT_VARIABLE",
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
		case STMT_STRING:
		{
			LogMsg("   Contents: '%s'", pStmt->m_str_data->m_str);

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
		case STMT_FUNCTION:
		{
			LogMsgNoCr("  Name: %s Arg:", pStmt->m_fun_data->m_name);
			for (int i = 0; i < pStmt->m_fun_data->m_nargs; i++)
				LogMsgNoCr("%s%s", i == 0 ? "" : ",", pStmt->m_fun_data->m_args[i]);
			LogMsg("");

			PadLineTo(padding); LogMsg("Function body:");
			ParserDumpStatement(pStmt->m_fun_data->m_statement, padding + 4);
		}
		default:
		{
			LogMsg("");
			break;
		}
	}
}

void ParserFreeStatement(Statement* pStatement)
{
	if (!pStatement) return;

	switch (pStatement->type)
	{
		case STMT_NULL: break;
		case STMT_STRING:
		{
			MemFree(pStatement->m_str_data->m_str);
			break;
		}
		case STMT_COMMAND:
		{
			MemFree(pStatement->m_cmd_data->m_name);

			for (size_t i = 0; i < pStatement->m_cmd_data->m_nargs; i++)
			{
				ParserFreeStatement(pStatement->m_cmd_data->m_args[i]);
			}

			MemFree(pStatement->m_cmd_data->m_args);
			break;
		}
		case STMT_BLOCK:
		{
			for (size_t i = 0; i < pStatement->m_blk_data->m_nstatements; i++)
			{
				ParserFreeStatement(pStatement->m_blk_data->m_statements[i]);
			}

			MemFree(pStatement->m_blk_data->m_statements);
			break;
		}
		case STMT_FUNCTION:
		{
			for (size_t i = 0; i < pStatement->m_fun_data->m_nargs; i++)
			{
				MemFree(pStatement->m_fun_data->m_args[i]);
			}

			ParserFreeStatement(pStatement->m_fun_data->m_statement);
			MemFree(pStatement->m_fun_data->m_name);
			break;
		}
		case STMT_VARIABLE:
		{
			ParserFreeStatement(pStatement->m_var_data->m_statement);
			MemFree(pStatement->m_var_data->m_name);
			break;
		}
		case STMT_IF:
		case STMT_WHILE:
		{
			ParserFreeStatement(pStatement->m_if_data->m_condition);
			ParserFreeStatement(pStatement->m_if_data->m_true_part);
			ParserFreeStatement(pStatement->m_if_data->m_false_part);
			break;
		}
	}

	MemFree(pStatement->m_data);
	MemFree(pStatement);
}

void Parse()
{
	g_currentToken = 0;

	g_mainBlock = ParserSetupBlockStatement();

	ParseBlockStatementInside(g_mainBlock);

	// dump the main block
	//ParserDumpStatement(g_mainBlock, 0);
}

void ParserTeardown()
{
	ParserFreeStatement(g_mainBlock);

	g_mainBlock = NULL;
}
