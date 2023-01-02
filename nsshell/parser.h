#pragma once

typedef enum
{
	STMT_NULL,
	STMT_COMMAND,
	STMT_BLOCK,
	STMT_IF,
}
eStatementType;

typedef struct STATEMENT Statement;

typedef struct
{
	Statement* m_condition;
	size_t     m_condition_size;

	Statement* m_true_part;
	size_t     m_true_part_size;

	// if there's an 'else', this will be active and not NULL
	Statement* m_false_part;
	size_t     m_false_part_size;
}
StatementIfData;

typedef struct
{
	// This is a regular old command.
	char** tokens;
	size_t ntokens;
}
StatementCmdData;

typedef struct
{
	Statement** m_statements;
	size_t      m_nstatements;
}
StatementBlkData;

typedef struct STATEMENT
{
	eStatementType type;

	union
	{
		void* m_data;
		StatementIfData* m_if_data;
		StatementBlkData* m_blk_data;
		StatementCmdData* m_cmd_data;
	};
}
Statement;

void Parse();


