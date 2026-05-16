#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "parser.h"

char* readFile(FILE* file) {
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* buffer = (char*)malloc(size + 1);
    fread(buffer, 1, size, file);
    buffer[size] = '\0';
    
    return buffer;
}

bool isValidBiktFile(char* str){
    if (strlen(str) < 5) return false;
return strcmp(str + strlen(str) - 5, ".bikt") == 0;
}

int main(int argc, char** argv)
{
    if (argc != 2){
        fprintf(stderr, "Runtime Error: needs <file.bikt> argument!\n");
    return -1;  
    }
    FILE* file = fopen(argv[1], "r");
    if (file == NULL){
        fprintf(stderr, "Runtime Error: Failed to open file '%s'\n", argv[1]);
    return -1;
    }
    if (!isValidBiktFile(argv[1])){
        fprintf(stderr, "Runtime Error: file must end with '.bikt'\n");
    return -1;
    }

    char* sourceCode = readFile(file);
	if (sourceCode == NULL){
		fprintf(stderr, "Compiler Error: Failed to read %s, its empty\n", argv[1]);
		return -1;
	}
	printf("%s\n\n", sourceCode);
    fclose(file);
    
    Lexer* lexer = initLexer(sourceCode);
	if (lexer == NULL){
		fprintf(stderr, "Error: Failed to initialize Lexer!\n");
		return -1;
	}
	
	printf("***TOKENIZING***\n");
	Token tk = nextToken(lexer);
	int tcount = 0;
	
	while (tk.type != TT_EOF){
		tcount++;

		printf("Token %d: %d", tcount, tk.type);
		if (tk.value != NULL){
			printf(", Value: %s", tk.value);
		}

		printf("\n");	
		destroyToken(&tk);
		tk = nextToken(lexer);
	}

    free(sourceCode);
    destroyLexer(lexer);
    return 0;
}
