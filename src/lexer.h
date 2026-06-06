#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "token.h"

typedef struct {
	char* input;
	size_t pos;
	size_t line;
	size_t column;
} Lexer;

Lexer* initLexer(const char* input){
	if (input == NULL){
		fprintf(stderr, "Compiler Error: cannot init Lexer with NULL input\n");
		return NULL;
	}
	Lexer* l = (Lexer*)malloc(sizeof(Lexer));
	l->input = (char*)input;
	l->pos = 0;
	l->line = 1;
	l->column = 0;
	return l;
}

void destroyToken(Token* tk){
	if (tk == NULL) return;
	if (tk->value != NULL){
		free(tk->value);
		tk->value = NULL;
	}
}

void destroyLexer(Lexer* l){
	if (l == NULL) return;
	free(l);
	l = NULL;
}

bool is_alpha(char c){
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool is_digit(char c){
	return (c >= '0' && c <= '9');
}

bool is_alnum(char c){
	return is_digit(c) || is_alpha(c);
}

bool is_whitspace(char c){
	return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

char peek(Lexer* l){
	if (l->pos >= strlen(l->input)){
		return '\0';
	}
return l->input[l->pos];
}

char peek_next(Lexer* l){
	if (l->pos >= strlen(l->input)){
		return '\0';
	}
return l->input[l->pos + 1];
}

void advance(Lexer* l){
	char current = peek(l);
	if (current == '\n'){
		l->line++;
		l->column = 0;
	} else {
		l->column++;
	}
l->pos++;
}

char* scan_number(Lexer* l){
	size_t st = l->pos;

	while(is_digit(peek(l))){
		advance(l);
	}

	size_t len = l->pos - st;
	char* number = (char*)malloc(sizeof(len + 1));
	if (number == NULL){
		fprintf(stderr, "Generation Error: Failed to Allocate memory in scan_number\n");
	return NULL;
	}
	strncpy(number, &l->input[st], len);
	number[len] = '\0';

return number;
}

char* scan_string(Lexer* l){
	size_t st = l->pos;
	while (peek(l) != '"' && peek(l) != '\0'){
		advance(l);
	}

	if (peek(l) == '\0'){
		fprintf(stderr, "Compiler Error: Unterminated string at line %ld", l->line);
	return NULL;
	}

	size_t len = l->pos - st;
	char* str = (char*)malloc(len + 1);
	strncpy(str, &l->input[st], len);
	str[len] = '\0';

	advance(l);

return str;
}

void skip_space(Lexer* l){
	while (is_whitspace(peek(l))){
		advance(l);
	}
}

char* scan_identifier(Lexer* l){
	size_t st = l->pos;
	
	advance(l);

	while(is_alnum(peek(l))){
		advance(l);
	}

	size_t len = l->pos - st;
	char* identifier = (char*)malloc(len + 1);
	strncpy(identifier, &l->input[st], len);
	identifier[len] = '\0';

return identifier;
}

TokenType check_keyword(const char* txt){
	if (txt == NULL){
		return TT_Identifier;
	}

	if (strcmp(txt, "rtrn") == 0){
		return TT_Return;
	} else if (strcmp(txt, "outlog") == 0){
		return TT_Print;
	} else if (strcmp(txt, "inlog") == 0){
		return TT_Input;
	} else if (strcmp(txt, "method") == 0){
		return TT_Function;
	} else if (strcmp(txt, "if") == 0){
		return TT_If;
	} else if (strcmp(txt, "else") == 0){
		return TT_Else;
	} else if (strcmp(txt, "let") == 0){
		return TT_Let;
	}

return TT_Identifier;
}

TokenType get_single_char_token(char c){
	switch(c){
		case '(':
			return TT_LParen;
		case ')':
			return TT_RParen;
		case '{':
			return TT_LBrace;
		case '}':
			return TT_RBrace;
		case ';':
			return TT_Semicolon;
		case ',':
			return TT_Comma;
		case '+':
			return TT_Plus;
		case '-':
			return TT_Minus;
		case '/':
			return TT_Slash;
		case '*':
			return TT_Astk;
		case '=':
			return TT_Equals;
		case ':':
			return TT_Colon;
		case '<':
			return TT_Less;
		case '>':
			return TT_Greater;
		default:
			return TT_Unknown;
	}
}

Token nextToken(Lexer* l){
	Token tk;
	tk.value = NULL;

	skip_space(l);

	char current = peek(l);
	if (current == '\0'){
		tk.type = TT_EOF;
		return tk;
	}

	//Handle Strings...
	if (current == '"'){
		advance(l);
		char* val = scan_string(l);
		if (val == NULL){
			tk.type = TT_Unknown;
			return tk;
		}
		tk.type = TT_String;
		tk.value = val;
		return tk;
	}

	//Handle Numbers...
	if (is_digit(current)){
		char* val = scan_number(l);
		if (val == NULL){
			tk.type = TT_Unknown;
			return tk;
		}
		tk.type = TT_Number;
		tk.value = val;
		return tk;
	}

	//Handle Identifiers and Keywords...
	if (is_alpha(current)){
		char* id_val = scan_identifier(l);
		if (id_val == NULL){
			tk.type = TT_Unknown;
			return tk;
		}
		tk.type = check_keyword(id_val);
		tk.value = id_val;
		return tk;
	}

	//Handle single character tokens...
	TokenType sct = get_single_char_token(current);
	if(sct != TT_Unknown){
		char* val = (char*)malloc(2);
		val[0] = current;
		val[1] = '\0';
		advance(l);
		tk.type = sct;
		tk.value = val;
		return tk;
	}

	advance(l);
	tk.type = TT_Unknown;
	return tk;
}

#endif
