#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "parser.h"
#include "codegen.h"

#define COMPILER_VERSION "1.1.3"

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

void printHelp(const char* prog_name) {
    printf("BIKT (Built-In Knowledge Translator) Compiler\n");
    printf("Usage: %s <filename.bikt> [options]\n\n", prog_name);
    printf("Options:\n");
    printf("  -v, --version   Display compiler version details\n");
    printf("  -h, --help      Display this helper documentation info\n");
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        fprintf(stderr, "fbc:fatal: Needs a <file.bikt> input\n");
	fprintf(stderr, "fbc:fatal: No input files detected\nCompilation Aborted!\n");
        return -1;
    }

    if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
         printf("FBC version %s\n", COMPILER_VERSION);
		 printf("Author: Ugwu Rhema\n");
		 printf("This version is prototype test version\n");
		 printf("BIKT-OS Integrated!\n");
         return 0;
    }
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
         printHelp(argv[0]);
         return 0;
    }

    if (!isValidBiktFile(argv[1])) {
        fprintf(stderr, "fbc:Compiler Error: input file is not '.bikt' format\n");
	fprintf(stderr, "fbc:Compilation Terminated!\n");
        return -1;
    }

    FILE* file = fopen(argv[1], "r");
    if (file == NULL) {
        fprintf(stderr, "Compiler Error: Could not open file %s\n", argv[1]);
        return -1;
    }

    char* sourceCode = readFile(file);
    fclose(file);

    Parser* parser = initParser(sourceCode);
    if (parser == NULL) {
        fprintf(stderr, "Compiler Error: Failed to initialize Parser!\n");
        free(sourceCode);
        return -1;
    }

    Program* ast = parseProgram(parser);
    if (parser->has_error) {
        fprintf(stderr, "FBC: Parsing failed - Aborting\n");
        free(sourceCode);
        free(parser);
        return -1;
    }

    char* base = getBaseName(argv[1]);

    char asm_path[256];
    char obj_path[256];
    char bin_path[256];
    
    snprintf(asm_path, sizeof(asm_path), "%s.asm", base);
    snprintf(obj_path, sizeof(obj_path), "%s.o", base);
    snprintf(bin_path, sizeof(bin_path), "%s", base);

    CodeGen* cg = initCodeGen(asm_path);
    if (cg == NULL) {
        fprintf(stderr, "Compiler Error: failed to initialize Code Gen\n");
        free(sourceCode);
        free(base);
        return -1;
    }

    genProgram(cg, ast);
    destroyCodeGen(cg);

    char compiler_dir[512] = {0};
    ssize_t len = readlink("/proc/self/exe", compiler_dir, sizeof(compiler_dir) - 1);
    if (len != -1) {
        compiler_dir[len] = '\0';
        char* last_slash = strrchr(compiler_dir, '/');
        if (last_slash) {
            *last_slash = '\0'; 
        }
    } else {
        strcpy(compiler_dir, ".");
    }

    char runtime_asm[512];
    char runtime_obj[512];
    snprintf(runtime_asm, sizeof(runtime_asm), "%s/bikt_runtime.asm", compiler_dir);
    snprintf(runtime_obj, sizeof(runtime_obj), "%s/bikt_runtime.o", compiler_dir);

    char runtime_cmd[1024];
    snprintf(runtime_cmd, sizeof(runtime_cmd), "nasm -f elf64 \"%s\" -o \"%s\"", runtime_asm, runtime_obj);
    system(runtime_cmd);

    char nasm_cmd[1024];
    snprintf(nasm_cmd, sizeof(nasm_cmd), "nasm -f elf64 %s.asm -o %s.o", base, base);
    system(nasm_cmd);

    char gcc_cmd[1024];
    snprintf(gcc_cmd, sizeof(gcc_cmd), "gcc -nostartfiles -no-pie %s.o \"%s\" -o %s", base, runtime_obj, base);
    system(gcc_cmd);

    // Clean up allocated heap strings
    free(sourceCode);
    free(base);

    return 0;
}
