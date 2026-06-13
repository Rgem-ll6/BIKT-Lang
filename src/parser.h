/*

HEY, I'M THE AUTHOR OF BIKT, GO BY THE NAME 'RHEMA' BUT MOST PEOPLE CALL ME
'RHEMA', I'M A LOW LEVEL DEVELOPER ONLY, EZZ
SOMETIMES I THINK I'M SCHIZO...
COULD ALL BE JUST PART OF MY IMAGINATION.
I PRETTY MUC DON'T LIKE...
BUT SINCE THA'S IMPOSSIBLE I CAME UP WITH AN IDEA, BIKT AND BIKT-OS, SO UHH TO WRAP THINGS UP
IF YOU'RE ACTUALLY READING THIS AND PLAN ON READING THE REST OF THE SOURCECODE
JUST KNOW YOU'RE ABOUT TO READ 800+ LINES OF PURE MISERY...
EVERY LINE FROM 200+ WAS WRITTEN IN PAIN, I'M ALSO PRETTY SURE I'VE GONE SENILE, BUT OH WELL
ENJOY...

*/

//macros..
#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lexer.h"
#include "ast.h"

//main parser struct...
typedef struct {
	Lexer* lx;
	Token current_token;
	Token peek_token;
	bool has_error;
} Parser;


//function prototype pre-definitions....
//Expressions...
Expression* parseAdditiveExpr(Parser* p);
Expression* parseMultiplicativeExpr(Parser* p);
Expression* parseUnaryExpr(Parser* p);
Expression* parseComparisonExpr(Parser* p);
Expression* parsePrimaryExpr(Parser* p);
Expression* parseInputExpression(Parser* p);

//Statements...
Statement* parseIfStatement(Parser* p);
Statement* parseStatement(Parser* p);
Statement* parseLetStatement(Parser* p);
Statement* parseReturnStatement(Parser* p);
Statement* parsePrintStatement(Parser* p);
Statement* parseBlockStatement(Parser* p);
Statement* parseAssignStatement(Parser* p);
Statement* parseExpressionStatement(Parser* p);

//core methods
Method* parseMethod(Parser* p);
Program* parseProgram(Parser* p);

//Parser initializer
Parser* initParser(const char* input){
	if (input == NULL){
		fprintf(stderr, "Runtime Parser Error: Failed to initialize parser with NULL input\n");
	return NULL;
	}

	Parser* p = (Parser*)malloc(sizeof(Parser));
	if (p == NULL){
		fprintf(stderr, "Runtime Parser Error: Failed to allocate memory for initParser()\n");
	return NULL;
	}

	p->lx = initLexer(input);
	if (p->lx == NULL){
		fprintf(stderr,"Runtime Parser Error: Failed to initilaize Lexer\n");
		free(p);
	return NULL;
	}

	p->current_token = nextToken(p->lx);

	p->peek_token = nextToken(p->lx);

	p->has_error = false;

return p;
}

//Aha! my favorites, helper functions...
void parser_advance(Parser* p){
	if (p == NULL) return;
	p->current_token = p->peek_token;
	//then advance the two of them so their always 1 step ahead...
	p->peek_token = nextToken(p->lx);
}

Token parser_peek(Parser* p){
	if (p == NULL){
		Token mpty;
		mpty.type = TT_Unknown;
		mpty.value = NULL;
		return mpty;
	}

	return p->current_token;
}

Token parser_peekNext(Parser* p){
	if (p == NULL){
		Token m;
		m.type = TT_Unknown;
		m.value = NULL;
		return m;
	}

	return p->peek_token;
}

//basically a load of shit...
bool parser_match(Parser* p, TokenType type){
	if (p == NULL) return false;

	if (p->current_token.type == type){
		parser_advance(p);
		return true;
	}

return false;
}

bool parser_expect(Parser* p, TokenType type, const char* err_msg){
	if (p == NULL) return false;

	if (p->current_token.type == type){
		parser_advance(p);
		return true;
	}

	fprintf(stderr, "Parser Error: %s", err_msg);
	fprintf(stderr, "Expected Token type %d, but got %d", type, p->current_token.type);
	p->has_error = true;

	return false;
}

//tricky parts now....primary expressions...

//this expression method is for the expressions '+' and '-'...
Expression* parseAdditiveExpr(Parser* p){
	if (p == NULL || p->has_error) return NULL;

	Expression* left = parseMultiplicativeExpr(p);
	if (left == NULL) return NULL;

	while (parser_peek(p).type == TT_Plus || parser_peek(p).type == TT_Minus){
		Token op_token = parser_peek(p);
		char* op = (char*)malloc(strlen(op_token.value) + 1);
		strcpy(op, op_token.value);

		 parser_advance(p);
         //using the multiplicative expression cause of order of precedence...
		 Expression* right = parseMultiplicativeExpr(p);
		 if (right == NULL){
			fprintf(stderr, "Compiler Error: Expected Expression after the %s operator", op);
			p->has_error = true;
			free(op);
			return NULL;
		 }
		 
         //creating the AST Node
		 Expression* combined = (Expression*)malloc(sizeof(Expression));
		 combined->type = EXPR_BINARY_OP;
		 combined->data.binOP.left = left;
		 combined->data.binOP.operator = op;
		 combined->data.binOP.right = right;

		 left = combined;
	}
	return left;
}

//y'know exprs for building block like 5, "ehellleo",etc.
Expression* parsePrimaryExpr(Parser* p){
	if (p == NULL || p->has_error) return NULL;
	Token current = parser_peek(p);

	if (current.type == TT_Input){
		return parseInputExpression(p);
	}

	// Number Literals
	if (current.type == TT_Number){
		Expression* expr = (Expression*)malloc(sizeof(Expression));
		expr->type = EXPR_NUMBER;
		expr->data.number.value = strtol(current.value, NULL, 10);
		parser_advance(p);
		return expr;
	}

	// String Literals
	if (current.type == TT_String){
		Expression* str_expr = (Expression*)malloc(sizeof(*str_expr));
		str_expr->type = EXPR_STRING;
		str_expr->data.string.value = (char*)malloc(strlen(current.value) + 1);
		strcpy(str_expr->data.string.value, current.value);
		parser_advance(p);
		return str_expr;
	}

	// Identifier or Function Call
	if (current.type == TT_Identifier || current.type == TT_Print){
		char* name = (char*)malloc(strlen(current.value) + 1);
		strcpy(name, current.value);
		parser_advance(p);

		// Check if it's a function call
		if (parser_peek(p).type == TT_LParen){
			Expression* func_expr = (Expression*)malloc(sizeof(Expression));
			func_expr->type = EXPR_FUNCTION_CALL;
			func_expr->data.func.func_name = name;
			parser_advance(p);

            //for function calls not definitions, the definition will be in the parseMethod() method
			Expression** arguments = NULL;
			size_t arg_count = 0;

            //man this is where the misery starts hehe...
			if (parser_peek(p).type != TT_RParen){
				Expression* arg = parseAdditiveExpr(p);
				if (arg == NULL){
					p->has_error = true;
					return NULL;
				}

				arguments = (Expression**)malloc(sizeof(Expression*));
				arguments[0] = arg;
				arg_count = 1;
                    //i was a hero but you get the glory!
					while (parser_peek(p).type == TT_Comma && !p->has_error){
						parser_advance(p);

						Expression* next_arg = parseAdditiveExpr(p);
						if (next_arg == NULL){
							p->has_error = true;
							return NULL;
						}

						arguments = (Expression**)realloc(arguments, (arg_count + 1) * sizeof(Expression*));
						arguments[arg_count] = next_arg;
						arg_count++;
					}
			}

			if (!parser_expect(p, TT_RParen, "Compiler Error: Expected closing parenthesis after function call")){
				return NULL;
			}

			func_expr->data.func.args = arguments;
			func_expr->data.func.arg_c = arg_count;
			return func_expr;
		} else {
			// Just an identifier
			Expression* iden_expr = (Expression*)malloc(sizeof(Expression));
			iden_expr->type = EXPR_IDENTIFIER;
			iden_expr->data.identifier.value = name;
			return iden_expr;
		}
	}

	// Parenthesized expression
	if (current.type == TT_LParen){
		parser_advance(p);
		Expression* paren_expr = parseAdditiveExpr(p);
		if (paren_expr == NULL) return NULL;
		if (!parser_expect(p, TT_RParen, "Compiler Error: Expected closing parenthesis after expression")){
			return NULL;
		}
		return paren_expr;
	}

	// Error
	fprintf(stderr, "Parser Error: Unexpected token in expression, Got token type: %d\n", current.type);
	p->has_error = true;
	return NULL;
}

//while this expression is for the expressions '*' and '/' since these have...
//...higher precedence
Expression* parseMultiplicativeExpr(Parser* p){
	if (p == NULL || p->has_error) return NULL;

	Expression* left = parseUnaryExpr(p);
	if (left == NULL) return NULL;

	while (parser_peek(p).type == TT_Astk || parser_peek(p).type == TT_Slash){
		Token op_token = parser_peek(p);
		char* operator = (char*)malloc(strlen(op_token.value) + 1);
		strcpy(operator, op_token.value);

		parser_advance(p);

		Expression* right = parseUnaryExpr(p);
		if (right == NULL){
			fprintf(stderr, "Compiler Error: Expected Expression after the %s operator", operator);
			p->has_error = true;
			free(operator);
			return NULL;
		}

		Expression* combined = (Expression*)malloc(sizeof(Expression));
		combined->type = EXPR_BINARY_OP;
		combined->data.binOP.left = left;
		combined->data.binOP.operator = operator;
		combined->data.binOP.right = right;

		left = combined;
	}
	return left;
}

//expressions like '-5' and '+70'
Expression* parseUnaryExpr(Parser* p){
	if (p == NULL || p->has_error) return NULL;

	if (parser_peek(p).type == TT_Minus || parser_peek(p).type == TT_Plus){
		char* op = (char*)malloc(strlen(parser_peek(p).value) + 1);
		strcpy(op, parser_peek(p).value);

		parser_advance(p);

        //it's a recursive function
		Expression* operand = parseUnaryExpr(p);
		if (operand == NULL){
			fprintf(stderr, "Compiler Error: Expected expression after unary operator %s\n", op);
			p->has_error = true;
			free(op);
			return NULL;
		}
        //AST* node
		Expression* unary_expr = (Expression*)malloc(sizeof(Expression));
		unary_expr->type = EXPR_UNARY_OP;
		unary_expr->data.uOP.operator = op;
		unary_expr->data.uOP.operand = operand;

		return unary_expr;
	}
	return parsePrimaryExpr(p);
}

//conditions '5 < 7' or '20 > 10'
Expression* parseComparisonExpr(Parser* p){
	if (p == NULL || p->has_error) return NULL;

	Expression* left = parseAdditiveExpr(p);
	if (left == NULL) return NULL;

	if (parser_peek(p).type == TT_Greater || parser_peek(p).type == TT_Less){
		char* optr = (char*)malloc(strlen(parser_peek(p).value) + 1);
		strcpy(optr, parser_peek(p).value);

		parser_advance(p);

		Expression* right = parseAdditiveExpr(p);
		if (right == NULL){
			fprintf(stderr, "Compiler Error: Expected expression after '%s' operand\n", optr);
			p->has_error = true;
			free(optr);
			return NULL;
		}

		Expression* combined = (Expression*)malloc(sizeof(Expression));
		combined->type = EXPR_BINARY_OP;
		combined->data.binOP.left = left;
		combined->data.binOP.operator = optr;
		combined->data.binOP.right = right;

		left = combined;
	}
	return left;
}

Statement* parseIfStatement(Parser* p){
	if (p == NULL || p->has_error) return NULL;

    //expect 'if' keyword
	if (!parser_expect(p, TT_If, "Parser Error: Expected 'if' statement\n")){
		return NULL;
	}

	if (!parser_expect(p, TT_LParen, "Compiler Error: Expected '(' after the if statement\n")){
		return NULL;
	}

	Expression* condition = parseComparisonExpr(p);
	if (condition == NULL){
		fprintf(stderr, "Compiler Error: Failed to parse condititon expression\n");
		p->has_error = true;
		return NULL;
	}

	if (!parser_expect(p, TT_RParen, "Compiler Error: Expected ')' after the if condition expression\n")){
		return NULL;
	}

    //have you also gone mad?
	Statement** then_block = NULL;
	size_t then_count = 0;

	if (parser_peek(p).type == TT_LBrace){
		parser_advance(p); //consume the left brace, parser_expect() also consumes so don't be scared...
		while (parser_peek(p).type != TT_RBrace && !p->has_error){
			Statement* stmt = parseStatement(p);
			if (stmt == NULL) continue;

            //i know you'll go coocoo when you see this
			then_block = (Statement**)realloc(then_block, (then_count + 1) * sizeof(Statement*));
			then_block[then_count] = stmt;
			then_count++;
		}
	}

	if (!parser_expect(p, TT_RBrace, "Compiler Error: Expected '}' after if block")){
		return NULL;
	}

	Statement** else_block = NULL;
	size_t else_count = 0;

    //check if there's the 'else' keyword after the then_block
	if (parser_peek(p).type == TT_Else){
		parser_advance(p);

		if (parser_peek(p).type == TT_LBrace){
			parser_advance(p);

			while (parser_peek(p).type != TT_RBrace && !p->has_error){

				Statement* stmt = parseStatement(p);
				if (stmt == NULL) continue;

                //hahahaha, its just a realloc, memory reallocation, get it?
				else_block = (Statement**)realloc(else_block, (else_count + 1) * sizeof(Statement*));
				else_block[else_count] = stmt;
				else_count++;
			}

			if (!parser_expect(p, TT_RBrace, "Compiler Error: Expected '}' after else block")){
				return NULL;
			}
		}
	}

    //this is memory allocation, get it? malloc?
	Statement* if_stmt = (Statement*)malloc(sizeof(Statement));

	if_stmt->type = IF;
	if_stmt->data.if_s.condition = condition;
	if_stmt->data.if_s.then_branch = then_block;
	if_stmt->data.if_s.then_count = then_count;
	if_stmt->data.if_s.else_branch = else_block;
	if_stmt->data.if_s.else_count = else_count;

	return if_stmt;
}

Statement* parseStatement(Parser* p){
	if (p == NULL || p->has_error) return NULL;

	//'let' keyword...
	if (parser_peek(p).type == TT_Let){
		return parseLetStatement(p);
	}

	//'rtrn' keyword...
	if (parser_peek(p).type == TT_Return){
		return parseReturnStatement(p);
	}

	//'outlog' keyword...
	if (parser_peek(p).type == TT_Print){
		return parsePrintStatement(p);
	}

	//'inlog' keyword...
	//i/f (parser_peek(p).type == TT_Input){
	//	return parseInputStatement(p);
	//}

	//'if' keyword...
	if (parser_peek(p).type == TT_If){
		return parseIfStatement(p);
	}

	//'{' statement block...
	if (parser_peek(p).type == TT_LBrace){
		return parseBlockStatement(p);
	}

	//assignment...
	if (parser_peek(p).type == TT_Identifier && parser_peekNext(p).type == TT_Equals){
		return parseAssignStatement(p);
	}

	//otherwise
	return parseExpressionStatement(p);
}

//parse any expression brev....
//whohooooo lallal!
Statement* parseExpressionStatement(Parser* p){
	if (p == NULL || p->has_error) return NULL;

	Expression* expr = parseComparisonExpr(p);
	if (expr == NULL){
		fprintf(stderr, "Compiler Error: Failed to parse expression statement\n");
		return NULL;
	}

	if (!parser_expect(p, TT_Semicolon, "Compiler Error: Expected ';' after expression statement")){
		return NULL;
	}

	Statement* expr_stmt = (Statement*)malloc(sizeof(Statement));
	expr_stmt->type = EXPR;
	expr_stmt->data.expr_s.expression = expr;
	return expr_stmt;
}

//Let statement....
//i plan on using Rust's system of creating variables...
//let a: int = 5; OR let name: string = "Rhema Ugwu";
Statement* parseLetStatement(Parser* p){
	if (p == NULL || p->has_error) return NULL;

	if (!parser_expect(p, TT_Let, "Parser Error: Expected the 'let' keyword\n")){
		return NULL;
	}

	if (parser_peek(p).type != TT_Identifier){
		fprintf(stderr, "Compiler Error: Expected an identifier after the 'let' keyword\n");
		p->has_error = true;
		return NULL;
	}

	char* var_name = (char*)malloc(strlen(parser_peek(p).value) + 1);
	strcpy(var_name, parser_peek(p).value);
	parser_advance(p);

	if (!parser_expect(p, TT_Colon, "Compiler Error: Expected ':' after variable identifier\n")){
		free(var_name);
		return NULL;
	}

	if (parser_peek(p).type != TT_Identifier){
		fprintf(stderr, "Compiler Error: Expected data type after ':'\n");
		p->has_error = true;
		free(var_name);
		return NULL;
	}

	char* var_type = (char*)malloc(strlen(parser_peek(p).value) + 1);
	strcpy(var_type, parser_peek(p).value);
	parser_advance(p);

	Expression* init_expr = NULL;

	if (parser_peek(p).type == TT_Equals){
		parser_advance(p);

		init_expr = parseComparisonExpr(p);
		if (init_expr == NULL){
			fprintf(stderr, "Compiler Error: Failed to Parse initialization expression\n");
			p->has_error = true;
			free(var_name);
			free(var_type);
			return NULL;
		}
	}

	//semicolon...
	if (!parser_expect(p, TT_Semicolon, "Compiler Error: Expected ';' after variable decleration\n")){
		free(var_name);
		free(var_type);
		return NULL;
	}

	Statement* let_stmt = (Statement*)malloc(sizeof(Statement));
	let_stmt->type = LET;
	let_stmt->data.let_s.name = var_name;
	let_stmt->data.let_s.type = var_type;
	let_stmt->data.let_s.init = init_expr;

	return let_stmt;
}

//assignment stmt, ass operator '=' hahahahaha!
Statement* parseAssignStatement(Parser* p){
	if (p == NULL || p->has_error) return NULL;

	if (parser_peek(p).type != TT_Identifier){
		fprintf(stderr, "Compiler Error: Expected identifier in assignment expression\n");
		p->has_error = true;
		return NULL;
	}

	char* var_name = (char*)malloc(strlen(parser_peek(p).value) + 1);
	strcpy(var_name, parser_peek(p).value);
	parser_advance(p); //always remember when to advance, to consume tokens or to move ahead...

	if (!parser_expect(p, TT_Equals, "Compiler Error: Expected '=' after variable name\n")){
		free(var_name);
		return NULL;
	}

	Expression* value_expr = parseComparisonExpr(p);
	if (value_expr == NULL){
		fprintf(stderr, "Compiler Error: failed to parse expression in the assignment variable %s\n", var_name);
		p->has_error = true;
		free(var_name);
		return NULL;
	}

	//semicolon...
	if (!parser_expect(p, TT_Semicolon, "Compiler Error: Expected ';' after variable decleration\n")){
		free(var_name);
		return NULL;
	}

	Statement* ass_stmt = (Statement*)malloc(sizeof(Statement));
	ass_stmt->type = ASSIGN;
	ass_stmt->data.assign_s.name = var_name;
	ass_stmt->data.assign_s.value = value_expr;

	return ass_stmt;
}

//RETURNNNN!! im pretty sure you've lost your mind already hehe
Statement* parseReturnStatement(Parser* p){
	if (p == NULL || p->has_error) return NULL;

	if (!parser_expect(p, TT_Return, "Compiler Error: Expected 'rtrn' statement at the end of main method\n" )){
		return NULL;
	}

	Expression* ret_expr = NULL;

	//semicolon...
	if (parser_peek(p).type != TT_Semicolon){
		ret_expr = parseComparisonExpr(p);
		if (ret_expr == NULL){
			fprintf(stderr, "Compiler Error: Expected primary expression after 'rtrn' statement\n");
			p->has_error = true;
			return NULL;
		}
	}

	if (!parser_expect(p, TT_Semicolon, "Compiler Error: Expression ';' after 'rtrn' statement\n")){
		return NULL;
	}

    //AST* node, one of my favorites parts of the parser hehe...
    //Creating these AST nodes!
	Statement* ret_stmt = (Statement*)malloc(sizeof(Statement));
	ret_stmt->type = RETURN;
	ret_stmt->data.rtrn_s.value = ret_expr;

	return ret_stmt;
}

//PRINT, pRINT, prINT, priNT, prinT, print, yabadabadooo!!!!!!!!!!
Statement* parsePrintStatement(Parser* p){
	if (p == NULL || p->has_error) return NULL;

	if(!parser_expect(p, TT_Print, "Parser Error: Expected 'outlog' keyword to display text\n")){
		return NULL;
	}
	
	if (!parser_expect(p, TT_LParen, "Compiler Error: Expected '(' after 'outlog' method\n")){
		return NULL;
	}

	Expression* what_expr = parseComparisonExpr(p);
	if (what_expr == NULL){
		fprintf(stderr, "Compiler Error: Failed to parse expression in the 'outlog' method\n");
		p->has_error = true;
		return NULL;
	}

	if (!parser_expect(p, TT_RParen, "Compiler Error: Expected ')' after outlog statement method\n")){
		return NULL;
	}

	if (!parser_expect(p, TT_Semicolon, "Compiler Error: Expected ';' after the closing parenthesis in the outlog method\n")){
		return NULL;
	}

	//creating the print statement node...
	Statement* print_stmt = (Statement*)malloc(sizeof(Statement));
	print_stmt->type = PRINT;
	print_stmt->data.print_s.value = what_expr;

	return print_stmt;
}

Expression* parseInputExpression(Parser* p) {
    if (!parser_expect(p, TT_Input, "Compiler Error: Expected 'inlog'\n")) {
        return NULL;
    }

    if (!parser_expect(p, TT_LParen, "Compiler Error: Expected '(' after 'inlog'\n")) {
        return NULL;
    }
    if (!parser_expect(p, TT_RParen, "Compiler Error: Expected ')' after 'inlog('\n")) {
        return NULL;
    }

    Expression* expr = (Expression*)malloc(sizeof(Expression));
    expr->type = EXPR_FUNCTION_CALL;
    
    expr->data.func.func_name = strdup("bikt_input");
    expr->data.func.args = NULL;
    expr->data.func.arg_c = 0;

    return expr;
}

//This is like a scope, not like actually, IT IS a scope
/*
{
  statements....
}
*/
Statement* parseBlockStatement(Parser* p){
	if (p == NULL || p->has_error ) return NULL;
	
    if (!parser_expect(p, TT_LBrace, "Parser Error: Expected '{' to open block\n")){
        return NULL;
    }

    Statement** stmts = NULL;
    size_t stmt_count = 0;
    
    //checking for EOF and the Right Brace so as to know the end of the scope
    //and if its not a RBrace at the end, the expect down there checks that!
    while (parser_peek(p).type != TT_RBrace && parser_peek(p).type != TT_EOF && !p->has_error){
        Statement* stmt = parseStatement(p);
        if (stmt == NULL) continue;
        
        stmts = (Statement**)realloc(stmts, (stmt_count + 1) * sizeof(Statement*));
        stmts[stmt_count] = stmt;
        stmt_count++;   
    }

    if (!parser_expect(p, TT_RBrace, "Compiler Error: Expected '}' after block of statements/scope\n")){
        return NULL;
    }
    
    //AST Node...
    Statement* blk = (Statement*)malloc(sizeof(Statement));
    blk->type = BLOCK;
    blk->data.blk_s.statements = stmts;
    blk->data.blk_s.count = stmt_count;

    return blk;
}

//NO SHIT, this is a no brainer method...
//Even a beginner should look at the name and just write all code needed for it, am i right?
//Too easy....I've gone senile! ahahhahahaah!
Method* parseMethod(Parser* p){
    if (p == NULL || p->has_error) return NULL;

    if (!parser_expect(p, TT_Function, "Parser Error: Expected 'method' keyword\n")){
        return NULL;
    }

    if (parser_peek(p).type != TT_Identifier){
        fprintf(stderr, "Compiler Error: expected method name after 'method' keyword\n");
        p->has_error = true;
        return NULL;
    }

    char* method_name = (char*)malloc(strlen(parser_peek(p).value) + 1);
    strcpy(method_name, parser_peek(p).value);
    parser_advance(p);

    if (!parser_expect(p, TT_LParen, "Compiler Error: expected '(' after method name\n")){
        free(method_name);
        return NULL;
    }

    char** params = NULL;
    size_t param_count = 0;

    while (parser_peek(p).type != TT_RParen && parser_peek(p).type != TT_EOF && !p->has_error){
        if (parser_peek(p).type != TT_Identifier){
            fprintf(stderr, "Compiler Error: Expected variable identifier after parenthesis\n");
            free(method_name);
            p->has_error = true;
            return NULL;
        }

        char* param = (char*)malloc(strlen(parser_peek(p).value) + 1);
        strcpy(param, parser_peek(p).value);
        parser_advance(p);

        params = (char**)realloc(params, (param_count + 1) * sizeof(char*));
        params[param_count] = param;
        param_count++;

        if (parser_peek(p).type == TT_Comma){
            parser_advance(p);
        } else {
            break;
        }
    }

    if (!parser_expect(p, TT_RParen, "Compiler Error: Expected ')' after method parameters\n")){
        free(method_name);
        return NULL;
    }

    if (parser_peek(p).type != TT_LBrace){
        fprintf(stderr, "Compiler Error: Expected '{' to open method body\n");
        p->has_error = true;
        free(method_name);
        return NULL;
    }

    parser_advance(p);

    Statement** body = NULL;
    //body count? hahahahhahahaha! bruh what da hell!
    size_t body_count = 0; //hahahahahhaah!!!!

    while (parser_peek(p).type != TT_RBrace && parser_peek(p).type != TT_EOF && !p->has_error){
        Statement* stmt = parseStatement(p);
        if (stmt == NULL) continue;

        body = (Statement**)realloc(body, (body_count + 1) * sizeof(Expression*));
        body[body_count] = stmt;
        body_count++;
    }

    if (!parser_expect(p, TT_RBrace, "Compiler Error: Expected '}' after method body\n")){
        free(method_name);
        return NULL;
    }

    Method* mthd = (Method*)malloc(sizeof(Method));
    mthd->name = method_name;
    mthd->parameters = params;
    mthd->param_count = param_count;
    mthd->body = body;
    mthd->body_count = body_count;

    return mthd;
}

//The main power house of the parser!!
Program* parseProgram(Parser* p){
    if (p == NULL || p->has_error) return NULL;

    Program* program = (Program*)malloc(sizeof(Program));
    if (program == NULL){
        fprintf(stderr, "Failed to Allocate Program node!");
        return NULL;
    }

    //zero initialize everything
    //like how the people should also becomes zero initialized, get it? you prolly don't
    program->funcs = NULL;
    program->func_count = 0;
    program->statements = NULL;
    program->stmt_count = 0;

    while (parser_peek(p).type != TT_EOF && !p->has_error){
        //if its a method definition at the top and keeps track of how many methods (including method main(){})
        if (parser_peek(p).type == TT_Function){
            Method* mthd = parseMethod(p);

            if (mthd == NULL){
                fprintf(stderr, "Compiler Error: Failed to parse method %s definition\n", parser_peek(p).value);
                p->has_error = true;
                //no return NULL only p->has_error
                break;
            }

        program->funcs = (Method**)realloc(program->funcs, (program->func_count + 1) * sizeof(Method*));
        program->funcs[program->func_count] = mthd;
        program->func_count++;

        } else {
            Statement* stmt = parseStatement(p);
            
            if (stmt == NULL){
                fprintf(stderr, "Compiler Error: Unexpected token at top level %s\n", parser_peek(p).value);
                p->has_error = true;
                //no return NULL, only p->has_error
                break;
            }

            program->statements = (Statement**)realloc(program->statements, (program->stmt_count + 1) * sizeof(Statement*));
            program->statements[program->stmt_count] = stmt;
            program->stmt_count++;
        }
    }

    //return the program eve if there are errors as you saw above we didnt return NULL
    //like we normally do in some functions
    return program;
}

#endif
