#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <setjmp.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "nanoshell.h"

#define SHELL_VERSION_STRING "V0.01"

#ifdef _WIN32
#define NORETURN _declspec(noreturn)
#else
#define NORETURN __attribute__((noreturn))
#endif

enum eErrorCode
{
	ERROR_NONE,

	// tokenizer errors
	ERROR_TOKENIZER_START,
	ERROR_MEMORY_ALLOC_FAILURE,
	ERROR_UNTERMINATED_STRING,
	ERROR_UNTERMINATED_ESC_SEQ,
	ERROR_UNKNOWN_ESC_SEQ,
	ERROR_INTERNAL_UNKNOWN_SYMBOL_TOKEN,
	ERROR_INTERNAL_UNKNOWN_KEYWORD_TOKEN,

	// parser errors
	ERROR_PARSER_START,
	ERROR_P_MEMORY_ALLOC_FAILURE,
	ERROR_UNTERMINATED_BLOCK_STMT,
	ERROR_UNTERMINATED_COMMAND_STMT,
	ERROR_UNTERMINATED_FUNCTION_DECL,
	ERROR_INTERNAL_NOT_A_BLOCK_STMT,
	ERROR_INTERNAL_NOT_A_COMMAND_STMT,
	ERROR_INTERNAL_NOT_A_FUNCTION_STMT,
	ERROR_EXPECTED_STATEMENT,
	ERROR_EXPECTED_OPEN_PAREN,
	ERROR_EXPECTED_CLOSE_PAREN,
	ERROR_EXPECTED_SEMICOLON,
	ERROR_EXPECTED_THEN,
	ERROR_EXPECTED_DO,
	ERROR_EXPECTED_OPEN_BLOCK,
	ERROR_EXPECTED_CLOSE_BLOCK,
	ERROR_EXPECTED_SEMICOLON_OR_ARGUMENTS,
	ERROR_EXPECTED_CLOSE_PAREN_OR_ARGUMENTS,
	ERROR_EXPECTED_COMMA,
	ERROR_EXPECTED_EQUALS,
	ERROR_EXPECTED_EQUALS_OR_TO,
	ERROR_EXPECTED_EQUALS_OR_BE,
	ERROR_EXPECTED_STRING,
	ERROR_EXPECTED_FUNCTION_NAME,
	ERROR_EXPECTED_ARGUMENTS,
	ERROR_EXPECTED_BLOCK_STATEMENT,
	ERROR_EXPECTED_FUNCTION_STATEMENT,
	ERROR_EXPECTED_COMMAND_STATEMENT,
	ERROR_EXPECTED_IF_STATEMENT,
	ERROR_EXPECTED_WHILE_STATEMENT,
	ERROR_EXPECTED_STRING_STATEMENT,
	ERROR_EXPECTED_NUMBER_STATEMENT,
	ERROR_EXPECTED_EMPTY_STATEMENT,
	ERROR_EXPECTED_LET_STATEMENT,
	ERROR_EXPECTED_ASSIGN_STATEMENT,
	ERROR_EXPECTED_VARIABLE_NAME,
	ERROR_EXPECTED_KEYWORD,

	// runtime errors
	ERROR_RUNTIME_START,
	ERROR_R_MEMORY_ALLOC_FAILURE,
	ERROR_UNKNOWN_STATEMENT,
	ERROR_UNKNOWN_FUNCTION,
	ERROR_SPECIFIED_ARGUMENTS,
	ERROR_TOO_MANY_ARGUMENTS,
	ERROR_TOO_FEW_ARGUMENTS,
	ERROR_TOO_MANY_ARGS_DEF,
	ERROR_FUNCTION_ALREADY_EXISTS,
	ERROR_VARIABLE_ALREADY_EXISTS,
	ERROR_ASSIGNEE_IS_NOT_VARIABLE,
	ERROR_STACK_OVERFLOW,
	ERROR_EXPECTED_STRING_PARM,
	ERROR_EXPECTED_INT_PARM,
	ERROR_UNKNOWN_VARIANT_TYPE,
	ERROR_IF_EXPECTS_INT,
	ERROR_WHILE_EXPECTS_INT,
	ERROR_INT_CONVERSION_FAILURE,
	ERROR_ARRAY_INDEX_OUT_OF_BOUNDS,
	ERROR_DIVISION_BY_ZERO,

	ERROR_END,
};
/*
void RunnerAddStandardFunctions()
{
	RunnerAddFunctionPtr(BuiltInHelp,     "help",   0, false);
	RunnerAddFunctionPtr(BuiltInVersion,  "ver",    0, false);
	RunnerAddFunctionPtr(BuiltInEcho,     "echo",   1, false);
	RunnerAddFunctionPtr(BuiltInGetVer,   "getver", 0, true);
	RunnerAddFunctionPtr(BuiltInEquals,   "equals", 2, true);
	RunnerAddFunctionPtr(BuiltInConcat,   "concat", 2, true);
	RunnerAddFunctionPtr(BuiltInToString, "str",    1, true);
	RunnerAddFunctionPtr(BuiltInToInt,    "int",    1, true);

	// Arithmetic operations
	RunnerAddFunctionPtr(BuiltInAdd,      "add", 2, true);
	RunnerAddFunctionPtr(BuiltInSub,      "sub", 2, true);
	RunnerAddFunctionPtr(BuiltInMul,      "mul", 2, true);
	RunnerAddFunctionPtr(BuiltInDiv,      "div", 2, true);
	RunnerAddFunctionPtr(BuiltInLessThan, "lt",  2, true);
	RunnerAddFunctionPtr(BuiltInMoreThan, "gt",  2, true);
	RunnerAddFunctionPtr(BuiltInAnd,      "and", 2, true);
	RunnerAddFunctionPtr(BuiltInOr,       "or",  2, true);
}

*/

char GetErrorCategory(int error);
int  GetErrorNumber(int error);
const char* GetErrorMessage(int error);

void LoadFile(const char* pfn);

