#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#define ASM PL"Assembly"
#define QA  PLQ"QAsm"
#define QCP PLQ"QCode Plus"
#define BIN PL"Binary"

FILE* out;
struct QCP_Node* root;
int os = -1, rm_obj = 0;
extern int lnNmb;
extern char* fileIn;
void add(int t, const char* instruction) { fprintf(out, "%s%s\n", t ? "\t" : "", instruction); }
void adda(int t, const char* instruction) { fprintf(out, "%s %s\n", t ? "t." : ".", instruction); }
static int r(char* text) { printf(RED"%s\n"RST, text); return 1; }
static void g(char* text) { printf(RED"!"COMPLETE" %s "RED"!\n"RST, text); }
static void y(char* text) { printf(ONGOING"%s\n"RST, text); }
static void t(char* text) { printf(TEXT"%s\n"RST, text); }

struct QCP_Node* startCompiler() {
	#ifdef _WIN32
	os = 0;
	#elif __linux__
	os = 1;
	#endif
	if (os <= -1) { r("Error! Unable to recognize operating system!"); return NULL; }
	y("Starting compilation..."TEXT);
	printf(TEXT"<_/██"RED"██"COMPLETE"██"ONGOING"██"PL"██"PLQ"██"TEXT"\\_>\n");
	if (system("nasm -v") != 0) { t("Error: NASM is not installed or not in the PATH!"); return NULL; }
	y("Creating directory 'qcp_obj'...");
	if (system(os ? "mkdir -p qcp_obj" : "if not exist qcp_obj mkdir qcp_obj")) { r("Error! Could not create directory 'obj'"); return NULL; }
	g("Catalog 'qcp_obj' is created!");

	out = fopen(os ? "qcp_obj/out.qa" : "qcp_obj\\out.qa", "w");
	if (!out) { r("Cannot open/write a file!"); return NULL; }

	root = createNode(QCP_ROOT, "root", "");

	adda(0, "; db - 255");
	adda(0, "; dw - 65.535");
	adda(0, "; dd - 4.294.967.295");
	adda(0, "; dq - 18.446.744.073.709.551.615");
	add(0, "");
	adda(0, "; sm - start main");
	adda(0, "; em - end main");
	adda(0, "; step - function");
	adda(0, "; es - end function");
	adda(0, "; call - call");
	adda(0, "; prt - print string");
	adda(0, "; prtn - print number");
	adda(0, "; prti - print int");
	adda(0, "; prtln - print line");
	adda(0, "; add - add int");
	adda(0, "; sub - sub int");
	adda(0, "; mul - mul int");
	adda(0, "; div - div int");
	add(0, "");
	adda(0, "; QCode Plus v. 0.0.9");
	add(0, "data");
	add(1, ""); // global vars
	add(0, "end");
	add(0, "");
	add(0, "sm");
	add(1, ""); // main function
	add(0, "em");
	// other functions
	return root;
}
int endCompiler() {
	if (out && root) {
		fclose(out);
		free(root);
		y("Compiling "QA""ONGOING" to "ASM""ONGOING"...");
		if (system(os ? "./qasm \"qcp_obj/out.qa\" \"qcp_obj/out.asm\"" : "qasm.exe \"qcp_obj\\out.qa\" \"qcp_obj\\out.asm\"")) return r(QA""RED" compilation error!");
		g(QA""COMPLETE" to "ASM""COMPLETE" compilation complete!");
		y("Compiling "ASM""ONGOING" to "BIN""ONGOING"...");
		if (system(os ? "nasm -f elf64 qcp_obj/out.asm -o qcp_obj/out.o" : "nasm -f elf64 qcp_obj\\out.asm -o qcp_obj\\out.o")) return r(ASM" compilation error!");
		if (system(os ? "ld qcp_obj/out.o -o program" : "ld qcp_obj\\out.o -o program.exe")) return r("Linking error!");
		if (rm_obj) {
			t("Deleting the 'qcp_obj' directory...");
			if (system(os ? "rm -rf qcp_obj" : "rmdir /s /q qcp_obj")) return r("Error! Cannot remove directory 'qcp_obj'!");
		}
		printf(RED"!"COMPLETE" Compilation of file '%s' completed successfully! (%d lines) "RED"!\n"RST, fileIn, lnNmb);
		t(os ? "Running: './program'...\n" : "Running: 'program.exe'...\n");
		system(os ? "./program" : "program.exe");
	}
	return 0;
}
