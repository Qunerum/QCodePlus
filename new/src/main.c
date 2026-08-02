#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE 4096 + 1
#define MAX_NAME 1024 + 1
#define MAX_FUNC_LINES 2048

void err(int i, char* text) { if (!i) return; printf("%s\n", text); exit(1); }

FILE *qasm;
int len(char* t) {
	int x = 0;
	while(t[x] != '\0') x++;
	return x;
}
void mvL(char* t) {
	int l = len(t);
	if (l == 1) { t[0] = '\0'; return; }
	for (int i = 1; i <= l; i++) t[i-1] = t[i];
}
void cutL(char* t, char c) { while(t[0] == c) mvL(t); }
void cpy(char* a, char* b) { int i = 0; while(a[i]) { b[i] = a[i]; i++; } }
int is(char* a, char* b) { while(*a && *b) { a++; b++; if (*a != *b) return 0; } return 1; }


typedef struct {
	char name[MAX_NAME];
} qcpVar;
// ! = = = = = = = = = = FUNCTIONS = = = = = = = = = = !
static int mainCreated = 1;
typedef struct {
	char name[MAX_NAME];
	int lineCount;
	char lines[MAX_FUNC_LINES][MAX_LINE];
} qcpFunc;
qcpFunc* funcs;
int funcCount = 0;
void createFunc(char* name) {
	qcpFunc* new = realloc(funcs, (funcCount + 1) * sizeof(qcpFunc));
	err(!new, "Cannot add a function!");
	funcs = new;
	cpy(name, funcs[funcCount].name);
	if (is(name, "main")) mainCreated = 1;
	funcs[funcCount].lineCount = 0;
	funcCount++;
}
void addLine(char* line) {
	int i = funcCount - 1;
	cpy(line, funcs[i].lines[funcs[i].lineCount]);
	funcs[i].lineCount++;
}
// ! = = = = = = = = = = MAIN = = = = = = = = = = !
#define LOG 1
int main() {
	FILE *file = fopen("program.qcp", "r");
	if (!file) {
		file = fopen("program.qcp", "w");
		fprintf(file, "func main() {\n\tint num = 26;\n\tprint(\"Decimal: \" v:num 10 \"Binary (8): \" b:num,8 10 \"HEX (4): \" h:num,4 10);\n}\n");
		fclose(file);
	}
	file = fopen("program.qcp", "r");
	if (!file) { printf("Failed to open qcp file!\n"); return 1; }
	qasm = fopen(".obj/out.qa", "w");
	char buffer[MAX_LINE];
	while (fgets(buffer, sizeof(buffer), file) != NULL) {
		if (buffer[0] == '\n' || buffer[0] == '\0') continue;
		int l = len(buffer);
		if (buffer[l - 1] == '\n') { buffer[l - 1] = '\0'; l--; }
		cutL(buffer, ' ');
		cutL(buffer, '\t');
		printf("%s\n", buffer);
	}
	fclose(file);
	fclose(qasm);
	createFunc("main");
	addLine("int num = 26;");
	addLine("print(\"Decimal: \" v:num 10 \"Binary (8): \" b:num,8 10 \"HEX (4): \" h:num,4 10);");
	if (LOG) {
		printf("FUNCS:\n");
		for (int i = 0; i < funcCount; i++) {
			printf("- %s:\n", funcs[i].name);
			for (int l = 0; l < funcs[i].lineCount; l++) printf("---|%s\n", funcs[i].lines[l]);
		}
	}
	if (!mainCreated) return 1;
	return 0;
}
