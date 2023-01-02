#include "shell.h"
#include "parser.h"
#include "runner.h"
#include "builtin.h"

extern jmp_buf g_errorJumpBuffer;

NORETURN void RunnerOnError(int error)
{
	longjmp(g_errorJumpBuffer, error);
}

// The runner is very simple - it looks through the statements and does decisions based on them.

// This could be optimized. By a lot.
Function* g_functionsList;

char* RunStatement(Statement* pStatement, int argc, char** argv);

void RunnerFreeFunction(Function * pFunc)
{
	if (pFunc->type == FUNCTION_VARIABLE)
	{
		MemFree(pFunc->m_pContents);
	}

	// if it's a pointer, we shouldn't free it, and if it's a statement, it's managed by the parser code

	MemFree(pFunc);
}

void RunnerRemoveFunctionFromList(Function* pFunc)
{
	if (pFunc == g_functionsList)
	{
		g_functionsList = pFunc->m_nlink;
		if (g_functionsList)
			g_functionsList->m_plink = NULL;
	}
	else
	{
		if (pFunc->m_nlink) pFunc->m_nlink->m_plink = pFunc->m_plink;
		if (pFunc->m_plink) pFunc->m_plink->m_nlink = pFunc->m_nlink;
	}

	RunnerFreeFunction(pFunc);
}

void RunnerCleanup()
{
	while (g_functionsList)
		RunnerRemoveFunctionFromList(g_functionsList);
}

void RunnerAddFunctionToList(Function* pFunc)
{
	if (g_functionsList == NULL)
	{
		g_functionsList = pFunc;
	}
	else
	{
		pFunc->m_nlink = g_functionsList;
		g_functionsList->m_plink = pFunc;
		g_functionsList = pFunc;
	}
}

void RunnerAddFunctionPtr(CallableFunPtr fp, const char* fname, int nargs, bool returns)
{
	if (nargs >= C_MAX_BUILTIN_ARGS)
	{
		LogMsg("ERROR: Too many arguments specified in this function. Not a great idea!");
	}

	Function* pFunc = MemCAllocate(1, sizeof (Function));
	if (!pFunc)
	{
		RunnerOnError(ERROR_R_MEMORY_ALLOC_FAILURE);
	}

	RunnerAddFunctionToList(pFunc);

	pFunc->type = FUNCTION_POINTER;
	pFunc->m_name     = fname;
	pFunc->m_bReturns = returns;
	pFunc->m_nArgs    = nargs;
	pFunc->m_pFunction = fp;
}

void RunnerAddFunctionStatement(Statement* statement, const char* fname, int nargs, bool returns)
{
	if (nargs >= C_MAX_ARGS)
	{
		RunnerOnError(ERROR_TOO_MANY_ARGS_DEF);
	}

	Function* pFunc = MemCAllocate(1, sizeof(Function));
	if (!pFunc)
	{
		RunnerOnError(ERROR_R_MEMORY_ALLOC_FAILURE);
	}

	RunnerAddFunctionToList(pFunc);

	pFunc->type = FUNCTION_STATEMENT;
	pFunc->m_name = fname;
	pFunc->m_bReturns = returns;
	pFunc->m_nArgs = nargs;
	pFunc->m_pStatement = statement;
}

void RunnerAddFunctionVariable(Statement* statement, const char* fname)
{
	Function* pFunc = MemCAllocate(1, sizeof(Function));
	if (!pFunc)
	{
		RunnerOnError(ERROR_R_MEMORY_ALLOC_FAILURE);
	}

	RunnerAddFunctionToList(pFunc);

	pFunc->type = FUNCTION_VARIABLE;
	pFunc->m_name = fname;
	pFunc->m_bReturns = true;
	pFunc->m_nArgs = 0;
	if (statement)
	{
		pFunc->m_pContents = RunStatement(statement, 0, NULL);
	}
	else
	{
		pFunc->m_pContents = StrDuplicate("");
	}
}

Function * RunnerLookUpFunction(const char * name)
{
	Function* fn = g_functionsList;
	while (fn)
	{
		if (strcmp(fn->m_name, name) == 0)
			return fn;

		fn = fn->m_nlink;
	}

	return NULL;
}


extern Statement* g_mainBlock;




char* RunStatement(Statement* pStatement, int argc, char** argv)
{
	// well, it depends on the type of statement
	switch (pStatement->type)
	{
		case STMT_NULL: return NULL;
		case STMT_BLOCK:
		{
			// For each statement within the block, run it with zero arguments.
			StatementBlkData* pData = pStatement->m_blk_data;

			for (size_t i = 0; i < pData->m_nstatements; i++)
			{
				char* returnValue = RunStatement(pData->m_statements[i], 0, NULL);

				// TempleOS style. If this statement was a simple string, just print it.
				if (pData->m_statements[i]->type == STMT_STRING)
				{
					LogMsg("%s", pData->m_statements[i]->m_str_data->m_str);
				}

				// if it returned something, we most likely won't use it. Free the memory.
				if (returnValue)
					MemFree(returnValue);
			}

			break;
		}
		case STMT_FUNCTION:
		{
			// Add the function to the list of known functions.
			StatementFunData* pData = pStatement->m_fun_data;

			Function* pPreExistingFunc = RunnerLookUpFunction(pData->m_name);
			if (pPreExistingFunc)
			{
				// If the function has the same body, it should be okay.
				if (pPreExistingFunc->m_pStatement != pData->m_statement)
				{
					RunnerOnError(ERROR_FUNCTION_ALREADY_EXISTS);
				}
			}

			RunnerAddFunctionStatement(pData->m_statement, pData->m_name, pData->m_nargs, true);

			break;
		}
		case STMT_VARIABLE:
		{
			StatementVarData* pData = pStatement->m_var_data;

			Function* pPreExistingFunc = RunnerLookUpFunction(pData->m_name);
			if (pPreExistingFunc)
				RunnerOnError(ERROR_VARIABLE_ALREADY_EXISTS);
			
			RunnerAddFunctionVariable(pData->m_statement, pData->m_name);

			break;
		}
		case STMT_ASSIGNMENT:
		{
			StatementVarData* pData = pStatement->m_var_data;

			Function* pPreExistingFunc = RunnerLookUpFunction(pData->m_name);

			//well, I'm going to be nice and allow this behavior
			if (!pPreExistingFunc)
			{
				RunnerAddFunctionVariable(pData->m_statement, pData->m_name);
				pPreExistingFunc = RunnerLookUpFunction(pData->m_name);
			}

			if (pPreExistingFunc->type != FUNCTION_VARIABLE)
			{
				RunnerOnError(ERROR_ASSIGNEE_IS_NOT_VARIABLE);
			}

			if (pPreExistingFunc->m_pContents)
				MemFree(pPreExistingFunc->m_pContents);

			pPreExistingFunc->m_pContents = RunStatement(pData->m_statement, 0, NULL);

			break;
		}
		case STMT_STRING:
		{
			return strdup(pStatement->m_str_data->m_str);
		}
		case STMT_COMMAND:
		{
			StatementCmdData* pData = pStatement->m_cmd_data;

			Function* pFunc = RunnerLookUpFunction(pData->m_name);
			if (!pFunc)
			{
				RunnerOnError(ERROR_UNKNOWN_FUNCTION);
			}

			if (pFunc->type == FUNCTION_VARIABLE)
			{
				if (pData->m_nargs != 0)
				{
					RunnerOnError(ERROR_SPECIFIED_ARGUMENTS);
				}

				return strdup(pFunc->m_pContents);
			}

			// If the command's argument calls don't match..
			if (pData->m_nargs < pFunc->m_nArgs)
				RunnerOnError(ERROR_TOO_FEW_ARGUMENTS);
			if (pData->m_nargs > pFunc->m_nArgs)
				RunnerOnError(ERROR_TOO_MANY_ARGUMENTS);

			char* args[C_MAX_ARGS] = { 0 };

			if (pData->m_nargs >= C_MAX_BUILTIN_ARGS)
			{
				RunnerOnError(ERROR_TOO_MANY_ARGUMENTS);
			}

			for (size_t i = 0; i < pData->m_nargs; i++)
			{
				args[i] = RunStatement(pData->m_args[i], 0, NULL);
			}

			char* returnValue = NULL;

			switch (pFunc->type)
			{
				case FUNCTION_POINTER:
				{
					returnValue = pFunc->m_pFunction(args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]);
					break;
				}
				case FUNCTION_STATEMENT:
				{
					returnValue = RunStatement(pFunc->m_pStatement, pFunc->m_nArgs, args);
					break;
				}
				default:
				{
					LogMsg("Don't know how to run function type %d with name %s", pFunc->type, pFunc->m_name);
					RunnerOnError(ERROR_UNKNOWN_STATEMENT); 
				}
			}

			// free the args
			for (size_t i = 0; i < pData->m_nargs; i++)
			{
				if (args[i])
					MemFree(args[i]);

				args[i] = NULL;
			}

			return returnValue;
		}

		default:
		{
			LogMsg("Don't know how to run statement type %d", pStatement->type);
			RunnerOnError(ERROR_UNKNOWN_STATEMENT);
		}
	}

	return NULL;
}

void RunnerAddStandardFunctions()
{
	RunnerAddFunctionPtr(BuiltInHelp,    "help",   0, false);
	RunnerAddFunctionPtr(BuiltInVersion, "ver" ,   0, false);
	RunnerAddFunctionPtr(BuiltInEcho,    "echo",   1, false);
	RunnerAddFunctionPtr(BuiltInGetVer,  "getver", 0, true);
	RunnerAddFunctionPtr(BuiltInEquals,  "equals", 2, true);
	RunnerAddFunctionPtr(BuiltInConcat,  "concat", 2, true);
}

void RunnerGo()
{
	RunnerAddStandardFunctions();
	
	char* chr = RunStatement(g_mainBlock, 0, NULL);

	// If this happens to return anything, free it
	if (chr)
		MemFree(chr);
}

