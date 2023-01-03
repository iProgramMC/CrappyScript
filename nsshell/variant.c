#include "variant.h"

Variant* VariantCreateInt(long long value)
{
	Variant* pVar = MemCAllocate(1, sizeof(Variant));
	pVar->m_type = VAR_INT;
	pVar->m_intValue = value;
	return pVar;
}

Variant* VariantCreateString(const char* value)
{
	Variant* pVar = MemCAllocate(1, sizeof(Variant));
	pVar->m_type = VAR_STRING;
	pVar->m_strValue = StrDuplicate(value);
	return pVar;
}

Variant* VariantDuplicate(Variant* pVar)
{
	Variant* pNewVar = MemCAllocate(1, sizeof(Variant));
	pNewVar->m_type = pVar->m_type;

	switch (pVar->m_type)
	{
		case VAR_INT:
			pNewVar->m_intValue = pVar->m_intValue;
			break;
		case VAR_STRING:
			pNewVar->m_strValue = StrDuplicate(pVar->m_strValue);
			break;
		default:
			assert("Don't know how to clone variant");
	}

	return pNewVar;
}

void VariantFree(Variant* pVariant)
{
	if (pVariant->m_type == VAR_STRING)
		MemFree(pVariant->m_strValue);
	MemFree(pVariant);
}
