#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    TT_Return,
    TT_Number,
    TT_String,
    TT_Identifier,
    TT_Print,
    TT_Semicolon,
    TT_EOF,
    TT_Unknown,
    TT_Lparen,
    TT_Rparen,
    TT_LBrace,
    TT_RBrace,
    TT_Function,
    TT_If,
    TT_Else,
    TT_Comma,
    TT_Plus,
    TT_Minus,
    TT_Astk,
    TT_Slash
} TokenType;

typedef struct {
    TokenType type;
    char* value;
} Token;

typedef struct {
    const char* input;
    size_t inputLen;
    size_t pos;
    size_t line;
    char ch;
} Lexer;

Lexer* New(const char* str){
    Lexer* lx = (Lexer*)malloc(sizeof(*lx));
    lx->inputLen = strlen(str);
    lx->line = 1;
    lx->pos = 0;
    lx->input = str;
return lx;
}

//HELPER FUNCTIONS
char peek (Lexer* l, int offset){
    int posn = l->pos + offset;
    if (l->pos >= l->inputLen){
        return '\0';
    }
return l->input[posn];
}

void readCurrentAndAdvance(Lexer* l){
    if (l->pos >= l->inputLen){
        l->ch = l->input[l->pos] == '\0';
    } else {
    l->ch = l->input[l->pos];
    }
 l->pos++; 
}

bool isDigit (char c){
    return c >= '0' && c <= '9';
}

bool isLetter (char c){
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool isIdentifier (char c){
    return isLetter(c) || isDigit(c);
}

Token makeT (TokenType t, char* val){
    Token tk;
    tk.type = t;
    tk.value = val;
return tk;
}

Token readNum (Lexer* l){
    size_t st = l->pos - 1;

    while (isDigit(peek(l, 1))){
        readCurrentAndAdvance(l);
    }

    size_t len = l->pos - st;
    char* num = malloc(len + 1);
    strncpy(num, &l->input[st], len);
    num[len] = '\0';

return makeT(TT_Number, num);    
}

TokenType lookUpKey(char* str){
    if (strcmp(str, "rtrn") == 0){
        return TT_Return;
    } else if (strcmp(str, "puts") == 0){
        return TT_Print;
    } else if (strcmp(str, "func") == 0){
        return TT_Function;
    } else if (strcmp(str, "If") == 0){
        return TT_If;
    } else if (strcmp(str, "Else") == 0){
        return TT_Else;
    } else {
        return TT_Identifier;
    }
}

Token readIdentifier (Lexer* x){
    size_t st = x->pos - 1;
    while (isIdentifier(peek(x, 1))){
        readCurrentAndAdvance(x);
    }

    size_t len = x->pos - st;
    char* iden = malloc(len + 1);
    strncpy(iden, &x->input[st], len);
    iden[len] = '\0';

    TokenType type = lookUpKey(iden);
return makeT(type, iden);
}

Token readStr (Lexer* e){
    readCurrentAndAdvance(e);
    size_t st = e->pos;

    while(e->ch != '"' && e->ch != '\0'){
        if (e->ch == '\n'){
            e->line++;
        }
    readCurrentAndAdvance(e);
    }

    size_t len = e->pos - st;
    char* str = malloc(len + 1);
    strncpy(str, &e->input[st], len);
    str[len] = '\0';

    if (e->ch == '"'){
        readCurrentAndAdvance(e);
    }

return makeT(TT_String, str);    
}

void skws (Lexer* l){
    while (l->ch == ' ' || l->ch == '\t' || l->ch == '\n'){
        if (l->ch == '\n'){
            l->line++;
        }
    readCurrentAndAdvance(l);
    }
}

Token nextToken (Lexer* r){
    skws(r);

    if (r->pos >= strlen(r->input)){ 
        Token tk;
        tk.value = "";
        tk.type = TT_EOF;
        return tk;
    }

    Token t;
    if (r->ch == '\0'){
        return makeT(TT_EOF, "");
    } else if (isDigit(r->ch)){
        return readNum(r);
    } else if (isLetter(r->ch)){
        return readIdentifier(r);
    } else if (r->ch == '"'){
        return readStr(r);
    }

    switch (r->ch){
        case '(':
            t = makeT(TT_Lparen,"(");
            readCurrentAndAdvance(r);
            return t;
        case ')':
            t  = makeT(TT_Rparen, ")");
            readCurrentAndAdvance(r);
            return t;
        case ';':
            t = makeT(TT_Semicolon, ";");
            readCurrentAndAdvance(r);
            return t;
        case '{':
            t = makeT(TT_LBrace, "{");
            readCurrentAndAdvance(r);
            return t;
        case '}':
            t = makeT(TT_RBrace, "}");
            readCurrentAndAdvance(r);
            return t;
        case '+':
            t = makeT(TT_Plus, "+");
            readCurrentAndAdvance(r);
            return t;
        case '-':
            t = makeT(TT_Minus, "-");
            readCurrentAndAdvance(r);
            return t;
        case '/':
            t = makeT(TT_Slash, "/");
            readCurrentAndAdvance(r);
            return t;
        case '*':
            t = makeT(TT_Astk, "*");
            readCurrentAndAdvance(r);
            return t;
    }

    t = makeT(TT_Unknown, &r->ch);
    readCurrentAndAdvance(r);
    return t;
}

#endif