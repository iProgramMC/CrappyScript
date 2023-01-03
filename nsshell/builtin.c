#include "nanoshell.h"
#include "builtin.h"

void RunnerOnError(int error);

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
	if (str->m_type == VAR_INT)
		LogMsg("%lld", str->m_intValue);
	else if (str->m_type == VAR_STRING)
		LogMsg("%s", str->m_strValue);
	else
		LogMsg("???");
	return NULL;
}

Variant* BuiltInEquals(Variant* str1, Variant* str2)
{
	if (str1->m_type != VAR_STRING || str2->m_type != VAR_STRING)
		RunnerOnError(ERROR_EXPECTED_STRING_PARM);

	if (strcmp(str1->m_strValue, str2->m_strValue) == 0)
		return VariantCreateInt(1);
	else
		return VariantCreateInt(0);
}

Variant* BuiltInConcat(Variant* str1, Variant* str2)
{
	if (str1->m_type != VAR_STRING || str2->m_type != VAR_STRING)
		RunnerOnError(ERROR_EXPECTED_STRING_PARM);

	size_t len1 = strlen(str1->m_strValue), len2 = strlen(str2->m_strValue);

	char* cpy = MemAllocate(len1 + len2 + 1);
	memcpy(cpy,        str1->m_strValue, len1);
	memcpy(cpy + len1, str2->m_strValue, len2 + 1);

	Variant* pVar = VariantCreateString(cpy);
	MemFree(cpy);

	return pVar;
}
