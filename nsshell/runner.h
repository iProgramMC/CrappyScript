#pragma once

#include "parser.h"

#define C_MAX_ARGS (128)
#define C_MAX_BUILTIN_ARGS (8)

typedef enum
{
	FUNCTION_STATEMENT,
	FUNCTION_POINTER, //Built in function
	FUNCTION_VARIABLE,//Actually just a variable
}
eFunctionType;

// note: This function isn't just "void"
// note: This returns a malloc'ed pointer. Free it properly.
// note: All parameters are also malloc'ed pointers.
typedef char* (*CallableFunPtr) ();

typedef struct Function Function;

struct Function
{
	Function* m_nlink;
	Function* m_plink;

	eFunctionType type;
	const char* m_name;
	int m_nArgs;
	bool m_bReturns;

	union
	{
		Statement* m_pStatement;
		CallableFunPtr m_pFunction;
		char* m_pContents;
	};
};

void RunnerGo();
