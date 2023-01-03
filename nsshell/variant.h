#pragma once

#include <assert.h>
#include "nanoshell.h"

typedef enum
{
	VAR_INT,
	VAR_STRING,
	//...
}
eVariantType;

typedef struct Variant
{
	eVariantType m_type;

	union {
		long long m_intValue;
		char* m_strValue;
	};
}
Variant;

Variant* VariantCreateInt(long long value);
Variant* VariantCreateString(const char* value);
Variant* VariantDuplicate(Variant* pVar);
void VariantFree(Variant* pVariant);
