#include <stdio.h>
#include <stdlib.h>

typedef enum {
    TT_Return,
    TT_Number,
	TT_Let,
    TT_String,
    TT_Identifier,
    TT_Print,
    TT_Semicolon,
    TT_EOF,
    TT_Unknown,
    TT_LParen,
    TT_RParen,
    TT_LBrace,
    TT_RBrace,
    TT_Function,
    TT_If,
    TT_Else,
    TT_Comma,
    TT_Plus,
    TT_Minus,
    TT_Astk,
    TT_Slash,
	TT_Equals,
	TT_Colon,
	TT_Greater,
	TT_Less,
} TokenType;

typedef struct {
    TokenType type;
    char* value;
} Token;

