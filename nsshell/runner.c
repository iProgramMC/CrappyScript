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

	Function* pFunc = calloc(1, sizeof (Function));
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

	Function* pFunc = calloc(1, sizeof(Function));
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

				// if it returned something, we most likely won't use it. Free the memory.
				if (returnValue)
					MemFree(returnValue);
			}

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
}

void RunnerGo()
{
	RunnerAddStandardFunctions();
	
	char* chr = RunStatement(g_mainBlock, 0, NULL);

	// If this happens to return anything, free it
	if (chr)
		MemFree(chr);
}

