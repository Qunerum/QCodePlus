#include <stdlib.h>
#include <stdio.h>
#include "main.h"

FILE* out;
struct QCP_Node* root;
int os = -1, rm_obj = 0;
void add(int t, const char* instruction) { fprintf(out, "%s%s\n", t ? "\t" : "", instruction); }

struct QCP_Node* startCompiler() {
	#ifdef _WIN32
	os = 0;
	#elif __linux__
	os = 1;
	#endif
	if (os <= -1) { printf("Error! Unable to recognize operating system!\n"); return NULL; }
	printf("Starting compilation...\n");

	if (system("nasm -v") != 0) { printf("Error: NASM is not installed or not in the PATH!\n"); return NULL; }

	printf("Creating directory 'qcp_obj'...\n");
	if (system(os ? "mkdir -p qcp_obj" : "if not exist qcp_obj mkdir qcp_obj")) { printf("Error! Could not create directory 'obj'\n"); return NULL; }
	printf("Catalog 'qcp_obj' is created!\n");

	out = fopen(os ? "qcp_obj/out.asm" : "qcp_obj\\out.asm", "w");
	if (!out) { printf("Cannot open/write a file!\n"); return NULL; }

	root = createNode(QCP_ROOT, "root", "");

	add(0, "; db - 255");
	add(0, "; dw - 65.535");
	add(0, "; dd - 4.294.967.295");
	add(0, "; dq - 18.446.744.073.709.551.615");
	add(0, "");
	add(0, "; QCode Plus v. 0.0.9");
	add(0, "default rel");
	add(0, "; = = = = = = = = = = VARIABLES = = = = = = = = = =");
	add(0, "section .data");
	add(1, "nl db 10");
	add(1, "nll equ $ - nl");
	// add values
	add(1, "qcp_int_x dd 15 ; int x = 15; | QCP_INT");

	add(1, "qcp_l_main_int_y dd 8 ; int y = 8; | QCP_LOCAL_INT");

	add(0, "section .bss");
	add(1, "itt_bfr resb 12");
	// add allocation
	add(0, "; = = = = = = = = = = INT TO TEXT = = = = = = = = = =");
	add(0, "section .text");
	add(1, "global _start");
	add(0, "intToText:");
	add(1, "mov rcx, 0");
	add(1, "mov ebx, 10");
	add(0, ".ittLoop:");
	add(1, "mov edx, 0");
	add(1, "div ebx");
	add(1, "add edx, 48");
	add(1, "push rdx");
	add(1, "inc rcx");
	add(1, "cmp eax, 0");
	add(1, "jne .ittLoop");
	add(1, "lea rdi, [rel itt_bfr]");
	add(1, "mov rdx, rcx");
	add(1, "lea rsi, [rel itt_bfr]");
	add(0, ".ittLoopWrite:");
	add(1, "pop rax");
	add(1, "mov [rdi], al");
	add(1, "inc rdi");
	add(1, "loop .ittLoopWrite");
	add(1, "inc rdx");
	add(1, "ret");
	add(0, "; = = = = = = = = = = PRINT = = = = = = = = = =");
	add(0, "prt:");
	add(1, "mov rax, 1");
	add(1, "mov rdi, 1");
	add(1, "syscall");
	add(1, "ret");
	add(0, "prtln:");
	add(1, "lea rsi, [rel nl]");
	add(1, "mov rdx, nll");
	add(1, "mov rax, 1");
	add(1, "mov rdi, 1");
	add(1, "syscall");
	add(1, "ret");
	add(0, "; = = = = = = = = = = CODE = = = = = = = = = =");
	add(0, "_start:");
	// main function
	add(1, "mov eax, [rel qcp_l_main_int_y] ; x += y;");
	add(1, "add dword [rel qcp_int_x], eax ; x += y;");

	add(1, "mov eax, [rel qcp_int_x] ; if (x == 23)");
	add(1, "cmp eax, 23 ; if (x == 23)");
	add(1, "jne .if_0_end ; if (x == 23)");

	add(1, "mov eax, [rel qcp_int_x] ; print(x 10);");
	add(1, "call intToText ; print(x 10);");
	add(1, "call prt ; print(x 10);");
	add(1, "call prtln ; print(x 10);");

	add(0, ".if_0_end:");
	return root;
}
/*
_start:
	add dword [rel vint_x], 10 ; Add
	lea rsi, [rel vstr_text]
	mov rdx, len_text
	call prt ; Print string
	sub dword [rel vint_x], 2 ; Sub
	add dword [rel vint_x], 5 ; Add
	mov eax, [rel vint_x]
	call intToText
	call prt ; Print int
	lea rsi, [rel nl]
	mov rdx, nll
	call prt ; Print line
	mov rax, 60
	mov rdi, 0
	syscall
*/


int endCompiler() {
	if (out && root) {
		add(1, "; = = = END = = =");
		add(1, "mov rax, 60");
		add(1, "mov rdi, 0");
		add(1, "syscall");



		fclose(out);
		free(root);
		printf("Compiling to binary...\n");
		if (system(os ? "nasm -f elf64 qcp_obj/out.asm -o qcp_obj/out.o" : "nasm -f elf64 qcp_obj\\out.asm -o qcp_obj\\out.o")) { printf("Assembler compilation error!\n"); return 1; }
		if (system(os ? "ld qcp_obj/out.o -o program" : "ld qcp_obj\\out.o -o program.exe")) { printf("Linking error!\n"); return 1; }

		if (rm_obj) {
			printf("Deleting the 'qcp_obj' directory...\n");
			if (system(os ? "rm -rf qcp_obj" : "rmdir /s /q qcp_obj")) { printf("Error! Cannot remove directory 'qcp_obj'!\n"); return 1; }
		}

		printf("Compilation completed successfully!\n");
		printf(os ? "Running: './program'\n\n" : "Running: 'program.exe'\n\n");
		system(os ? "./program" : "program.exe");
	}
	return 0;
}
