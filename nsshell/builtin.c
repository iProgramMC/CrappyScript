#include "nanoshell.h"
#include "builtin.h"
#include "runner.h"
#include <errno.h>

NORETURN void RunnerOnError(int error);

Variant* BuiltInHelp()
{
	LogMsg("Help was called!");
	return NULL;
}

Variant* BuiltInVersion()
{
	LogMsg("NanoShell Shell Version %s", SHELL_VERSION_STRING);
	return NULL;
}

Variant* BuiltInGetVer()
{
	return VariantCreateString(SHELL_VERSION_STRING);
}

Variant* BuiltInEcho(Variant* str)
{
	switch (str->m_type)
	{
		case VAR_INT:
			LogMsg("%lld", str->m_intValue);
			break;
		case VAR_STRING:
			LogMsg("%s", str->m_strValue);
			break;
		default:
			RunnerOnError(ERROR_UNKNOWN_VARIANT_TYPE);
			break;
	}
	return NULL;
}

Variant* BuiltInEquals(Variant* str1, Variant* str2)
{
	if (str1->m_type != str2->m_type) return VariantCreateInt(0);

	switch (str1->m_type)
	{
		case VAR_INT:
			return VariantCreateInt(str1->m_intValue == str2->m_intValue);
		case VAR_STRING:
			return VariantCreateInt(!strcmp(str1->m_strValue, str2->m_strValue));
		default:
			RunnerOnError(ERROR_UNKNOWN_VARIANT_TYPE);
	}
}

Variant* BuiltInConcat(Variant* str1, Variant* str2)
{
	if (str1->m_type != VAR_STRING || str2->m_type != VAR_STRING)
		RunnerOnError(ERROR_EXPECTED_STRING_PARM);

	size_t len1 = strlen(str1->m_strValue), len2 = strlen(str2->m_strValue);

	char* cpy = MemAllocate(len1 + len2 + 1);
	if (!cpy) RunnerOnError(ERROR_R_MEMORY_ALLOC_FAILURE);

	memcpy(cpy,        str1->m_strValue, len1);
	memcpy(cpy + len1, str2->m_strValue, len2 + 1);

	Variant* pVar = VariantCreateString(cpy);
	MemFree(cpy);

	return pVar;
}

Variant* BuiltInToString(Variant* var)
{
	switch (var->m_type)
	{
		case VAR_STRING:
		{
			return VariantDuplicate(var);
		}
		case VAR_INT:
		{
			char buffer[64];
			snprintf(buffer, sizeof buffer, "%lld", var->m_intValue);
			return VariantCreateString(buffer);
		}
		default:
		{
			RunnerOnError(ERROR_UNKNOWN_VARIANT_TYPE);
		}
	}
}

Variant* BuiltInToInt(Variant* var)
{
	switch (var->m_type)
	{
		case VAR_STRING:
		{
			errno = 0;
			long long value = strtoll(var->m_strValue, NULL, 0);
			if (errno != 0)
				RunnerOnError(ERROR_INT_CONVERSION_FAILURE);

			return VariantCreateInt(value);
		}
		case VAR_INT:
		{
			return VariantDuplicate(var);
		}
		default:
		{
			RunnerOnError(ERROR_UNKNOWN_VARIANT_TYPE);
		}
	}
}

Variant* BuiltInAdd(Variant* var1, Variant* var2)
{
	if (var1->m_type != VAR_INT || var2->m_type != VAR_INT)
		RunnerOnError(ERROR_EXPECTED_INT_PARM);

	return VariantCreateInt(var1->m_intValue + var2->m_intValue);
}

Variant* BuiltInSub(Variant* var1, Variant* var2)
{
	if (var1->m_type != VAR_INT || var2->m_type != VAR_INT)
		RunnerOnError(ERROR_EXPECTED_INT_PARM);

	return VariantCreateInt(var1->m_intValue - var2->m_intValue);
}

Variant* BuiltInMul(Variant* var1, Variant* var2)
{
	if (var1->m_type != VAR_INT || var2->m_type != VAR_INT)
		RunnerOnError(ERROR_EXPECTED_INT_PARM);

	return VariantCreateInt(var1->m_intValue * var2->m_intValue);
}

Variant* BuiltInDiv(Variant* var1, Variant* var2)
{
	if (var1->m_type != VAR_INT || var2->m_type != VAR_INT)
		RunnerOnError(ERROR_EXPECTED_INT_PARM);

	return VariantCreateInt(var1->m_intValue / var2->m_intValue);
}

Variant* BuiltInLessThan(Variant* var1, Variant* var2)
{
	if (var1->m_type != VAR_INT || var2->m_type != VAR_INT)
		RunnerOnError(ERROR_EXPECTED_INT_PARM);

	return VariantCreateInt(var1->m_intValue < var2->m_intValue);
}

Variant* BuiltInMoreThan(Variant* var1, Variant* var2)
{
	if (var1->m_type != VAR_INT || var2->m_type != VAR_INT)
		RunnerOnError(ERROR_EXPECTED_INT_PARM);

	return VariantCreateInt(var1->m_intValue > var2->m_intValue);
}

Variant* BuiltInAnd(Variant* var1, Variant* var2)
{
	if (var1->m_type != VAR_INT || var2->m_type != VAR_INT)
		RunnerOnError(ERROR_EXPECTED_INT_PARM);

	return VariantCreateInt(var1->m_intValue & var2->m_intValue);
}

Variant* BuiltInOr(Variant* var1, Variant* var2)
{
	if (var1->m_type != VAR_INT || var2->m_type != VAR_INT)
		RunnerOnError(ERROR_EXPECTED_INT_PARM);

	return VariantCreateInt(var1->m_intValue | var2->m_intValue);
}


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

