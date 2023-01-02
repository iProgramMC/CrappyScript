#pragma once

typedef enum
{
	STMT_NULL,
	STMT_COMMAND,
	STMT_BLOCK,
	STMT_IF,
	STMT_WHILE,
	STMT_STRING,
	STMT_FUNCTION,
	STMT_VARIABLE,
	STMT_ASSIGNMENT,
}
eStatementType;

typedef struct STATEMENT Statement;

// note: Also applies to other structures, such as 'while'
typedef struct
{
	Statement* m_condition;
	Statement* m_true_part;
	// if there's an 'else', this will be active and not NULL
	Statement* m_false_part;
}
StatementIfData;

typedef struct
{
	char* m_name;
	Statement** m_args; // note: only command statements
	size_t m_nargs;
}
StatementCmdData;

typedef struct
{
	Statement** m_statements;
	size_t      m_nstatements;
}
StatementBlkData;

typedef struct
{
	char* m_str;
}
StatementStrData;

typedef struct
{
	char* m_name;
	char** m_args;
	size_t m_nargs;
	Statement* m_statement;
}
StatementFunData;

typedef struct
{
	char* m_name;
	Statement* m_statement;
}
StatementVarData;

typedef struct
{
	char* m_varName;
	Statement* m_statement;
}
StatementAsgData;
// No, I'm not going to name this "StatementAssData"

typedef struct STATEMENT
{
	eStatementType type;

	union
	{
		void* m_data;
		StatementIfData* m_if_data;
		StatementBlkData* m_blk_data;
		StatementCmdData* m_cmd_data;
		StatementStrData* m_str_data;
		StatementFunData* m_fun_data;
		StatementVarData* m_var_data;
		StatementAsgData* m_asg_data;
	};
}
Statement;

void Parse();
void ParserTeardown();


