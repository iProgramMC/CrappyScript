#include "nanoshell.h"
#include "builtin.h"

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
	if (str1->m_type != str2->m_type) return;

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
