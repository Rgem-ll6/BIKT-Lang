#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include "ast.h"

typedef struct {
    char* name;
    size_t offset;
} SymbolEntry;

typedef struct {
    SymbolEntry** entries;
    size_t count;
    size_t current_offset;
} SymbolTable;

typedef struct {
    FILE* output;
    SymbolTable* symtab;
    size_t label_count;
} CodeGen;

CodeGen* initCodeGen(const char* filename){
    CodeGen* cg = (CodeGen*)malloc(sizeof(*cg));
    cg->output = fopen(filename, "w");
    if (cg->output == NULL){
        fprintf(stderr, "CodeGen Fault: Could not open file <%s>\n", filename);
        return NULL;
    }
    cg->symtab = (SymbolTable*)malloc(sizeof(*(cg->symtab)));
    cg->symtab->entries = NULL;
    cg->symtab->count = 0;
    cg->symtab->current_offset = 8;
    cg->label_count = 0;
    return cg;
}

//work on this later...
void destroyCodeGen(CodeGen* cg){
    if (cg == NULL) return;
    free(cg->symtab);
    fclose(cg->output);
    free(cg);
}

//adding an entry to the symbol table..
bool symtab_add(SymbolTable* st, const char* name){
	if (st == NULL || name == NULL) return false;

	SymbolEntry* entry = (SymbolEntry*)malloc(sizeof(*entry));
	if (entry == NULL) return false;

	entry->name = (char*)malloc(strlen(name) + 1);
	if (entry->name == NULL){
		free(entry);
		return false;
	}
	strcpy(entry->name, name);

	entry->offset = st->current_offset;
	st->current_offset += 8;

	st->entries = (SymbolEntry**)realloc(st->entries, (st->count + 1) * sizeof(SymbolEntry*));
	st->entries[st->count] = entry;
	st->count++;
	
	return true;
}

//looking up the symbol table for a symbol entry ot return...
SymbolEntry* symtab_lookup(SymbolTable* st, const char* name){
	if (st == NULL || name == NULL) return NULL;

	for (size_t k = 0; k < st->count; ++k){
		if (strcmp(st->entries[k]->name, name) == 0){
			return st->entries[k];
		}
	}
	fprintf(stderr, "CodeGen Fault: Undefined variable %s", name);
	return NULL;
}

//in-line assembly hahahaha!!
//Welcome to the pinnacle of maddness 
//lalallallallalal!!!!!!!!!hahaaa
void emit(CodeGen* cg, const char* assembly, ...){
    if (cg == NULL || assembly == NULL) return;

    va_list args;
    va_start(args, assembly);
    vfprintf(cg->output, assembly, args);
    va_end(args);
    fprintf(cg->output, "\n");
}

void emit_comment(CodeGen* cg, const char* comment){
    if (cg == NULL || comment == NULL) return;
    fprintf(cg->output, " ;%s\n", comment);
}

//genuine in-line assembly
//this is what i call mercy
//genuine misery, hihihihhihi!!!
void genExpression(CodeGen* cg, Expression* expr){
    if (cg == NULL) return;

    switch (expr->type){
		case EXPR_STRING:
			emit_comment(cg, "string expression not yet supported");
			break;
		case EXPR_NUMBER:
			emit(cg, "	mov rax, %ld", expr->data.number.value);
			break;
		case EXPR_IDENTIFIER: {
			SymbolEntry* entry = symtab_lookup(cg->symtab, expr->data.identifier.value);
			if (entry == NULL) return;

			emit(cg, "	mov rax, [rbp - %zu]", entry->offset);
			break;
		}
		case EXPR_BINARY_OP: {
			genExpression(cg, expr->data.binOP.left);
			emit(cg, "	push rax");
			genExpression(cg, expr->data.binOP.right);
			emit(cg, "	mov rbx, rax");
			emit(cg, "	pop rax");

			if (strcmp(expr->data.binOP.operator, "+") == 0){
				emit(cg, "	add rax, rbx");
			} else if (strcmp(expr->data.binOP.operator, "-") == 0){
				emit(cg, "	sub rax, rbx");
			} else if (strcmp(expr->data.binOP.operator, "*") == 0){
				emit(cg, "	imul rax, rbx");
			} else if (strcmp(expr->data.binOP.operator, "/") == 0){
				emit(cg, "	cqo"); //sign extended rax into rdx:rax
				emit(cg, "	idiv rbx"); //result go into rbx
			}
			break;
		}
		case EXPR_UNARY_OP: {
			genExpression(cg, expr->data.uOP.operand);

			if (strcmp(expr->data.uOP.operator, "-") == 0){
				emit(cg, "	neg rax");
			}
			break;
		}
		case EXPR_FUNCTION_CALL: {
			const char* arg_reg[6] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
			for (size_t i = 0; i < expr->data.func.arg_c; ++i){
				genExpression(cg, expr->data.func.args[i]);

				if (i < 6){
					emit(cg, "	mov %s, rax", arg_reg[i]);
				} else {
					emit(cg, "	push rax");
				}
			}
			emit(cg, "	call %s", expr->data.func.func_name);
			break;
		}
	}
}

//in-line assembly keeps getting fun...
void genStatement(CodeGen* cg, Statement* st){
	if (cg == NULL || st == NULL) return;
	
	switch (st->type){
		case RETURN: {
			genExpression(cg, st->data.rtrn_s.value);
			emit(cg, "	leave");
			emit(cg, "	ret");
			break;
		}
		case LET: {
			symtab_add(cg->symtab, st->data.let_s.name);

			if (st->data.let_s.init != NULL){
				genExpression(cg, st->data.let_s.init);
			} else {
				emit(cg, "	xor rax, rax");
			}

			SymbolEntry* entry = symtab_lookup(cg->symtab, st->data.let_s.name);
			emit(cg, "	mov [rbp - %zu], rax", entry->offset);
			break;
		}
		case ASSIGN: {
			genExpression(cg, st->data.assign_s.value);

			SymbolEntry* entry = symtab_lookup(cg->symtab, st->data.assign_s.name);
			if (entry == NULL) return;

			emit(cg, "	mov [rbp - %zu], rax", entry->offset);
			break;
		}
		case PRINT: {
			genExpression(cg, st->data.print_s.value);
			emit(cg, "	mov rdi, rax");
			// this function doesn't exist yet...
			//we'll create it in our standard library, 'lib.h' probably....
			//after the codegen...
			emit(cg, "	call bikt_print"); //<- this is the function, if your confused, 'bikt_print'
			break;
		}
		case IF: {
			//the counter for labels(they are like functions for assembly)
			size_t label = cg->label_count++;
			genExpression(cg, st->data.if_s.condition);
			emit(cg, "	cmp rax, 0");
			emit(cg, "	je .L%zu_else", label);

			for (size_t i = 0; i < st->data.if_s.then_count; ++i){
				genStatement(cg, st->data.if_s.then_branch[i]);
			}

			emit(cg, "	jmp .L%zu_end", label);
			emit(cg, ".L%zu_else: ", label);
			if (st->data.if_s.else_branch != NULL){
				for (size_t i = 0; i < st->data.if_s.else_count; ++i){
					genStatement(cg, st->data.if_s.else_branch[i]);
				}
			}

			emit(cg , ".L%zu_end:", label);
			break;
		}
		case BLOCK: {
			for (size_t i = 0; i < st->data.blk_s.count; ++i){
				genStatement(cg, st->data.blk_s.statements[i]);
			}
			break;
		}
		case EXPR:{
			genExpression(cg, st->data.expr_s.expression);
			break;
		}
	}
}

void genMethod(CodeGen* cg, Method* method){
	if (cg == NULL || method == NULL) return;

	emit(cg, "%s: ", method->name);
	emit(cg, "	push rbp");
	emit(cg, "	mov rbp, rsp");
	//reserve 128 bytes of stack space for local variables
	emit(cg, "	sub rsp, 128");
	const char* arg_regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
	for (size_t i = 0; i < method->param_count && i < 6; ++i){
		symtab_add(cg->symtab, method->parameters[i]);
		SymbolEntry* ent = symtab_lookup(cg->symtab, method->parameters[i]);
		//store reguster value onto the stack
		emit(cg, "	mov [rbp - %zu], %s", ent->offset, arg_regs[i]);
	}

	for (size_t k = 0; k < method->body_count; ++k){
		genStatement(cg, method->body[k]);
	}

	emit(cg, "	leave");
	emit(cg, "	ret");
}

void genProgram(CodeGen* cg, Program* prog){
	if (cg == NULL || prog == NULL) return;

	emit(cg, "extern bikt_print");
	emit(cg, "extern bikt_input");
	emit(cg, "section .text");
	emit(cg, "global _start");
	emit(cg, "	extern bikt_print");
	emit(cg, "");

	for (size_t i = 0; i < prog->func_count; ++i){
		genMethod(cg, prog->funcs[i]);
		emit(cg, "");
	}

	emit(cg, "_start:");
	emit(cg, "	call main");
	emit(cg, "	mov rax, 60");
	emit(cg, "	mov rdi, 0");
	emit(cg, "	syscall");
}

#endif
