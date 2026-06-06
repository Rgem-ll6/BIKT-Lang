#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "parser.h"
#include "codegen.h"

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

char* getBaseName(const char* path){
	const char* base = strrchr(path, '/');
	base = base ? base + 1 : path;

	const char* dot = strrchr(base, '.');
	size_t len = dot ? (size_t)(dot - base) : strlen(base);

	char* name = (char*)malloc(len + 1);
	strncpy(name, base, len);
	name[len] = '\0';
	return name;
}

int main(int argc, char** argv)
{
    if (argc < 2){
       	fprintf(stderr, "\nRuntime Error: needs <file.bikt> argument!\nOr access other flags like '-v' or '--version' to display the current version\n\n");
    	return -1;  
    }
	if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0){
		printf("\nFBC - First Bikt Compiler (test prototype) v1.0.0\n");
		printf("Author: Ugwu Rhema\n\n");
		return 0;
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
    fclose(file);
    
	//Stage 1, parsing and tokenizing...collab
	Parser* parser = initParser(sourceCode);
	if (parser == NULL){
		fprintf(stderr, "Compiler Error: Failed to initialize Parser\n");
		free(sourceCode);
		return -1;
	}

	Program* ast = parseProgram(parser);
	if (parser->has_error){
		fprintf(stderr, "FBC: Parsing failed - Aborting\n");
		free(sourceCode);
		free(parser);
		return -1;
	}

	char* base = getBaseName(argv[1]);

	char asm_path[256];
	char obj_path[256];
	char bin_path[256];
	snprintf(asm_path, sizeof(asm_path), "output/%s.asm", base);
	snprintf(obj_path, sizeof(obj_path), "output/%s.o", base);
	snprintf(bin_path, sizeof(bin_path), "output/%s", base);

	CodeGen* cg = initCodeGen(asm_path);
	if (cg == NULL){
		fprintf(stderr, "Compiler Error: failed to initialize Code Gen\n");
		free(sourceCode);
		free(base);
		return -1;
	}

	genProgram(cg, ast);
	destroyCodeGen(cg);

	system("nasm -f elf64 bikt_runtime.asm -o ../output/bikt_runtime.o");

	char nasm_cmd[1024];
	snprintf(nasm_cmd, sizeof(nasm_cmd), "nasm -f elf64 %s -o %s", asm_path, obj_path);
	if (system(nasm_cmd) != 0){
		fprintf(stderr, "FBC: NASM Assembly failed, aborting!");
		free(sourceCode);
		free(base);
		return -1;
	}

	char ld_cmd[1024];
	snprintf(ld_cmd, sizeof(ld_cmd), "ld -o %s %s ..output/bikt_runtime.o", bin_path, obj_path);
	if(system(ld_cmd) != 0){
		fprintf(stderr, "FBC: Linking failed, aborting!");
		free(sourceCode);
		free(base);
		return -1;
	}

	printf("FBC: Build Successful -> %s\n", bin_path);
	
	free(sourceCode);
	free(base);
	free(parser);

    return 0;
}
