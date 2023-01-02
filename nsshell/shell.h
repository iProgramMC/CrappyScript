#pragma once

enum eErrorCode
{
	ERROR_NONE,

	// tokenizer errors
	ERROR_TOKENIZER_START,
	ERROR_MEMORY_ALLOC_FAILURE,
	ERROR_UNTERMINATED_STRING,
	ERROR_UNTERMINATED_ESC_SEQ,
	ERROR_UNKNOWN_ESC_SEQ,

	// parser errors
	ERROR_PARSER_START,

	ERROR_END,
};

#include <stdbool.h>
#include <setjmp.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

void LoadFile(const char* pfn);

