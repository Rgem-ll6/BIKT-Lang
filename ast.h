#ifndef AST_H
#define AST_H

#include <stdbool.h>
#include <stddef.h>

typedef enum {
	EXPR_NUMBER,
	EXPR_STRING,
	EXPR_IDENTIFIER,
	EXPR_BINARY_OP,
	EXPR_UNARY_OP,
	EXPR_FUNCTION_CALL
} ExpressionType;

typedef struct Expression Expression;
typedef struct Statement Statement;

//Number Literal...
typedef struct {
	long value;
} NumExpr;

//String literal...
typedef struct {
	char* value;
} StrExpr;

//Identifiers...
typedef struct {
	char* value;
} IdentifierExpr;

//Binary Operations...
typedef struct {
	Expression* left;
	char* operator;
	Expression* right;
} BOExpr;

//Unary OPerators
typedef struct {
	char* operator;
	Expression* operand;
} UOExpr;

typedef struct {
	char* func_name;
	Expression** args;
	size_t arg_c;
} FunctionExpr;

typedef struct Expression {
	ExpressionType type;
	union {
		NumExpr number;
		StrExpr string;
		IdentifierExpr identifier;
		BOExpr binOP;
		UOExpr uOP;
		FunctionExpr func;
	} data;
} Expression;

typedef enum {
	LET,
	ASSIGN,
	RETURN,
	PRINT,
	IF,
	EXPR,
	BLOCK
} StatementType;

typedef struct {
	Expression* expression;
} ExprStmt;

typedef struct {
	char* name;
	char* type;
	Expression* init;
}LetStmt;

typedef struct {
	char* name;
	Expression* value;
} AssignStmt;

typedef struct {
	Expression* value;
} ReturnStmt;

typedef struct {
	Expression* value;
} PrintStmt;

typedef struct {
	Expression* condition;
	Statement** then_branch;
	size_t then_count;
	Statement** else_branch;
	size_t else_count;
} IfStmt;

typedef struct {
	Statement** statements;
    size_t count;
} BlockStmt;

typedef struct Statement {
	StatementType type;
	union {
		LetStmt let_s;
		AssignStmt assign_s;
		ReturnStmt rtrn_s;
		PrintStmt print_s;
		IfStmt if_s;
		BlockStmt blk_s;
		ExprStmt expr_s;
	} data;
} Statement;

typedef struct {
	char* name;
	char** parameters;
	size_t param_count;
	Statement** body;
	size_t body_count;
} Method;

typedef struct {
	Method** funcs;
	size_t func_count;
	Statement** statements;
	size_t stmt_count;
} Program;

#endif
