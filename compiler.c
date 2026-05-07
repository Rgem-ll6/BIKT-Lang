#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "lexer.h"

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
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file)){
        printf("%s", buffer);
    }

return 0;
}