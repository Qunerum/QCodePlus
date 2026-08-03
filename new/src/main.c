#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE 4096 + 1
#define MAX_SUB 1024 + 1
#define MAX_FUNC_LINES 2048
#define MAX_ARGS 256

void err(int i, char* text) { if (!i) return; printf("%s\n", text); exit(1); }

FILE *qasm;
typedef char _arg[MAX_ARGS][MAX_SUB];
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
void mvLbc(char* t, int cnt) { for (int i = 0; i < cnt; i++) mvL(t); }
void cutL(char* t, char c) { while(t[0] == c) mvL(t); }
void cutR(char* t, char c) { int l = len(t) - 1; while(t[l] == c && l > 0) { mvL(t); l = len(t) - 1; } }
void cpy(char* from, char* to) {
	int i = 0;
	while(from[i]) {
		to[i] = from[i];
		i++;
	}
	to[i] = '\0';
}
void cpyF(char* from, char* to, int cnt) {
	int i = 0;
	for (i = 0; i < cnt; i++) {
		if (!from[i]) return;
		to[i] = from[i];
	}
	to[i] = '\0';
}
int is(char* a, char* b) {
	while(*a && *b) {
		if (*a != *b) return 0;
		a++;
		b++;
	}
	return 1;
}
int startWith(char* t, char* st) {
	while (*t && *st) {
		if (*t != *st) return 0;
		t++;
		st++;
	}
	return 1;
}
int contains(char* t, char c) {
	while(*t != '\0') {
		if (*t == c) return 1;
		t++;
	}
	return 0;
}
int fnd(char* t, char c) {
	if (!contains(t, c)) return -1;
	int i = 0;
	while(t[i] != c) i++;
	return i + 1;
}
int fndR(char* t, char c) {
	if (!contains(t, c)) return -1;
	int i = len(t) - 1;
	while(t[i] != c && i > 0) i--;
	return i;
}
void trim(char* t, char c) {
	cutL(t, c);
	cutR(t, c);
}

void split(_arg args, int* argc, char* t, char c) {
	char bfr[MAX_LINE];
	cpy(t, bfr);
	printf("[");
	if (!contains(t, c)) {
		cpy(t, args[0]);
		trim(args[0], ' ');
		if (argc) *argc = 1;
		printf("'\033[1;38;5;34m%s\033[0m'] [%i]\n", args[0], *argc);
		return;
	}
	if (argc) *argc = 1;
	while (contains(bfr, c)) {
		int i = fnd(bfr, c);
		cpyF(bfr, args[0], i-1);
		trim(args[0], ' ');
		mvLbc(bfr, i);
		if (argc) *argc = *argc + 1;
		printf("'\033[1;38;5;34m%s\033[0m', ", args[0]);
	}
	cpy(bfr, args[0]);
	trim(args[0], ' ');
	printf("'\033[1;38;5;34m%s\033[0m'] [%i]\n", args[0], *argc);
}

typedef struct {
	char name[MAX_SUB];
} qcpVar;
// ! = = = = = = = = = = FUNCTIONS = = = = = = = = = = !
static int mainCreated = 1;
typedef struct {
	char name[MAX_SUB];
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
// ! = = = = = = = = = = COMMANDS = = = = = = = = = = !
typedef struct { int isBlock; char* cmd; void (*handler)(_arg, int); int args; } qcpCmd;
void qvFunc(_arg args, int argc) {
	//
}
qcpCmd cmds[] = {
	{0,"func", qvFunc, -1}
};
int cmdCount = sizeof(cmds) / sizeof(qcpCmd);
// ! = = = = = = = = = = MAIN = = = = = = = = = = !
#define LOG 1
int main() {
	FILE *file = fopen("program.qcp", "r");
	if (!file) {
		file = fopen("program.qcp", "w");
		fprintf(file, "func main() {\n");
		fprintf(file, "\tint num(26);\n\n");
		fprintf(file, "\tprintln(\"Decimal: \", v:num);\n");
		fprintf(file, "\tprintln(\"Binary (8): \", b:num'8);\n");
		fprintf(file, "\tprintln(\"HEX (4): \", h:num'4);\n");
		fprintf(file, "}\n");
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
		int ps = fnd(buffer, '('), pe = fndR(buffer, ')');
		if (ps < 0 || pe < 0 || pe < ps) continue;
		char argBfr[MAX_LINE];
		_arg args = {0};
		cpy(buffer, argBfr);
		mvLbc(argBfr, ps);
		pe = fndR(argBfr, ')');
		argBfr[pe] = '\0';
		// printf("%s\n", argBfr);
		int argc = 0;
		split(args, &argc, argBfr, ',');
		for (int i = 0; i < cmdCount; i++) {
			if (startWith(buffer, cmds[i].cmd)) {
				//
			}
		}
	}
	fclose(file);
	fclose(qasm);
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
